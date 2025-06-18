//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 uv : TEXCOORD0;		//	xy: uv; z: page index
};

Texture2DArray g_texArrayVPages : register(t0);		//	VPages
SamplerState g_samplerVPage : register(s0);	//	VPage sampler

float4 ps_main(PS_INPUT input) : SV_TARGET
{
	float4 color = g_texArrayVPages.Sample(g_samplerVPage, input.uv);
	return color;
}

