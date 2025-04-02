#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/objmesh.h"

#include "helper/globals.h"

#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;
    glm::vec2 texCoord;
};

class SceneBasic_Uniform : public Scene
{
private:

    std::unique_ptr<ObjMesh> objectMesh;

    GLuint normalMap;
    GLuint diffTex;
    GLuint particleTex;

    GLSLProgram prog, particleShader, finalShader;

    GLuint particleVBO, particleVAO;
    GLuint hdrFBO, colorBuffer, rboDepth;

    std::vector<Particle> particles;
    const int MAX_PARTICLES = 1000;

    void setMatrices();
    void compile();

public:
    SceneBasic_Uniform();

    void initParticles();
    void updateParticles(float deltaTime);
    void renderParticles();
    void respawnParticle(Particle& p);
    void initScene();
    void update( float t );
    void render();
    void resize(int, int);

    void setupBloomBuffers();
    void renderQuad();
};

#endif // SCENEBASIC_UNIFORM_H
