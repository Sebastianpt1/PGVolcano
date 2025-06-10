#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle {
    glm::vec3 Position;
    glm::vec3 Velocity;
    float Life;
    bool Active;

    Particle()
        : Position(0.0f), Velocity(0.0f), Life(0.0f), Active(false) {
    }
};

#endif
