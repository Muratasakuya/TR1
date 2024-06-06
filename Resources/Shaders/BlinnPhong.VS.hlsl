#include "Object3d.hlsli"

//================================================
// BlinnPhong VS Shader
//================================================

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.texcoord = input.texcoord;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
    
    return output;
}