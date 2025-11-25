#pragma pack_matrix(row_major)

struct VSOutput {
    float4 pos       : SV_POSITION;
    float3 normal    : NORMAL;
    float3 worldPos  : TEXCOORD0;
    float4 col       : COLOR;
};

cbuffer GridParams : register(b2) {
    float cellSize;
    float lineWidth; 
    float majorStep; 
    float majorWidth; 
    float baseAlpha; 
    float3 gridColor;
    float3 majorColor;
    float3 axisXColor;
    float3 axisZColor;
    float axisWidth; 
    float fadeDistance;
    float pad[2];
};

cbuffer CameraParams : register(b3) {
    float3 cameraPos;
    float pad1;
};

float aastep(float threshold, float x, float fw) {
   return smoothstep(threshold - fw, threshold + fw, x);
}

float4 PSMain(VSOutput input) : SV_TARGET {
    float2 uv = input.worldPos.xz / cellSize;
    float2 g = abs(frac(uv) - 0.5);
    float lineDist = min(g.x, g.y);
    float fw = fwidth(lineDist);
    float lineMask = 1.0 - aastep(lineWidth * 0.5, lineDist, fw);

    float2 uvMajor = uv / majorStep;
    float2 gm = abs(frac(uvMajor) - 0.5);
    float dm = min(gm.x, gm.y);
    float fwMajor = fwidth(dm);
    float majorMask = 1.0 - aastep(majorWidth * 0.5, dm, fwMajor);

    float ax = 1.0 - aastep(axisWidth * 0.5, abs(input.worldPos.z) / cellSize, fwidth(input.worldPos.z / cellSize));
    float az = 1.0 - aastep(axisWidth * 0.5, abs(input.worldPos.x) / cellSize, fwidth(input.worldPos.x / cellSize));

    float3 col = gridColor;
    col = lerp(col, majorColor, majorMask);
    col = lerp(col, axisXColor, ax);
    col = lerp(col, axisZColor, az);

    float mask = max(max(lineMask, majorMask), max(ax, az));
    float dist = distance(input.worldPos, cameraPos);
    float fade = saturate(1.0 - dist / 50.0f);
    float alpha = baseAlpha * saturate(mask) * fade;

    return float4(col, alpha);
}