
#include "Collision.h"
#include <glm/glm.hpp>

//implementamos el intersectador de triangulos 
bool intersectRayTriangle(
    const glm::vec3& orig, const glm::vec3& dir,
    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
    float& t)
{
    const float EPSILON = 1e-6f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (fabs(a) < EPSILON)
        return false;

    float f = 1.0f / a;
    glm::vec3 s = orig - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = f * glm::dot(edge2, q);
    return (t > EPSILON);
}

//funcion para obtener la altura del terreno con raycasting
float getAlturaDesdeTerreno(const glm::vec3& posXZ, const std::vector<Triangle>& triList) {
    glm::vec3 orig = glm::vec3(posXZ.x, 10000.0f, posXZ.z);
    glm::vec3 dir = glm::vec3(0.0f, -1.0f, 0.0f);

    float nearestY = -INFINITY;
    for (const Triangle& tri : triList) {
        float t;
        if (intersectRayTriangle(orig, dir, tri.v0, tri.v1, tri.v2, t)) {
            glm::vec3 hitPoint = orig + t * dir;
            if (hitPoint.y > nearestY)
                nearestY = hitPoint.y;
        }
    }
    return nearestY;
}

