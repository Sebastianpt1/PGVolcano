#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "shaderClass.h"
#include "Particle.h"

class ParticleSystem {
public:
    std::vector<Particle> particles;
    unsigned int maxParticles;
    unsigned int VAO, VBO;
    Shader* shader;

    ParticleSystem(unsigned int maxParticles, Shader* shader);
    void Update(float deltaTime, glm::vec3 origin);
    void Draw();

private:
    unsigned int lastUsed = 0;
    unsigned int findUnusedParticle();
    void respawnParticle(Particle& particle, glm::vec3 origin);
};

#endif
