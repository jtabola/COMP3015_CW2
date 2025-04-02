#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec3 VertexTangent;

out vec2 TexCoord;
out float FogCoord;
out mat3 TBN;
out vec3 FragPos;

const int NUM_LIGHTS = 2;

uniform float u_time;

uniform struct LightInfo {
    vec4 Position;
    vec3 Ld;
    vec3 La;
    vec3 Ls;
} Lights[NUM_LIGHTS];

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main() {
    TexCoord = VertexTexCoord;

    vec3 N = normalize(NormalMatrix * VertexNormal);
    vec3 T = normalize(NormalMatrix * VertexTangent);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    vec4 eyePos = ModelViewMatrix * vec4(VertexPosition, 1.0);
    FragPos = eyePos.xyz;

    FogCoord = abs(eyePos.z);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}