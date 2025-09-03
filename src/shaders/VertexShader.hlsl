#pragma pack_matrix(row_major)

struct VSInput {
    float3 pos    : POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD;
    float4 col    : COLOR; 
};

struct VSOutput {
    float4 pos      : SV_POSITION;
    float3 normal   : NORMAL; 
    float3 worldPos : TEXCOORD0; 
    float4 col      : COLOR; 
};

cbuffer TransformBuffer : register(b0) {
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

VSOutput VSMain(VSInput input) {
    VSOutput output;

    float4 worldPos  = mul(float4(input.pos, 1.0f), world);
    output.pos       = mul(mul(worldPos, view), projection);
    output.normal    = normalize(mul(input.normal, (float3x3)world));
    output.worldPos  = mul(float4(input.pos, 1.0f), world).xyz;
    output.col       = input.col;
    
    return output;
}