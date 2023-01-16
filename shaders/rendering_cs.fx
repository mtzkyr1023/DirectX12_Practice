

static const uint SuperSampledAA = 100;
static const uint TriangleSampledAA = 200;
static const uint StencilAA = 440;
static const uint WireframeAA = 450;

static const uint AAMode = SuperSampledAA;

static const float MipMapLodBias = 0.5f;
static const uint SamplesPerPixel = 4;
static const float InvSpp = 0.25f;
static const uint TileSize = 128;
static const uint TileSizeX = 16;
static const uint TileSizeY = 8;
static const uint InstanceIdMask = 1023;
static const uint InstanceIdBits = 0;
static const uint MaxShadingSamplesPerTile = 512;


struct VertexInput
{
   float4 position  ;
   float4 tangent   ;
   float4 bitangent ;
   float4 texCoord  ;
};

struct VertexOutput
{
   float4 position  ;
   float4 tangent   ;
   float4 bitangent ;
   float4 texCoord  ;

   float4 clipSpacePosition : SV_POSITION;
};

cbuffer __SystemConstant : register(b0) {
	matrix InvViewProjMatrix;
	matrix ViewMatrix;
	float4 WorldSpaceCameraPosition;
	uint2 ImageSize;
	float isVB;
	uint ShaderId;
	matrix Padding2;
	float4x2 Padding3;
}

Texture2D<uint2> visibilityBuffer : register(t0);
StructuredBuffer<VertexInput> vertexBuffer : register(t1);
StructuredBuffer<uint> indexBuffer : register(t2);
StructuredBuffer<uint4> instanceToIndexMap : register(t3);
Texture2D<float4> albedoTexture[] : register(t4);
SamplerState wrapSampler : register(s0);

RWTexture2D<float4> resultTex : register(u0);

VertexInput __loadVertex(uint bufferIndex)
{
   VertexInput vIn;
   vIn.position  = vertexBuffer[bufferIndex].position;
   vIn.tangent   = vertexBuffer[bufferIndex].tangent;
   vIn.bitangent = vertexBuffer[bufferIndex].bitangent;
	
	
	
   vIn.texCoord = vertexBuffer[bufferIndex].texCoord;

   return vIn;
}

float3 __intersect(float3 p0, float3 p1, float3 p2, float3 o, float3 d)
{
   float3 eo =  o - p0;
   float3 e2 = p2 - p0;
   float3 e1 = p1 - p0;
   float3 r  = cross(d, e2);
   float3 s  = cross(eo, e1);
   float iV  = 1.0f / dot(r, e1);
   float V1  = dot(r, eo);
   float V2  = dot(s,  d);
   float b   = V1 * iV;
   float c   = V2 * iV;
   float a   = 1.0f - b - c;
   return float3(a, b, c);
}

VertexInput __loadAndInterpolateVertex(uint tID, uint iId, float2 pixelCoord, out float filterWidth)
{
   uint indexOffset = indexOffsetBuffer[iId].indexOffset;
   uint index0 = indexBuffer[ tID * 3 + 0 + indexOffset ];
   uint index1 = indexBuffer[ tID * 3 + 1 + indexOffset ];
   uint index2 = indexBuffer[ tID * 3 + 2 + indexOffset ];

   VertexInput v0 = __loadVertex( index0 );
   VertexInput v1 = __loadVertex( index1 );
   VertexInput v2 = __loadVertex( index2 );
   
   v0.position = mul(v0.position, ViewMatrix);
   v1.position = mul(v1.position, ViewMatrix);
   v2.position = mul(v2.position, ViewMatrix);
   
   //v0.tangent.xyz = normalize(v0.tangent.xyz + v1.tangent.xyz + v2.tangent.xyz);
   
      
   float4 d = float4(pixelCoord / ImageSize.xy * float2(2.0f, -2.0f) - float2(1.0f, -1.0f), 0.1f, 1.0f);
   d = mul(d, InvViewProjMatrix);
   d /= d.w;
   
   d = normalize(d);
      
   float4 p0 = v0.position;
   float4 p1 = v1.position;
   float4 p2 = v2.position;

   /// Compute the barycentric coordinates
   float3 H  = __intersect(p0.xyz, p1.xyz, p2.xyz, float3(0.0f, 0.0f, 0.0f), d.xyz);

   VertexInput vIn;
   vIn.position  = mad(v0.position,  H.x, mad(v1.position,  H.y, (v2.position  * H.z)));
   vIn.tangent   = mad(v0.tangent,   H.x, mad(v1.tangent,   H.y, (v2.tangent   * H.z)));
   vIn.bitangent = mad(v0.bitangent, H.x, mad(v1.bitangent, H.y, (v2.bitangent * H.z)));
   vIn.texCoord  = mad(v0.texCoord,  H.x, mad(v1.texCoord,  H.y, (v2.texCoord  * H.z)));
	
	
   return vIn;
}




[numthreads(16,8,1)]
void main(uint3 globalThreadId : SV_DispatchThreadID,
	uint3 tileThreadId : SV_GroupThreadID,
	uint3 groupId : SV_GroupID) {
	
	const uint2 PixelCoord = uint2(globalThreadId.xy
	);
	
	const uint PixelIndex = PixelCoord.x + ImageSize.x * PixelCoord.y;
	uint groupThreadId = tileThreadId.x + tileThreadId.y * TileSizeX;
	
	uint2 vData = visibilityBuffer[PixelCoord];
	uint tId = vData.y;
	uint iId = vData.x;
	uint sId = instanceToIndexMap[iId];
	//if (sId > 10) return;
	
	float2 ImageCoord = float2(PixelCoord.x + 0.5f, PixelCoord.y + 0.5f) / ImageSize;
	float FilterWidth = 1.0f;
	
	VertexInput vIn = __loadAndInterpolateVertex(tId, iId, (float2)PixelCoord, FilterWidth);
	
	//resultTex[PixelCoord] = float4((float2)vData / 100000.0f, 0.0f, 1.0f);
	
	float intensity = saturate(dot(normalize(float3(1, 1, 1)), vIn.tangent.xyz)) * 0.5f + 0.5f;
	intensity *= intensity;
	
	resultTex[PixelCoord] = float4((float)(iId % 8) / 8.0f, 0.0f, 0.0f, 0.0f);
	//resultTex[PixelCoord] = float4(intensity, intensity, intensity, 1.0f);
	
	float4 color = albedoTexture[sId].SampleLevel(wrapSampler, vIn.texCoord.xy, 0);
	
	if (isVB > 0.0f)
		color = float4((float)(tId % 128) / 128.0f, (float)(iId % 128) / 128.0f, 0.0f, 0.0f);
	
	resultTex[PixelCoord] = color;
}