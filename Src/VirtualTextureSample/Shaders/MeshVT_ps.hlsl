//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


cbuffer PS_CONST
{
	int		g_iVTSize;				//	VT texture size (mip 0 level)
	int		g_iVPageSize;			//	VPage size
	int		g_iVPageCntInRow;		//	VPage count in each row on VT
	int		g_iMaxMipmap;			//	maximum mipmap level
	int		g_iPageIDDownscale;		//	downscale for gathering page ids
	int		g_iPageIDBufWid;		//	page id buffer width
	int2	g_vPageIDDither;		//	dither for gathering page ids
	int		g_iCacheTexSize;		//	page cache texture (physical texture) size
	int		g_iBorderSize;			//	border size on each side
	float2	padding;
};

RWStructuredBuffer<uint> g_PageIDOutputBuf : register(u1);

Texture2D g_texVT : register(t0);		//	VT
Texture2D g_texVTIndir : register(t1);		//	VT indirect map
SamplerState g_samplerVT : register(s0);	//	VT sampler
SamplerState g_samplerVTIndir : register(s1);	//	VT indirect sampler

struct PS_INPUT
{
    float4 position : SV_POSITION;	//	screen position
	float2 uv : TEXCOORD0;	//	uv in VT
};

struct VTEX_ARGS
{
	uint2	posPage;	//	position on VT of mip level, in pages not in texels
	uint	mip;		//	desired mip level
};

struct INDIR_ENTRY
{
	uint2	offset;		//	offset in page cache texture, in pages not in texels
	uint	mip;		//	real mip level of this page
};

//	cacluate mipmap
//	uvVT: uv in virtual texture
float CalcMipmapLevel(float2 uvVT)
{
	float2 tex_uv = uvVT * g_iVTSize;

	float2 dx_vtc = ddx(tex_uv);
	float2 dy_vtc = ddy(tex_uv);
	float delta_max_sqr = max(dot(dx_vtc, dx_vtc), dot(dy_vtc, dy_vtc));
	return 0.5f * log2(delta_max_sqr);
}

uint PackPageID(VTEX_ARGS VTexArgs)
{
	uint packPageID = (VTexArgs.posPage.x << 20) | (VTexArgs.posPage.y << 8) | (VTexArgs.mip << 4) | 1;
	return packPageID;
}

//	vScreenPos: screen position
void OutputPageID(VTEX_ARGS VTexArgs, float2 vScreenPos)
{
	int2 vPos = (uint2)vScreenPos;
	int2 vPosOffset = ((uint2)vPos) % g_iPageIDDownscale;

	if (vPosOffset.x == g_vPageIDDither.x && vPosOffset.y == g_vPageIDDither.y)
	{
		uint uVPageID = PackPageID(VTexArgs);
		int2 vPosPageIDBuf = vPos / g_iPageIDDownscale;
		int index = vPosPageIDBuf.y * g_iPageIDBufWid + vPosPageIDBuf.x;
		g_PageIDOutputBuf[index] = uVPageID;
	}
}

//	unpack indirect entry info from indirect map color [0, 255]
INDIR_ENTRY UnpackIndirEntry(uint4 colIndir)
{
	uint id = (colIndir.a << 24) | (colIndir.b << 16) | (colIndir.g << 8) | colIndir.r;

	INDIR_ENTRY entry;
	entry.offset.x = (id & 0xfff00000) >> 20;
	entry.offset.y = (id & 0x000fff00) >> 8;
	entry.mip = (id & 0x000000f0) >> 4;

	return entry;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
	VTEX_ARGS VTexArgs;

	//	desired mip level
	float fDesiredMip = CalcMipmapLevel(input.uv);
	VTexArgs.mip = (uint)clamp(fDesiredMip, 0, g_iMaxMipmap);
	//	page in mip 0
	uint2 pageInMip0 = (uint2)(input.uv * g_iVPageCntInRow);
	VTexArgs.posPage = pageInMip0 >> VTexArgs.mip;

	//	output page id
	OutputPageID(VTexArgs, input.position.xy);

	//	fetch indirect map and unpack entry, add half texel so that we sample at the center of texel
//	float2 uvIndir = uvMip0;
	float2 uvIndir = (pageInMip0 + 0.5f) / (float)g_iVPageCntInRow;
	uint4 colIndir = (uint4)(g_texVTIndir.SampleLevel(g_samplerVTIndir, uvIndir, (int)VTexArgs.mip) * 255);
	INDIR_ENTRY IndirEntry = UnpackIndirEntry(colIndir);

	//	Add border factor to uv, this can erase seams between pages and
	//	enable bilinear filter work on sampling VT
	float2 uvMip0 = input.uv * g_iVTSize - pageInMip0 * g_iVPageSize;
	uvMip0 *= (g_iVPageSize - g_iBorderSize * 2) / (float)g_iVPageSize;
	uvMip0 = (uvMip0 + g_iBorderSize + pageInMip0 * g_iVPageSize) / (float)g_iVTSize;
	//	float2 uvMip0 = input.uv;	//	test code

	//	calculate pixel's final uv on page cache texture
	//	IndirEntry.mip is the real mip level the page can use.
	float fVTSize = (float)(g_iVTSize >> IndirEntry.mip);
	float2 offsetInPage = uvMip0 * fVTSize - (pageInMip0 >> IndirEntry.mip) * g_iVPageSize;
	float2 uvInCache = (offsetInPage + IndirEntry.offset * g_iVPageSize) / (float)g_iCacheTexSize;

	float4 color = g_texVT.Sample(g_samplerVT, uvInCache);

    return color;
//	return float4(temp.rgb, 1.0f);
}


