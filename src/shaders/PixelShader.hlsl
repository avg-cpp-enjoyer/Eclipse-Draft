#pragma pack_matrix(row_major)

struct VSOutput {
    float4 pos       : SV_POSITION;
    float3 normal    : NORMAL; 
    float3 worldPos  : TEXCOORD0; 
    float4 col       : COLOR;
};

cbuffer LightBuffer : register(b1) {
    float3 lightPos;
    float pad1;
    float3 lightColor;
    float pad2;
};

float4 PSMain(VSOutput input) : SV_TARGET {
    float3 light    = normalize(lightPos - input.worldPos);
    float  diff      = saturate(dot(input.normal, light));
    float3 ambient  = 0.6f * input.col.rgb;
    float3 diffuse  = diff * lightColor * input.col.rgb;
    
    return float4(ambient + diffuse, 1.0f);
}