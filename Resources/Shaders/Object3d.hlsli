struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};