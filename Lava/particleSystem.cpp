#include "particleSystem.h"
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>


ParticleSystem::ParticleSystem(unsigned int maxParticles, Shader* shader)
    : maxParticles(maxParticles), shader(shader) {
    particles.resize(maxParticles);

    float quad[] = {
        0.0f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

unsigned int ParticleSystem::findUnusedParticle() {
    for (unsigned int i = lastUsed; i < maxParticles; ++i) {
        if (!particles[i].Active || particles[i].Life <= 0.0f) {
            lastUsed = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < lastUsed; ++i) {
        if (!particles[i].Active || particles[i].Life <= 0.0f) {
            lastUsed = i;
            return i;
        }
    }
    return 0;
}

void ParticleSystem::respawnParticle(Particle& p, glm::vec3 origin) {
    p.Position = origin;
    p.Velocity = glm::vec3(glm::linearRand(-0.5f, 0.5f), glm::linearRand(2.5f, 5.0f), glm::linearRand(-0.5f, 0.5f));
    p.Life = 1.5f;
    p.Active = true;
}

void ParticleSystem::Update(float deltaTime, glm::vec3 origin) {
    for (int i = 0; i < 150; ++i) { // más particulas
        int unused = findUnusedParticle();
        respawnParticle(particles[unused], origin);
    }

    for (auto& p : particles) {
        if (p.Life > 0.0f) {
            p.Life -= deltaTime;
            p.Position += p.Velocity * deltaTime;
            p.Velocity += glm::vec3(0.0f, -9.8f, 0.0f) * deltaTime;
        }
        else {
            p.Active = false;
        }
    }
}

void ParticleSystem::Draw() {

    shader->Activate();
    glBindVertexArray(VAO);
    for (auto& p : particles) {
        if (!p.Active) continue;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), p.Position);
        model = glm::scale(model, glm::vec3(0.05f));
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_POINTS, 0, 1);
    }
    glBindVertexArray(0);
}
