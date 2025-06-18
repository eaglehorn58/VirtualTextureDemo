
cbuffer PS_CONST
{
    float4 g_color;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
};

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    return g_color;
}
