#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "particle.h"
#include "stick.h"
#include "motor.h"

class EnvItem
{
public:
    Vector2 pos;
    float width;
    float height;
    bool collision;
};

Particle spawn_particle_rand(float radius, float mass)
{
    float random_x = GetRandomValue(0, GetScreenHeight());
    float random_y = GetRandomValue(0, GetScreenWidth());
    return Particle{Vector2{random_x, random_y}, radius, mass};
}

int main()
{
    Color darkGreen = Color{20, 160, 133, 255};

    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "verlz");

    // INIT VARIABLES

    float dt;
    float default_mass = 1.0f;
    // Vector2 origin = {screenWidth / 2, screenHeight / 2};
    //Particle player({screenWidth / 2, screenHeight / 2}, 30.f, 3.f);
    Particle p1(600, 400, 10.f, default_mass);
    Particle p2(400, 400, 10.f, default_mass);
    Motor motor(0.f, 1000.f);

    motor.particles.push_back(p1);
    motor.particles.push_back(p2);
    Stick stick1(&motor.particles[0], &motor.particles[1], 50.0f); // Vector2Distance(p1.get_position(), p2.get_position()));
    //motor.particles.push_back(player);

    motor.sticks.push_back(stick1);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // EVENT HANDLING -----------------------------
        dt = GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // set prev to opposite of mouse coord
            //const float vel_x = (player.get_x() - GetMouseX()) * -1;
            //const float vel_y = (player.get_y() - GetMouseY()) * -1;

            //player.set_velocity(GetMouseX(), GetMouseY(), dt); // normalize maybe;
            // player.add_velocity(5000.f, dt);
            // player.accelerate();
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            motor.spawn_particle(GetMouseX(), GetMouseY(), 5.f, default_mass);
        }

        if (IsKeyDown(KEY_SPACE))
        {
        }

        // --------------------
        //        UPDATE
        // --------------------
        //stick1.update();

        //player.update(dt);
        motor.update(dt);
        // --------------------
        //        DRAW
        // --------------------

        BeginDrawing();
        ClearBackground(darkGreen);
        //stick1.draw();

        motor.draw();
        //player.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
