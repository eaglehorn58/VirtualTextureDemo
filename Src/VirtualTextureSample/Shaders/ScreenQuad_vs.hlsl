//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

//	Constant updated by terrain
cbuffer VS_CONST : register(b0)
{
	float4	g_rect;			//	x, y, z, w: left, top, right, bottom
	float2	g_vScnSize;		//	x, y: screen width / height
	float2	padding;		//	padding data
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
	float4 uv_pos : TEXCOORD0;	//	xy: uv; zw: position in clipping space
};

VS_OUTPUT vs_fullquad(uint vi : SV_VERTEXID)
{
    VS_OUTPUT output;
	output.uv_pos.xy = float2(vi & 1, vi >> 1);
	output.uv_pos.zw = float2((output.uv_pos.x - 0.5f) * 2, -(output.uv_pos.y - 0.5f) * 2);
	output.position = float4(output.uv_pos.zw, 0.0f, 1.0f);
    return output;
}

VS_OUTPUT vs_quad(uint vi : SV_VERTEXID)
{
	VS_OUTPUT output;

	uint border_x = vi & 1;
	uint border_y = vi >> 1;
	output.uv_pos.xy = float2(border_x, border_y);

	float x = (border_x == 0) ? g_rect.x : g_rect.z;
	output.uv_pos.z = ((x / g_vScnSize.x) - 0.5f) * 2.0f;
	float y = (border_y == 0) ? g_rect.y : g_rect.w;
	output.uv_pos.w = -((y / g_vScnSize.y) - 0.5f) * 2.0f;

	output.position = float4(output.uv_pos.zw, 0.0f, 1.0f);

	return output;
}

