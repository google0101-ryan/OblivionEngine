#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec3 vTangent;

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstants
{
    vec3 eyePos;
} pushConstants;

layout(binding = 2) uniform sampler2D texAlbedo;
layout(binding = 3) uniform sampler2D texNormal;
layout(binding = 4) uniform sampler2D texMetallicRoughness;
layout(binding = 5) uniform sampler2D texAmbientOcclusion;
layout(binding = 6) uniform sampler2D irradianceMap;

struct Cluster
{
    uint offset; // Offset into list
    uint lightNum; // Number of lights affecting cluster
};

struct Item
{
    uint lightIndex;
};

struct Light
{
    vec3 pos;
    vec3 color;
    vec3 radiance;
};

layout(std140,set = 1, binding = 0) readonly buffer ClusterBuffer
{
    Cluster list[3072];
} clusterList;

layout(std140,set = 1, binding = 1) readonly buffer ItemBuffer
{
    Item list[256];
} itemList;

layout(std140,set = 1, binding = 2) readonly buffer LightBuffer
{
    Light list[256];
} lightList;

const float PI = 3.14159265359;

vec3 CalculateNormal(vec3 sampledNormal)
{
    if (length(vTangent) == 0.0f)
        return vNormal;

    vec3 binormal = normalize(cross(vTangent, vNormal));
    mat3 localAxis = mat3(binormal, vTangent, vNormal);

    sampledNormal = (2.0f * sampledNormal) - 1.0f;

    vec3 N = normalize(vNormal * sampledNormal);

    N.xy *= -1.0; // x and y were flipped.
    return N;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 CalculatePBR(Light light, vec3 F0, vec3 N, vec3 V, vec3 baseColor, float metal, float rough)
{
    vec3 L = normalize(light.pos - vPosition);
    vec3 H = normalize(V + L);

    float distance = length(light.pos - vPosition);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * attenuation;

    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = DistributionGGX(N, H, rough);
    float G = GeometrySmith(N, V, L, rough);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metal;

    float NdotL = max(dot(N, L), 0.0);

    return (kD * baseColor / PI + specular) * radiance * NdotL;
}

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 FilmicTonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
    vec3 baseColor = texture(texAlbedo, vTexCoord).rgb;
    vec3 sampledNormal = texture(texNormal, vTexCoord).rgb;
    vec3 metallicRoughness = texture(texMetallicRoughness, vTexCoord).rgb;
    float ao = texture(texAmbientOcclusion, vTexCoord).r;

    float metallicness = metallicRoughness.b;
    float roughness = metallicRoughness.g;

    float alpha = texture(texAlbedo, vTexCoord).a;
    if (alpha <= 0.1f)
        discard;
    
    if (texture(texNormal, vTexCoord).a <= 0.1f)
        discard;
    
    vec3 N = CalculateNormal(sampledNormal);
    vec3 V = normalize(pushConstants.eyePos - vPosition);

    uint offset = clusterList.list[0].offset;
    uint lightNum = clusterList.list[0].lightNum;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallicness);

    vec3 Lo = vec3(0.0);
    for (uint i = 0; i < lightNum; i++)
    {
        Lo += CalculatePBR(lightList.list[offset+i], F0, N, V, baseColor, metallicness, roughness);
    }

    vec3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(irradianceMap, N.xy).rgb;
    vec3 diffuse = irradiance * baseColor;
    vec3 ambient = (kD * diffuse) * ao;

    vec3 color = ambient + Lo;

    color = FilmicTonemap(color);

    fragColor = vec4(color, alpha);
}