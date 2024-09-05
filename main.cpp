#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "particle.h"
#include "stick.h"
#include "physix.h"


int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "verlz");

    // INIT VARIABLES

    float dt;
    float default_mass = 1.f;
    // Vector2 origin = {screenWidth / 2, screenHeight / 2};
    Particle p1(600, 400, 10.f, default_mass);
    Particle p2(400, 400, 10.f, default_mass);
    Stick stick1({p1.get_x(), p1.get_y()}, {p2.get_x(), p2.get_y()}, Vector2Distance(p1.get_position(), p2.get_position()));
    Physix motor(0.f, 1000.f);

    motor.particles.push_back(p1);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // EVENT HANDLING -----------------------------
        dt = GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // set prev to opposite of mouse coord
            const float vel_x = (p1.get_x() - GetMouseX()) * -1;
            const float vel_y = (p1.get_y() - GetMouseY()) * -1;

            p1.set_velocity(vel_x, vel_y, dt); // normalize maybe;
            p1.add_velocity(5000.f, dt);
            // p1.accelerate();
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            motor.spawn_particle(GetMouseX(), GetMouseY(), 5.f,  default_mass);
        }

        if (IsKeyDown(KEY_SPACE))
        {
            int random_x = GetRandomValue(0, screenHeight);
            int random_y = GetRandomValue(0, screenWidth);
            motor.spawn_particle(random_x, random_y, 5.f, default_mass);
            motor.spawn_particle(random_x + 10, random_y + 5, 5.f, default_mass);
            
        }

        // --------------------
        //        UPDATE
        // --------------------

        motor.update(dt);

        // --------------------
        //        DRAW
        // --------------------

        BeginDrawing();
        ClearBackground(DARKGRAY);

        motor.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
