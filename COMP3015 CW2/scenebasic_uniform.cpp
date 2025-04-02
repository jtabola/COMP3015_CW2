#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

extern bool stopRotation;
float delay = 0;
float initT = 0;
float endT = 0;

GLuint quadVAO = 0;

SceneBasic_Uniform::SceneBasic_Uniform(){}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);

    setupBloomBuffers();

    model = mat4(1.0f);

    view = glm::lookAt(vec3(1.25f, 0.25f, 2.25f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    projection = mat4(1.0f);

    prog.setUniform("Lights[0].Ld", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Lights[0].La", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Lights[0].Ls", vec3(1.0f, 1.0f, 1.0f));

    prog.setUniform("Lights[1].Ld", vec3(0.8f, 0.1f, 0.1f));
    prog.setUniform("Lights[1].La", vec3(0.8f, 0.1f, 0.1f));
    prog.setUniform("Lights[1].Ls", vec3(0.5f, 0.2f, 0.2f));

    prog.setUniform("FogColor", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("FogStart", 1.5f);
    prog.setUniform("FogEnd", 3.5f);

    diffTex = Texture::loadTexture("media/texture/skull_diff.jpg");
    normalMap = Texture::loadTexture("media/texture/skull_normal.jpg");
    particleTex = Texture::loadTexture("media/texture/fire.png");
    if (particleTex == 0) {
        std::cerr << "ERROR: Particle texture failed to load!" << std::endl;
    }
    else {
        std::cout << "Particle texture loaded successfully!" << std::endl;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, particleTex);

    objectMesh = ObjMesh::load("media/skull.OBJ", false, true);

    initParticles();
}

void SceneBasic_Uniform::compile()
{
	try {
        particleShader.compileShader("shader/particle.vert");
        particleShader.compileShader("shader/particle.frag");
        particleShader.link();
        finalShader.compileShader("shader/finalComposition.vert");
        finalShader.compileShader("shader/finalComposition.frag");
        finalShader.link();
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
        std::cerr << "Shader Compilation Error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    float deltaTime = 0.016f;
    updateParticles(deltaTime);
    
    float rotationSpeed = 0.5f;
    float angle = (t - delay) * rotationSpeed;

    if (!stopRotation) {
        if (initT != 0) {
            endT = t;
        }
        delay += endT - initT;
        angle = (t - delay) * rotationSpeed;
        model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        initT = 0;
        endT = 0;
    }
    else {
        if (initT == 0) {
            initT = t;
        }
    }

    prog.setUniform("u_time", t);

    setMatrices();
}

void SceneBasic_Uniform::render()
{
    prog.use();

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    prog.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Shininess", 100.0f);

    glm::vec4 light0Pos = view * glm::vec4(0.0f, 3.0f, 1.0f, 1.0f);
    glm::vec4 light1Pos = view * glm::vec4(0.0f, -2.0f, 1.0f, 1.0f);

    prog.setUniform("Lights[0].Position", light0Pos);
    prog.setUniform("Lights[1].Position", light1Pos);

    setMatrices();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap); 

    objectMesh->render();

    renderParticles();

    glDisable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    finalShader.use();
    finalShader.setUniform("scene", 0);
    finalShader.setUniform("bloomIntensity", 1.8f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    renderQuad();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices() {
    prog.use();
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::initParticles() {
    glEnable(GL_PROGRAM_POINT_SIZE);

    particles.resize(MAX_PARTICLES);

    for (auto& p : particles) {
        respawnParticle(p);
    }

    glGenVertexArrays(1, &particleVAO);
    glBindVertexArray(particleVAO);

    glGenBuffers(1, &particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)0);  
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, velocity));  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, color)); 
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, texCoord));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}


void SceneBasic_Uniform::respawnParticle(Particle& p) {
    float radius = 0.8f;  
    float height = -1.0f;

    float angle = ((float)rand() / RAND_MAX) * 2.0f * 3.14159f;
    float randomRadius = ((float)rand() / RAND_MAX) * radius;

    float x = randomRadius * cos(angle);
    float z = randomRadius * sin(angle);

    p.position = glm::vec3(x, height, z);

    float speed = ((float)rand() / RAND_MAX) * 0.5f + 1.0f;
    float spread = 0.3f;  

    p.velocity = glm::vec3(
        spread * (x / radius),
        speed, 
        spread * (z / radius)
    );

    p.life = ((float)rand() / RAND_MAX) * 2.5f + 0.5f;
    p.color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); 
}


void SceneBasic_Uniform::updateParticles(float deltaTime) {
    for (auto& p : particles) {
        if (p.life > 0.0f) {
            
            p.position += p.velocity * deltaTime;
            p.life -= deltaTime;

            float lifeRatio = p.life / 1.0f;
            p.color = glm::vec4(1.0f, 0.5f * lifeRatio, 0.0f, p.life);
        }
        else {
            respawnParticle(p);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * MAX_PARTICLES, &particles[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SceneBasic_Uniform::renderParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, particleTex);

    particleShader.use();
    particleShader.setUniform("particleTex", 2);
    particleShader.setUniform("MVP", projection * view * model);

    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

void SceneBasic_Uniform::setupBloomBuffers() {
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "HDR Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        GLuint quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

