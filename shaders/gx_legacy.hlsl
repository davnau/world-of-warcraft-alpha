struct GxLight
{
    float4 position;
    float4 diffuse;
    float4 ambient;
    float4 attenuation;
};

cbuffer GxConstants : register(b0)
{
    row_major float4x4 gWorld;
    row_major float4x4 gView;
    row_major float4x4 gProjection;
    row_major float4x4 gTextureMatrix[4];
    float4 gMaterialDiffuse;
    float4 gMaterialEmissive;
    float4 gSceneAmbient;
    float4 gFogColor;
    float4 gFogParams;
    float4 gStageBlend;
    float4 gStageTexGen;
    float4 gStageTexShader;
    float4 gTextureEnabled;
    float4 gRenderFlags;
    GxLight gLights[8];
};

Texture2D gTexture0 : register(t0);
Texture2D gTexture1 : register(t1);
Texture2D gTexture2 : register(t2);
Texture2D gTexture3 : register(t3);
SamplerState gSampler0 : register(s0);
SamplerState gSampler1 : register(s1);
SamplerState gSampler2 : register(s2);
SamplerState gSampler3 : register(s3);

struct GxVertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR0;
    float2 tex0 : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
};

struct GxVertexOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float4 tex0 : TEXCOORD0;
    float4 tex1 : TEXCOORD1;
    float4 tex2 : TEXCOORD2;
    float4 tex3 : TEXCOORD3;
    float3 worldPosition : TEXCOORD4;
    float3 worldNormal : TEXCOORD5;
    float fogDistance : TEXCOORD6;
};

float4 GenerateTexCoord(uint stage, float2 supplied, float texGen,
    float3 objectPosition, float3 worldPosition, float3 viewPosition,
    float3 worldNormal)
{
    float4 coordinate = float4(supplied, 0.0, 1.0);
    if (texGen > 0.5 && texGen < 1.5)
        coordinate = float4(objectPosition, 1.0);
    else if (texGen >= 1.5 && texGen < 2.5)
        coordinate = float4(worldPosition, 1.0);
    else if (texGen >= 2.5 && texGen < 3.5)
        coordinate = float4(viewPosition, 1.0);
    else if (texGen >= 3.5 && texGen < 4.5)
    {
        float3 eye = normalize(viewPosition);
        coordinate = float4(reflect(eye, normalize(worldNormal)), 1.0);
    }
    else if (texGen >= 4.5 && texGen < 5.5)
        coordinate = float4(normalize(worldNormal), 1.0);
    else if (texGen >= 5.5)
    {
        float3 eye = normalize(viewPosition);
        float3 reflection = reflect(eye, normalize(worldNormal));
        float scale = 2.0 * sqrt(max(0.00001,
            reflection.x * reflection.x + reflection.y * reflection.y
            + (reflection.z + 1.0) * (reflection.z + 1.0)));
        coordinate = float4(reflection.xy / scale + 0.5, 0.0, 1.0);
    }
    return mul(coordinate, gTextureMatrix[stage]);
}

GxVertexOutput GxVertexMain(GxVertexInput input)
{
    GxVertexOutput output;
    float4 objectPosition = float4(input.position, 1.0);
    float4 worldPosition = mul(objectPosition, gWorld);
    float4 viewPosition = mul(worldPosition, gView);
    output.position = mul(viewPosition, gProjection);
    output.worldPosition = worldPosition.xyz;
    output.worldNormal = mul(float4(input.normal, 0.0), gWorld).xyz;
    if (gRenderFlags.w > 0.5)
        output.worldNormal = normalize(output.worldNormal);
    output.color = input.color;
    output.tex0 = GenerateTexCoord(0, input.tex0, gStageTexGen.x,
        input.position, worldPosition.xyz, viewPosition.xyz, output.worldNormal);
    output.tex1 = GenerateTexCoord(1, input.tex1, gStageTexGen.y,
        input.position, worldPosition.xyz, viewPosition.xyz, output.worldNormal);
    output.tex2 = GenerateTexCoord(2, input.tex0, gStageTexGen.z,
        input.position, worldPosition.xyz, viewPosition.xyz, output.worldNormal);
    output.tex3 = GenerateTexCoord(3, input.tex1, gStageTexGen.w,
        input.position, worldPosition.xyz, viewPosition.xyz, output.worldNormal);
    output.fogDistance = abs(viewPosition.z);
    return output;
}

float2 TextureCoordinate(float4 coordinate, float shaderMode)
{
    if (shaderMode > 1.5)
        return coordinate.xy / max(abs(coordinate.w), 0.000001);
    return coordinate.xy;
}

float4 ApplyTextureBlend(float4 color, float4 textureColor, float blendMode)
{
    if (blendMode < 0.5)
        return textureColor;
    if (blendMode < 1.5)
        return color * textureColor;
    if (blendMode < 2.5)
        return float4(lerp(color.rgb, textureColor.rgb, textureColor.a), color.a);
    if (blendMode < 3.5)
        return saturate(color + textureColor);
    return saturate(color * textureColor * 2.0);
}

float3 CalculateLighting(GxVertexOutput input, out float activeLights)
{
    float3 normal = normalize(input.worldNormal);
    float3 lighting = gSceneAmbient.rgb;
    activeLights = 0.0;
    [unroll]
    for (uint index = 0; index < 8; ++index)
    {
        if (gLights[index].attenuation.w < 0.5)
            continue;
        activeLights += 1.0;
        float3 lightVector;
        float attenuation = 1.0;
        if (gLights[index].position.w > 0.5)
        {
            lightVector = gLights[index].position.xyz - input.worldPosition;
            float distanceToLight = length(lightVector);
            lightVector /= max(distanceToLight, 0.000001);
            attenuation = 1.0 / max(0.000001,
                gLights[index].attenuation.x
                + gLights[index].attenuation.y * distanceToLight
                + gLights[index].attenuation.z * distanceToLight * distanceToLight);
        }
        else
            lightVector = normalize(gLights[index].position.xyz);
        lighting += gLights[index].ambient.rgb * attenuation;
        lighting += gLights[index].diffuse.rgb
            * max(dot(normal, lightVector), 0.0) * attenuation;
    }
    return lighting;
}

float4 GxPixelMain(GxVertexOutput input) : SV_TARGET
{
    float4 color = input.color * gMaterialDiffuse;
    if (gRenderFlags.x > 0.5)
    {
        float activeLights;
        float3 lighting = CalculateLighting(input, activeLights);
        if (activeLights > 0.5 || any(gSceneAmbient.rgb > 0.0))
            color.rgb = color.rgb * saturate(lighting) + gMaterialEmissive.rgb;
        else
            color.rgb += gMaterialEmissive.rgb;
    }

    if (gTextureEnabled.x > 0.5)
        color = ApplyTextureBlend(color,
            gTexture0.Sample(gSampler0, TextureCoordinate(input.tex0, gStageTexShader.x)),
            gStageBlend.x);
    if (gTextureEnabled.y > 0.5)
        color = ApplyTextureBlend(color,
            gTexture1.Sample(gSampler1, TextureCoordinate(input.tex1, gStageTexShader.y)),
            gStageBlend.y);
    if (gTextureEnabled.z > 0.5)
        color = ApplyTextureBlend(color,
            gTexture2.Sample(gSampler2, TextureCoordinate(input.tex2, gStageTexShader.z)),
            gStageBlend.z);
    if (gTextureEnabled.w > 0.5)
        color = ApplyTextureBlend(color,
            gTexture3.Sample(gSampler3, TextureCoordinate(input.tex3, gStageTexShader.w)),
            gStageBlend.w);

    if (gRenderFlags.y > 0.0)
        clip(color.a - gRenderFlags.y);

    if (gRenderFlags.z > 0.5)
    {
        float fogFactor;
        if (gFogParams.w < 0.5)
            fogFactor = saturate((gFogParams.y - input.fogDistance)
                / max(gFogParams.y - gFogParams.x, 0.000001));
        else if (gFogParams.w < 1.5)
            fogFactor = saturate(exp(-gFogParams.z * input.fogDistance));
        else
        {
            float densityDistance = gFogParams.z * input.fogDistance;
            fogFactor = saturate(exp(-densityDistance * densityDistance));
        }
        color.rgb = lerp(gFogColor.rgb, color.rgb, fogFactor);
    }
    return saturate(color);
}
