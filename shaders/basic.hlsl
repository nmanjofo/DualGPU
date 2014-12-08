
//Resources
cbuffer Matrices           : register(b0)
{
    matrix mvp;
};

Texture2D    objTex        : register(t0);
SamplerState texSampler    : register(s0);

//Structs
struct PS_OUT
{
    float4 position : SV_TARGET0;
    float4 normal   : SV_TARGET1;
    float4 color    : SV_TARGET2;
};

struct PS_INPUT
{
    float4 pos      : SV_POSITION;
    float2 texcoord : TEXCOORD;
};


struct VS_INPUT
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
};

//VS
PS_INPUT vertex_shader(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(input.position, mvp);
    output.texcoord = input.texcoord;

    return output;
}

//PS
PS_OUT pixel_shader(PS_INPUT input)
{
    PS_OUT p;
    p.position = input.pos;
    p.normal = float4(1,1,1,1);
    p.color = objTex.Sample(texSampler, input.texcoord);

    return p;
}

