#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace Tema1_object2D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateCircle(const std::string& name, float nr, glm::vec3 color);
    Mesh* CreateRectangle(const std::string& name, glm::vec3 corner, float n, float m, glm::vec3 color, bool fill = false);
}
