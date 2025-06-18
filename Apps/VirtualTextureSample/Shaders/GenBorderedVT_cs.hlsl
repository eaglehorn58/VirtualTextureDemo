
#define THREAD_CNT_ADD_BORDER	16

cbuffer CS_CONST
{
	int		g_iVTSize;				//	VT texture size (mip 0 level)
	int		g_iVPageSize;			//	VPage size
	int		g_iBorderSize;			//	border size
	float	padding;
};

Texture2D g_texVT : register(t0);		//	original VT
SamplerState g_samplerPt : register(s0);	//	point wrap sampler

RWTexture2D<float4> g_texBorderedVT : register(u0);

uint PackColor(float4 color)
{
	uint r = (uint)(color.r * 255.0f);
	uint g = (uint)(color.g * 255.0f) << 8;
	uint b = (uint)(color.b * 255.0f) << 16;
	uint a = (uint)(color.a * 255.0f) << 24;
	return r + g + b + a;
}

[numthreads(THREAD_CNT_ADD_BORDER, THREAD_CNT_ADD_BORDER, 1)]
void cs_main(uint3 gid : SV_GroupID, uint3 dtid : SV_DispatchThreadID, uint3 gtid : SV_GroupThreadID, uint gi : SV_GroupIndex)
{
	int2 page = (int2)(dtid.xy / g_iVPageSize);
	int2 vGridInPage = (int2)(dtid.xy % g_iVPageSize);
	float2 uv_off = (page * g_iVPageSize - g_iBorderSize) / (float)g_iVTSize;

	float fPageSizeWithBorder = g_iVPageSize + g_iBorderSize * 2;
	float2 vGridOff = (float2)vGridInPage * fPageSizeWithBorder / (float)g_iVPageSize;

	float2 uv0 = floor(vGridOff);
	float2 uv3 = uv0 + 1.0f;
//	float2 vFrac = (uv3 * g_iVPageSize / fPageSizeWithBorder) - vGridInPage;
	float2 vFrac = ((uv3 / fPageSizeWithBorder) - (vGridInPage / (float)g_iVPageSize)) * g_iVPageSize;

	//	add 0.2f offset so that we can get exactly texels that we want in point-sampling.
	uv0 = uv_off + (uv0 + 0.2f) / (float)g_iVTSize;
	uv3 = uv_off + (uv3 + 0.2f) / (float)g_iVTSize;
	float2 uv1 = float2(uv3.x, uv0.y);
	float2 uv2 = float2(uv0.x, uv3.y);

	//	bilinear interpolation
	float4 color0 = g_texVT.SampleLevel(g_samplerPt, uv0, 0);
	float4 color1 = g_texVT.SampleLevel(g_samplerPt, uv1, 0);
	float4 color2 = g_texVT.SampleLevel(g_samplerPt, uv2, 0);
	float4 color3 = g_texVT.SampleLevel(g_samplerPt, uv3, 0);

	color1 = lerp(color1, color0, vFrac.x);
	color3 = lerp(color3, color2, vFrac.x);
	color0 = lerp(color3, color1, vFrac.y);

	g_texBorderedVT[dtid.xy] = color0;
}

