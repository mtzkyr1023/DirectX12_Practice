

struct CB0 {
	float4x4 view;
	float4x4 proj;
	float4x4 world;
};

cbuffer MatrixBuffer : register(b0) {
	CB0 cb0;
}

struct VS_IN {
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float3 tan : TANGENT0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float3 tan : TANGNET0;
	float3 binor : BINORMAL0;
	float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN input) {
	VS_OUT output = (VS_OUT)0;
	
	output.pos = mul(cb0.world, float4(input.pos, 1));
	output.pos = mul(cb0.view, output.pos);
	output.pos = mul(cb0.proj, output.pos);
	
	output.nor = normalize(mul((float3x3)cb0.world, input.nor));
	output.tan = normalize(mul((float3x3)cb0.world, input.tan));
	
	output.binor = cross(output.nor, output.tan);
	
	output.tex = input.tex;
	
	return output;
}