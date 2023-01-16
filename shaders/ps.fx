

Texture2D albedoTex : register(t0);
SamplerState wrapSampler : register(s0);


struct PS_IN {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float3 tan : TANGNET0;
	float3 binor : BINORMAL0;
	float2 tex : TEXCOORD0;
};

float4 main(PS_IN input) : SV_Target0 {
	//return float4(input.tex, 0.0f, 0.0f);
	return albedoTex.Sample(wrapSampler, input.tex);
}