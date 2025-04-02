#version 460

in vec2 TexCoord;
in float FogCoord;
in mat3 TBN;
in vec3 FragPos;

out vec4 FragColor;

layout (binding = 0) uniform sampler2D baseTex;
layout (binding = 1) uniform sampler2D normalMap;

uniform struct LightInfo {
    vec4 Position;
    vec3 Ld;
    vec3 La;
    vec3 Ls;
} Lights[2];

uniform struct MaterialInfo {
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
} Material;

uniform vec3 FogColor;
uniform float FogStart;
uniform float FogEnd;

void main()
{
    vec3 normalTex = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
    vec3 normal = normalize(TBN * normalTex);

    vec3 diffuseColor = texture(baseTex, TexCoord).rgb;
    vec3 lightColor = vec3(0.0);

    for (int i = 0; i < 2; i++) {
        vec3 lightDir = normalize(Lights[i].Position.xyz - FragPos);
        float diff = max(dot(normal, lightDir), 0.0);

        if (diff > 0.75) diff = 1.0;
        else if (diff > 0.5) diff = 0.6;
        else if (diff > 0.25) diff = 0.3;
        else diff = 0.1;

        vec3 diffuse = Lights[i].Ld * Material.Kd * diff;
        vec3 ambient = Lights[i].La * Material.Ka;
        lightColor += diffuse + ambient;
    }

    vec3 finalColor = lightColor * diffuseColor;

    float fogFactor = clamp((FogEnd - FogCoord) / (FogEnd - FogStart), 0.0, 1.0);
    vec3 foggedColor = mix(FogColor, finalColor, fogFactor);

    FragColor = vec4(foggedColor, 1.0);
}
