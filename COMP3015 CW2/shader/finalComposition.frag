#version 460
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D scene;
uniform float bloomIntensity;

void main() {
    vec3 sceneColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = textureLod(scene, TexCoords, 4.0).rgb; // Sample lower mipmap level for blur
    FragColor = vec4(sceneColor + bloomColor * bloomIntensity, 1.0);
}