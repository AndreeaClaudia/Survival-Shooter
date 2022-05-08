#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

        struct Enemy
        {
            Enemy() : x(1), y(1), angle(0), speed(1) {}
            Enemy(float x, float y, float angle, float speed)
                : x(x), y(y), angle(angle), speed(speed) {}
            float x;
            float y;
            float angle;
            float speed;
        };

        struct Bullet
        {
            Bullet() : x(1), y(1), angle(0), speed(1), initialX(1), initialY(1), range(3) {}
            Bullet(float x, float y, float angle, float speed, float initialX, float initialY, float range)
                : x(x), y(y), angle(angle), speed(speed), initialX(initialX), initialY(initialY), range(range) {}
            float x;
            float y;
            float angle;
            float speed;
            float initialX;
            float initialY;
            float range;
        };

     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawPlayer(glm::mat3 visMatrix);
        void DrawEnemy(glm::mat3 visMatrix, float enemyX, float enemyY, float rotate);
        void DrawScene(glm::mat3 visMatrix);
        void DrawBullet(glm::mat3 visMatrix, float bulletX, float bulletY, float rotate);
        bool WithinMap(float moveX, float moveY);
        bool CheckCircleCollision(float center1X, float center2X, float center1Y, float center2Y, float radius1, float radius2);
        float Clamp(float value, float min, float max);
        bool CheckCollision(float center1X, float center1Y, float radius, float aabbX, float aabbY, float sizeX, float sizeY);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        // Sets the logic space and view space
        // logicSpace: { x, y, width, height }
        // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);

        void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

     protected:
        std::vector<Enemy> enemies;
        std::vector<Bullet> bullets;
        float length;
        float cx, cy; 
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        float Xaxis, Yaxis;
        float enemyX, enemyY;
        float n = 100;
        float mapSize;
        float radiusPlayer;
        float lengthEnemy;
        float theta;
        bool fire = false;
        float timer, bulletTimer;
        float bulletSize;
        float diagonalOfEnemy;
        float radiusEnemy;
        float healthBar;
        float score;
        bool hit;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 modelMatrix, visMatrix, temporaryMatrix;
       
    };
}   // namespace m1
