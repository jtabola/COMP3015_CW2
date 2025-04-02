#version 460

in vec4 vColor;
out vec4 FragColor;

uniform sampler2D particleTex;

void main() {
    vec4 texColor = texture(particleTex, gl_PointCoord); // Use gl_PointCoord instead

    FragColor = texColor * vColor;
}