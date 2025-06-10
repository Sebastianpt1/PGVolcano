#pragma once
#include <glm/glm.hpp>

struct Triangle {
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};

bool intersectRayTriangle(
    const glm::vec3& orig, const glm::vec3& dir,
    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
    float& t
);

float getAlturaDesdeTerreno(const glm::vec3& posXZ, const std::vector<Triangle>& triList);

