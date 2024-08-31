#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "Chain.h"

using namespace std;

class Particle
{
public:
    Vector2 cur, last, acc;
    float radius, mass, bounce;

    Particle(Vector2 pos, float m, float r)
    {
        cur = pos;
        last = pos;
        acc = {0, 0};
        mass = m;
        radius = r;
        bounce = .9f;
    }

    void update(float dt)
    {
        // Verlet Integration
        const Vector2 vel = Vector2Subtract(cur, last);
        last = cur;
        cur = Vector2Add(Vector2Add(cur, vel), Vector2Scale(acc, dt*dt));
        acc = {};
    }

    void accelerate(Vector2 a)
    {
        //a = {a.x / mass, a.y / mass};
        acc = Vector2Add(acc, a);
        // acc = {acc.x / mass, acc.y / mass};  incoorporate OBJECT MASS
    }

    void setVelocity(Vector2 v, float dt)
    {
        last = Vector2Subtract(cur, Vector2Scale(v, dt));
    }

    void addVelocity(Vector2 v, float dt)
    {
        last = Vector2Subtract(last, Vector2Scale(v, dt));
    }

    Vector2 getVelocity(float dt)
    {
        return Vector2Scale(Vector2Subtract(cur, last), 1 / dt);
    }

    void draw() 
    {
        DrawCircleV(cur, radius, RAYWHITE);
        DrawCircleLinesV(cur, radius + 0.1f, BLACK);
    }

    void keepInView(int width, int height)
    {
        const Vector2 vel = Vector2Subtract(cur, last);

        if (cur.x - radius <= 0)
        {
            cur.x = radius;
            last.x = cur.x + vel.x * bounce;
        }
        if (cur.y - radius <= 0.f )
        {
            cur.y = radius;
            last.y = cur.y + vel.y* bounce;
        }
        if ( cur.x + radius >=  width)
        {
            cur.x = width - radius;
            last.x = cur.x + vel.x* bounce;
        }

        if (cur.y + radius >= height)
        {
            cur.y = height - radius;
            last.y = cur.y + vel.y* bounce;
        }
    }
    
    void undoMovement()
    {
        cur = last;
    }

    void quicksand()  // if collide with ground -> checkbounds
    {
        last = cur;
    }
};

class Physix
{
    public:
    std::vector<Particle *> Particles;
    Vector2 g = {0.0f, 1000.f};

    void update(float dt) 
    {
        applyGravity();
        //applyConstraint();
        updatePositions(dt);
    }

    void draw()
    {
        for (Particle* p : Particles) {
            p->draw();
        }
    }

    void updatePositions(float dt)
    {
        for (Particle* p : Particles) {
            p->update(dt);
            p->keepInView(GetScreenWidth(), GetScreenHeight());
        }
    }

    void applyGravity()
    {
        for (Particle* p : Particles) {
            p->accelerate(g);
        }
    }

    void applyConstraint()
    {
        const Vector2 position = {800.0f, 450.0f};
        const float radius = 400.0f;
        for (Particle* p : Particles) 
        {
            const Vector2 toVec = Vector2Subtract(p->cur, position);
            const float dist = Vector2Length(toVec);
            
            if (dist > radius - 50)
            {
                const Vector2 n = Vector2Scale(toVec, 1.f / dist);
                p->cur = Vector2Add(position, Vector2Scale(n, (dist - 50.f)));
            }
        }
    }

};

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    const Vector2 v0 = Vector2{0, 0};
    const Vector2 v1 = Vector2{1, 1};

    InitWindow(screenWidth, screenHeight, "verlz");

    Vector2 origin = {screenWidth/2, screenHeight/2};
    Particle p1 = Particle{origin, 5.f, 20.f};
    Physix motor;

    motor.Particles.push_back(&p1);
    

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Event Handling
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            Vector2 curr = motor.Particles.back()->cur;
            Vector2 v1 = Vector2Normalize(Vector2Subtract(mouse, curr));
            v1 = Vector2Scale(v1, 5000.f);
            motor.Particles.back()->accelerate(v1);
        }
        if (IsKeyPressed(KEY_SPACE))
        {
            Vector2 v = Vector2Normalize(Vector2Subtract(p1.cur, {0, 1}));
            v = Vector2Scale({0, -1}, 10000.f);
            p1.accelerate(v);
        }

        // Update Logic
        motor.update(GetFrameTime());

        // Draw
        BeginDrawing();
        ClearBackground(DARKGRAY);

        motor.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
