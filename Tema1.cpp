#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/Tema1_transform2D.h"
#include "lab_m1/Tema1/Tema1_object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 2;       // logic x
    logicSpace.y = 2;       // logic y
    logicSpace.width = 4;   // logic width
    logicSpace.height = 4;  // logic height

    glm::vec3 corner = glm::vec3(0.001, 0.001, 0);
    length = 0.99f;
    cx = length / 2;
    cy = length / 2;
    healthBar = 1;
    score = 0;
    cout << "SCORE: " << score << endl;

    mapSize = 8;
    radiusPlayer = 0.2;
    lengthEnemy = 0.3;
    //diagonala inamicului
    diagonalOfEnemy = lengthEnemy / sqrt(2);
    radiusEnemy = lengthEnemy + 0.09;
    bulletSize = 0.15;
    hit = false;

    timer = 0;
    bulletTimer = 0;

    Xaxis = logicSpace.width / 2 + logicSpace.x;
    Yaxis = logicSpace.height / 2 + logicSpace.y;  
    

    Mesh* square1 = Tema1_object2D::CreateSquare("square1", corner, length, glm::vec3(1, 0, 0));
    AddMeshToList(square1);

    Mesh* border = Tema1_object2D::CreateSquare("border", corner, mapSize, glm::vec3(1, 1, 0));
    AddMeshToList(border);

    Mesh* dreptunghi = Tema1_object2D::CreateRectangle("dreptunghi", corner, 1, 2, glm::vec3(1,0.5,0),true);
    AddMeshToList(dreptunghi);

    Mesh* circle = Tema1_object2D::CreateCircle("circle", n, glm::vec3(0.96, 0.87, 0.70));
    AddMeshToList(circle);
    //pt culoare e ok :)

    Mesh* circleBlack = Tema1_object2D::CreateCircle("circleBlack", n, glm::vec3(0));
    AddMeshToList(circleBlack);

    Mesh* squareRed = Tema1_object2D::CreateSquare("squareRed", corner, length, glm::vec3(1,0,0),true);
    AddMeshToList(squareRed);

    Mesh* squareBlack = Tema1_object2D::CreateSquare("squareBlack", corner, length, glm::vec3(0), true);
    AddMeshToList(squareBlack);

    Mesh* health = Tema1_object2D::CreateRectangle("health", corner, 1, 1, glm::vec3(1, 0, 0), true);
    AddMeshToList(health);

    Mesh* healthBorder = Tema1_object2D::CreateSquare("healthBorder", corner,1,glm::vec3(1,0,0));
    AddMeshToList(healthBorder);
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{

    glm::ivec2 resolution = window->GetResolution();
    // The viewport is now the right half of the window
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0.2f), true);

    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    DrawPlayer(visMatrix);

    //dupa 3 secunde
    if (timer >= 3)
    {
        //facem un inamic nou
        Enemy newEnemy = Enemy();
        do {
            newEnemy.x = rand() % 8;
            newEnemy.y = rand() % 8;
        } while (CheckCircleCollision(Xaxis, newEnemy.x+lengthEnemy/2,Yaxis,newEnemy.y+lengthEnemy/2, radiusPlayer+0.09,radiusEnemy));
        newEnemy.speed = rand() % 2 + 1;
        newEnemy.angle = atan2(Yaxis - newEnemy.y, Xaxis -newEnemy.x);
        enemies.push_back(newEnemy);
        timer = 0;
    }
    
    for (auto it = enemies.begin(); it < enemies.end();)
    {
        float centerX = it->x + lengthEnemy / 2;
        float centerY = it->y + lengthEnemy / 2;
        if ((!CheckCircleCollision(Xaxis,centerX,Yaxis,centerY,radiusPlayer+0.09,radiusEnemy)))
        {
            it->x += it->speed * deltaTimeSeconds * cos(it->angle);
            it->y += it->speed * deltaTimeSeconds * sin(it->angle);
            it->angle = atan2(Yaxis - it->y, Xaxis - it->x);
            DrawEnemy(visMatrix, it->x, it->y, it->angle);
            it++;
        }
        else
        {
            it = enemies.erase(it);
            healthBar -= 0.1;
            if (healthBar <= 0)
            {
                cout << "GAME OVER!" << endl;
                exit(0);
            }
        }
        
    }

    timer += deltaTimeSeconds;
    bulletTimer += deltaTimeSeconds;
    
    //it->x, it->y will always be the left bottom corner coordinates of the bullet 
    for (auto it = bullets.begin(); it < bullets.end(); )
    {
        //checking the range of the bullet and if it is within the map
        if(((it->x - it->initialX) <= it->range && (it->y - it->initialY) <= it->range) 
                && ((it->x - it->initialX) >= -it->range && (it->y - it->initialY) >= -it->range) 
                //checking if it is within the map
                && (it->x > 0 && it->y > 0 && (it->x + bulletSize) < mapSize && (it->y + bulletSize) < mapSize))
        {
            it->x += it->speed * deltaTimeSeconds * cos(it->angle);
            it->y += it->speed * deltaTimeSeconds * sin(it->angle);
            DrawBullet(visMatrix, it->x, it->y, it->angle);
            it++;
        }
        else
        {
            it = bullets.erase(it);
        }
    }

    for (auto itBullet = bullets.begin(); itBullet < bullets.end(); )
    {
        hit = false;
        for (auto itEnemy = enemies.begin(); itEnemy < enemies.end();)
        {
            float centerX = itEnemy->x + lengthEnemy / 2;
            float centerY = itEnemy->y + lengthEnemy / 2;
            if (CheckCollision(centerX, centerY, radiusEnemy, itBullet->x, itBullet->y, bulletSize, bulletSize))
            {
                hit = true;
                score += 100;
                cout << "Score: " << score << endl;
                itEnemy = enemies.erase(itEnemy);
                itBullet = bullets.erase(itBullet);
                break;
            }
            else
                itEnemy++;
        }
        if (!hit) {
            itBullet++;
        }
    }
    DrawScene(visMatrix);
}

bool Tema1::CheckCollision(float center1X, float center1Y, float radius, float aabbX, float aabbY, float sizeX, float sizeY)
{
    glm::vec2 center(center1X, center1Y);
    glm::vec2 aabb_half_extents(sizeX/ 2,sizeY/2);
    glm::vec2 aabbcenter(aabbX + aabb_half_extents.x, aabbY + aabb_half_extents.y);
    glm::vec2 difference(center1X - aabbcenter.x,center1Y - aabbcenter.y);
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabbcenter + clamped;
    difference = closest - center;
    return glm::length(difference) < radius;
}

bool Tema1::CheckCircleCollision(float center1X, float center2X, float center1Y, float center2Y, float radius1, float radius2)
{
    float dx = (center1X + radius1) - (center2X + radius2);
    float dy = (center1Y + radius1) - (center2Y + radius2);
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < radius1 + radius2) {
        //we have a collision
        return true;
    }
    return false;
}

//checks if the position after move is within the map
bool Tema1::WithinMap(float moveX, float moveY) {

    //circle "hit-box" size (a bit bigger than the radius, because of the hands of the player)
    float size = radiusPlayer + 0.09;
    
    return ((Xaxis + size + moveX) <= mapSize) && ((Yaxis + size + moveY) <= mapSize) && ((Xaxis - size + moveX) >= 0) && ((Yaxis - size + moveY) >= 0);
}

void Tema1::FrameEnd()
{
}

void Tema1::DrawPlayer(glm::mat3 visMatrix)
{
    //matrice corp
    modelMatrix = visMatrix * Tema1_transform2D::Translate(Xaxis, Yaxis);
    modelMatrix *= Tema1_transform2D::Scale(radiusPlayer, radiusPlayer);
    modelMatrix *= Tema1_transform2D::Rotate(angularStep);

    //ochi stanga
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.65, 0.6);
    temporaryMatrix *= Tema1_transform2D::Scale(0.45, 0.45);
    RenderMesh2D(meshes["circle"], shaders["VertexColor"], temporaryMatrix);

    //contur ochi stanga
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.65, 0.6);
    temporaryMatrix *= Tema1_transform2D::Scale(0.515, 0.515);

    RenderMesh2D(meshes["circleBlack"], shaders["VertexColor"], temporaryMatrix);

    //ochi dreapta
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.65, -0.6);
    temporaryMatrix *= Tema1_transform2D::Scale(0.45, 0.45);
    RenderMesh2D(meshes["circle"], shaders["VertexColor"], temporaryMatrix);

    //contur ochi dreapta
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.65, -0.6);
    temporaryMatrix *= Tema1_transform2D::Scale(0.515, 0.515);
    RenderMesh2D(meshes["circleBlack"], shaders["VertexColor"], temporaryMatrix);

    //corp
    RenderMesh2D(meshes["circle"], shaders["VertexColor"], modelMatrix);
    
    //contur corp
    modelMatrix = visMatrix * Tema1_transform2D::Translate(Xaxis, Yaxis);
    modelMatrix *= Tema1_transform2D::Scale(0.215, 0.215);
    RenderMesh2D(meshes["circleBlack"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawEnemy(glm::mat3 visMatrix, float enemyX, float enemyY, float rotate)
{
    //matrice corp
    modelMatrix = visMatrix * Tema1_transform2D::Translate(enemyX, enemyY);
    modelMatrix *= Tema1_transform2D::Scale(lengthEnemy, lengthEnemy);

    modelMatrix *= Tema1_transform2D::Rotate(rotate);

    //ochi stanga
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.65, 0.65);
    temporaryMatrix *= Tema1_transform2D::Scale(0.45, 0.45);
    RenderMesh2D(meshes["squareRed"], shaders["VertexColor"], temporaryMatrix);

    //contur ochi stanga
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.6, 0.6);
    temporaryMatrix *= Tema1_transform2D::Scale(0.515, 0.515);

    RenderMesh2D(meshes["squareBlack"], shaders["VertexColor"], temporaryMatrix);

    //ochi dreapta
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.65, -0.2);
    temporaryMatrix *= Tema1_transform2D::Scale(0.45, 0.45);
    RenderMesh2D(meshes["squareRed"], shaders["VertexColor"], temporaryMatrix);

    //contur ochi dreapta
    temporaryMatrix = modelMatrix;
    temporaryMatrix *= Tema1_transform2D::Translate(0.6, -0.2);
    temporaryMatrix *= Tema1_transform2D::Scale(0.515, 0.515);
    RenderMesh2D(meshes["squareBlack"], shaders["VertexColor"], temporaryMatrix);

    //corp
    RenderMesh2D(meshes["squareRed"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawScene(glm::mat3 visMatrix)
{
    //healthbar
    modelMatrix = visMatrix * Tema1_transform2D::Translate(logicSpace.x - 1.5, logicSpace.y + 3.6);
    temporaryMatrix = modelMatrix * Tema1_transform2D::Scale(1, 0.25);
    RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], temporaryMatrix);
    modelMatrix *= Tema1_transform2D::Scale(healthBar, 0.25);
    RenderMesh2D(meshes["health"], shaders["VertexColor"], modelMatrix);

    //harta
    RenderMesh2D(meshes["border"], shaders["VertexColor"], visMatrix);
    
    //obstacole
    modelMatrix = visMatrix * Tema1_transform2D::Translate(1,1);
    modelMatrix *= Tema1_transform2D::Scale(0.65, 1);
    RenderMesh2D(meshes["dreptunghi"],shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * Tema1_transform2D::Translate(4, 5);
    modelMatrix *= Tema1_transform2D::Scale(2, 0.3);
    RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * Tema1_transform2D::Translate(1, 1);
    modelMatrix *= Tema1_transform2D::Scale(2.5, 0.3);
    RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * Tema1_transform2D::Translate(5.5, 1);
    modelMatrix *= Tema1_transform2D::Scale(1, 0.4);
    RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * Tema1_transform2D::Translate(1.5, 6.2);
    modelMatrix *= Tema1_transform2D::Scale(healthBar, 0.4);
    RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawBullet(glm::mat3 visMatrix, float bulletX, float bulletY, float rotate)
{
    modelMatrix = visMatrix * Tema1_transform2D::Translate(bulletX, bulletY);
    modelMatrix *= Tema1_transform2D::Scale(bulletSize, bulletSize);
    RenderMesh2D(meshes["squareBlack"], shaders["VertexColor"], modelMatrix);

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_W) && (Tema1::WithinMap(0 ,deltaTime * 2)))
    {
        logicSpace.y += deltaTime * 2;
        Yaxis += deltaTime * 2;
    }
        if (window->KeyHold(GLFW_KEY_S) && (Tema1::WithinMap(0, -(deltaTime * 2))))
        {
            logicSpace.y -= deltaTime * 2;
            Yaxis -= deltaTime * 2;
        }
        if (window->KeyHold(GLFW_KEY_D) && (Tema1::WithinMap(deltaTime * 2, 0)))
        {
            logicSpace.x += deltaTime * 2;
            Xaxis += deltaTime * 2;
        }
        if (window->KeyHold(GLFW_KEY_A) && (Tema1::WithinMap(-(deltaTime * 2), 0)))
        {
            logicSpace.x -= deltaTime * 2;
            Xaxis -= deltaTime * 2;
        }
}

void Tema1::OnKeyPress(int key, int mods)
{
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    //getting the resolution of the window
    glm::ivec2 resolution = window->GetResolution();

    //substracting the resolution to get coordinates, not pixels
    int dx = mouseX - Xaxis - resolution.x /2;
    int dy = mouseY - Yaxis - resolution.y /2;

    //calculating the angle between the player and the mouse
    angularStep = - atan2(dy, dx);
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        if (bulletTimer >= 0.5) {
            Bullet newBullet = Bullet();
            newBullet.x = Xaxis;
            newBullet.y = Yaxis;
            newBullet.initialX = Xaxis;
            newBullet.initialY = Yaxis;
            newBullet.speed = 3;
            newBullet.angle = angularStep;
            bullets.push_back(newBullet);
            bulletTimer = 0;
        }
    }
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
