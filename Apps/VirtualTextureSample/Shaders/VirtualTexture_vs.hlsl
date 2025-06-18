
cbuffer VS_CONST
{
    matrix g_matWVP;
};

struct VS_INPUT
{
    float4 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;

    input.position.w = 1.0f;
    output.position = mul(input.position, g_matWVP);
   
    return output;
}

