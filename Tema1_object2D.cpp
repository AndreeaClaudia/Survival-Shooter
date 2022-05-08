#include "Tema1_object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* Tema1_object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* Tema1_object2D::CreateCircle(const std::string& name, float nr, glm::vec3 color)
{
    Mesh* circle = new Mesh(name);
    std::vector<VertexFormat> vertices_cerc{
        VertexFormat(glm::vec3(0,0,0), color, glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(1,0,0), color, glm::vec3(0.2, 0.8, 0.6)),
    };
    for (int i = 1; i < nr; i++) {
        float grade = RADIANS((float) 360 / nr);
        vertices_cerc.push_back(VertexFormat(glm::vec3(cos(grade * i), sin(grade * i), 0), color, glm::vec3(0.2, 0.8, 0.6)));
    }

    std::vector<unsigned int> indices_cerc;
    for (int i = 0; i < nr; i++)
        indices_cerc.push_back(i);
    indices_cerc.push_back(1);

    circle->SetDrawMode(GL_TRIANGLE_FAN);
    circle->InitFromData(vertices_cerc, indices_cerc);
    return circle;
}

Mesh* Tema1_object2D::CreateRectangle(const std::string& name, glm::vec3 corner, float n, float m, glm::vec3 color, bool fill)
{
    Mesh* rectangle = new Mesh(name);
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(n, 0, 0), color),
        VertexFormat(corner + glm::vec3(n, m, 0), color),
        VertexFormat(corner + glm::vec3(0, m, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;

}