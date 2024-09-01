#include <iostream>
#include <raylib.h>
#include <math.h>
#include <vector>
#include "particle.h"
#include "util.hpp"

class Stick
{
public:
    float pa_x, pa_y;
    float pc_x, pc_y;
    float length;

    Stick(float pa_x, float pa_y, float pc_x, float pc_y, float length)
    {
        this->pa_x = pa_x;
        this->pa_y = pa_y;
        this->pc_x = pc_x;
        this->pc_y = pc_y;
        this->length = length;
    }

    ~Stick()
    {
    }

    void draw()
    {
        DrawLineEx({pa_x, pa_y}, {pc_x, pc_y}, 5.f, GRAY);
    }
};

class Physix
{
public:
    std::vector<Particle> particles;
    std::vector<Stick> sticks;
    float force_x = 0;
    float force_y = 1000;

    void update(float dt)
    {
        apply_force(force_x, force_y);
        check_collision_particles(dt);
        // applyConstraint();
        update_positions(dt);
    }

    void draw()
    {
        for (Particle &p : particles)
        {
            p.draw();
        }
    }

    void update_positions(float dt)
    {
        for (Particle &p : particles)
        {
            p.update(dt);
            p.constrain(GetScreenWidth(), GetScreenHeight());
        }
    }

    void apply_force(float force_x, float force_y)
    {
        for (Particle &p : particles)
        {
            p.accelerate(force_x, force_y);
        }
    }

    void apply_random_force()
    {
        SetRandomSeed(GetTime());
        for (Particle &p : particles)
        {
            p.accelerate(GetRandomValue(0, GetScreenHeight()), GetRandomValue(0, GetScreenHeight()));
        }
    }

    void spawn_particle(float x, float y, float mass)
    {
        Particle particle(GetMouseX(), GetMouseY(), 4.f, mass);
        particles.push_back(particle);
    }

    void spawn_stick(float pa_x, float pa_y, float pc_x, float pc_y, float length)
    {
        Stick stick(pa_x, pa_y, pc_x, pc_y, length);
        sticks.push_back(stick);
    }

    void check_collision_particles(float dt)
    {
        const float response_coef = 1.f; // 0.75f;
        const int particle_count = particles.size();

        for (int i = 0; i < particle_count; i++)
        {
            Particle &p1 = particles.at(i);

            for (int k = i + 1; k < particle_count; k++)
            {
                Particle &p2 = particles.at(k);
                const float vel_x = p1.x - p2.x;
                const float vel_y = p1.y - p2.y;

                const float dist2 = vel_x * vel_x + vel_y * vel_y;
                const float min_dist = p1.radius + p2.radius;

                if (dist2 < min_dist * min_dist)
                {
                    const float dist = sqrt(dist2);
                    const float n_x = vel_x / dist;
                    const float n_y = vel_y / dist;
                    const float mass_ratio_1 = p1.radius / (p1.radius + p2.radius);
                    const float mass_ratio_2 = p2.radius / (p1.radius + p2.radius);
                    const float delta = 0.5f * response_coef * (dist - min_dist);
                    // Update positions
                    p1.x -= n_x * (mass_ratio_2 * delta);
                    p1.y -= n_y * (mass_ratio_2 * delta);
                    p2.x += n_x * (mass_ratio_1 * delta);
                    p2.y += n_y * (mass_ratio_1 * delta);
                }
            }
        }
    }

    // void applyConstraint()
    // {
    //     const Vector2 position = {800.0f, 450.0f};
    //     const float radius = 400.0f;
    //     for (Particle* p : particles)
    //     {
    //         const Vector2 toVec = Vector2Subtract(p->cur, position);
    //         const float dist = Vector2Length(toVec);

    //         if (dist > radius - 50)
    //         {
    //             const Vector2 n = Vector2Scale(toVec, 1.f / dist);
    //             p->cur = Vector2Add(position, Vector2Scale(n, (dist - 50.f)));
    //         }
    //     }
    // }
};

int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "verlz");

    // INIT VARIABLES
    float dt;
    float default_mass = 20.f;
    // Vector2 origin = {screenWidth / 2, screenHeight / 2};
    Particle p1(600, 400, 5.f, default_mass);
    Particle p2(400, 400, 5.f, default_mass);
    Stick stick1(p1.x, p1.y, p2.x, p2.y, utl::d_v(p1.x, p1.y, p2.x, p2.y));
    Physix motor;

    motor.particles.push_back(p1);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // EVENT HANDLING
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // set prev to opposite of mouse coord
            const float vel_x = (p1.x - GetMouseX()) * -1;
            const float vel_y = (p1.y - GetMouseY()) * -1;
            p1.set_velocity(vel_x, vel_y, dt); // normalize maybe;
            p1.add_velocity(5000.f, dt);
            // p1.accelerate();
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            motor.spawn_particle(GetMouseX(), GetMouseY(), default_mass);
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            // int random_x = GetRandomValue(0, screenHeight);
            // int random_y = GetRandomValue(0, screenWidth);
            motor.apply_random_force();
        }

        // UPDATE
        dt = GetFrameTime();
        motor.update(dt);

        // DRAW
        BeginDrawing();
        ClearBackground(DARKGRAY);

        motor.draw();
        stick1.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
