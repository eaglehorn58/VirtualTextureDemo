
cbuffer VS_CONST
{
    matrix g_matWVP;
};

struct VS_INPUT
{
    float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;

    input.position.w = 1.0f;
    output.position = mul(input.position, g_matWVP);
	output.uv = input.uv;
   
    return output;
}

