//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

cbuffer PS_CONST
{
    float4 g_color;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    return g_color;
}
