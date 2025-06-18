//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

//	Constant updated by terrain
cbuffer VS_CONST : register(b0)
{
	float4	g_aOffsetX[8];		//	offset (in pages, not texel) of page in cache texture, 32 float
	float4	g_aOffsetY[8];
	float	g_fPageDivRTSize;	//	VPage size / render target size
	float3	padding;
};

static float g_aPageOffX[32] = g_aOffsetX;
static float g_aPageOffY[32] = g_aOffsetY;

struct VS_INPUT
{
	float4 position : POSITION;		//	xy: uv; z: page index
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
	float3 uv : TEXCOORD0;	//	xy: uv; z: page index
};

VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.uv = input.position.xyz;

	//float x = g_aPageOffX[input.position.z] * g_fPageDivRTSize + input.position.x * g_fPageDivRTSize;
	//float y = g_aPageOffY[input.position.z] * g_fPageDivRTSize + input.position.y * g_fPageDivRTSize;
	float2 offset = float2(g_aPageOffX[input.position.z], g_aPageOffY[input.position.z]);
	float2 pos = (offset + input.position.xy) * g_fPageDivRTSize;

	//output.position.x = (x - 0.5f) * 2.0f;
	//output.position.y = -(y - 0.5f) * 2.0f;
	output.position.xy = (pos - 0.5f) * 2.0f * float2(1.0f, -1.0f);
	output.position.zw = float2(0.0f, 1.0f);

	return output;
}

