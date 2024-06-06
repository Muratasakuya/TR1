
//================================================
// Primitive VS Shader
//================================================

struct TransformationMatrix
{
    
    float4x4 WVP;
};

struct VertexShaderOutput
{
    
    float4 position : SV_POSITION;
};

struct VetexShaderInput
{
    
    float4 position : POSITION0;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexShaderOutput main(VetexShaderInput input)
{
    
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);

    return output;
}