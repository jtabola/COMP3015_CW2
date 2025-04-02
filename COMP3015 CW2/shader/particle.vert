#version 460

layout (location = 0) in vec3 aPosition;  // Particle position
layout (location = 1) in vec3 aVelocity;  // Particle velocity 
layout (location = 2) in vec4 aColor;     // Particle color
layout (location = 3) in vec2 aTexCoord;  // Texture coordinate

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(aPosition, 1.0);
    gl_PointSize = 40.0;
    vColor = aColor;
    vTexCoord = aTexCoord;
}