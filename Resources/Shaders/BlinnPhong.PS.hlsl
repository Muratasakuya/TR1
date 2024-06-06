#include "Object3d.hlsli"

//================================================
// BlinnPhong PS Shader
//================================================

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct Camera
{
    float3 position;
    float padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

/// ここをMaterialに持たせる
// 最優先
static const float4 SpecularColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
static const float SpecularPower = 64.0f;

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    PixelShaderOutput output;

    if (gMaterial.enableLighting != 0)
    {
        float3 normal = normalize(input.normal);
        float3 lightDir = normalize(-gDirectionalLight.direction);
        float3 viewDir = normalize(gCamera.position - input.worldPosition);

        // 拡散反射 (Diffuse)
        float NdotL = saturate(dot(normal, lightDir));

        // ハーフベクトルの計算
        float3 halfVector = normalize(lightDir + viewDir);
        float NdotH = saturate(dot(normal, halfVector));

        // スペキュラ反射の計算
        float spec = pow(NdotH, SpecularPower);

        // ライト計算
        float3 diffuse =
        gMaterial.color.rgb * textureColor.xyz * gDirectionalLight.color.rgb * gDirectionalLight.intensity * NdotL;
        
        float3 specular =
        SpecularColor.rgb * spec * gDirectionalLight.intensity * gDirectionalLight.color.rgb;

        output.color.rgb = diffuse + specular;
        
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    return output;
}