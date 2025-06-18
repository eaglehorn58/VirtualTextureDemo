//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 uv_pos : TEXCOORD0;
};

Texture2D g_tex : register(t0);			//	texture will be rendering
SamplerState g_sampler : register(s0);	//	texture sampler

float4 ps_main(PS_INPUT input) : SV_TARGET
{
	float3 color = g_tex.Sample(g_sampler, input.uv_pos.xy).rgb;
//	float3 color = g_tex.SampleLevel(g_sampler, input.uv_pos.xy, 0).rgb;
	return float4(color, 1.0f);
}



