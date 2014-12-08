//----------------------------------------------------------
//Shaders processing deferred textures

//Resources
Texture2D    positionTex  : register(t0);
Texture2D    normalTex    : register(t1);
Texture2D    colorTex     : register(t2);

SamplerState texSampler   : register(s0);

//Structs
struct GS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

//VS
float4 vs_quad(float4 input : POSITION) : SV_POSITION
{
    return input;
}

//GS
[maxvertexcount(4)]
void gs_quad(point float4 pt[1] : SV_POSITION, inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT arr[4];
    arr[0].position = float4(-1, -1, 0, 1);
    arr[1].position = float4(-1, 1, 0, 1);
    arr[2].position = float4(1, 1, 0, 1);
    arr[3].position = float4(1, -1, 0, 1);

    arr[0].texcoord = float2(0, 1);
    arr[1].texcoord = float2(0, 0);
    arr[2].texcoord = float2(1, 0);
    arr[3].texcoord = float2(1, 1);

    outputStream.Append(arr[0]);
    outputStream.Append(arr[1]);
    outputStream.Append(arr[3]);
    outputStream.Append(arr[2]);
}

//PS
float4 ps_quad(GS_OUT input) : SV_TARGET
{
    float4 pos = positionTex.Sample(texSampler, input.texcoord);
    float4 normal = normalTex.Sample(texSampler, input.texcoord);
    float4 color = colorTex.Sample(texSampler, input.texcoord);

    return color;
}