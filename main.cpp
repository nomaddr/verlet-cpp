#include <iostream>
#include <raylib.h>
#include <math.h>
#include <vector>
#include "particle.h"
#include "util.hpp"
// #include "stick.hpp"

class Physix
{
public:
    std::vector<Particle> particles;
    std::vector<Stick> sticks;
    float force_x = 0;
    float force_y = 100 * 9.8f;

    void update(float dt)
    {
        apply_force(force_x, force_y);
        check_collision_particles_alt(dt);
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
            p.constrain_rectangle(GetScreenWidth(), GetScreenHeight());
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

    void spawn_stick(Vector2f a, Vector2f b, float length)
    {
        Stick stick(a, b, length);
        sticks.push_back(stick);
    }

    void check_collision_particles_alt(float dt)
    {
        const int particle_count = particles.size();
        const float response_coef = 0.75f; // 0.75f;

        // Iterate Particles
        for (int i = 0; i < particle_count; i++)
        {
            Particle &p1 = particles.at(i);
            // Iterate on object involved in new collision pairs
            for (int k = i + 1; k < particle_count; k++)
            {

                Particle &p2 = particles.at(k);
                    const float vel_x = p1.get_x() - p2.get_x();
                    const float vel_y = p1.get_y() - p2.get_y();

                    const float dist2 = vel_x * vel_x + vel_y * vel_y;
                    const float min_dist = p1.get_radius() + p2.get_radius();
                if (p1.check_collision_circle(p2))
                {
                   
                    // Check overlapping
                    //if (dist2 < min_dist * min_dist)
                    {
                        const float dist = sqrt(dist2);
                        const float n_x = vel_x / dist;
                        const float n_y = vel_y / dist;
                        const float mass_ratio_1 = p1.get_radius() / (p1.get_radius() + p2.get_radius());
                        const float mass_ratio_2 = p2.get_radius() / (p1.get_radius() + p2.get_radius());
                        const float delta = 0.5f * response_coef * (dist - min_dist);
                        // Update positions

                        Vector2f pos1 = p1.get_position();
                        Vector2f pos2 = p2.get_position();
                        pos1.x = pos1.x - n_x * (mass_ratio_2 * delta);
                        pos1.y = pos1.y - n_y * (mass_ratio_2 * delta);
                        pos2.x = pos2.x + n_x * (mass_ratio_1 * delta);
                        pos2.y = pos2.y + n_y * (mass_ratio_1 * delta);
                        p1.set_position(pos1.x, pos1.y);
                        p2.set_position(pos2.x, pos2.y);
                    }
                }
            }
        }
    }

        void check_collision_particles(float dt)
        {
            const float response_coef = 0.75f; // 0.75f;
            const int particle_count = particles.size();

            // Iterate Particles
            for (int i = 0; i < particle_count; i++)
            {
                Particle &p1 = particles.at(i);
                // Iterate on object involved in new collision pairs
                for (int k = i + 1; k < particle_count; k++)
                {
                    Particle &p2 = particles.at(k);
                    const float vel_x = p1.get_velocity_x(dt) - p2.get_velocity_x(dt);
                    const float vel_y = p1.get_velocity_y(dt) - p2.get_velocity_y(dt);

                    const float dist2 = vel_x * vel_x + vel_y * vel_y;
                    const float min_dist = p1.get_radius() + p2.get_radius();
                    // Check overlapping
                    if (dist2 < min_dist * min_dist)
                    {
                        const float dist = sqrt(dist2);
                        const float n_x = vel_x / dist;
                        const float n_y = vel_y / dist;
                        const float mass_ratio_1 = p1.get_radius() / (p1.get_radius() + p2.get_radius());
                        const float mass_ratio_2 = p2.get_radius() / (p1.get_radius() + p2.get_radius());
                        const float delta = 0.5f * response_coef * (dist - min_dist);
                        // Update positions

                        Vector2f pos1 = p1.get_position();
                        Vector2f pos2 = p2.get_position();
                        pos1.x = pos1.x - n_x * (mass_ratio_2 * delta);
                        pos1.y = pos1.y - n_y * (mass_ratio_2 * delta);
                        pos2.x = pos2.x + n_x * (mass_ratio_1 * delta);
                        pos2.y = pos2.y + n_y * (mass_ratio_1 * delta);
                        p1.set_position(pos1.x, pos1.y);
                        p2.set_position(pos2.x, pos2.y);
                        /*
                        p1.set_x(pos1.x - n_x * (mass_ratio_2 * delta));
                        p1.set_y(pos1.y - n_y * (mass_ratio_2 * delta));
                        p2.set_x(pos2.x + n_x * (mass_ratio_1 * delta));
                        p2.set_y(pos2.y + n_y * (mass_ratio_1 * delta));
                        */
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
        float default_mass = 14.f;
        // Vector2 origin = {screenWidth / 2, screenHeight / 2};
        Particle p1(600, 400, 5.f, default_mass);
        Particle p2(400, 400, 5.f, default_mass);
        // Stick stick1(p1.x, p1.y, p2.x, p2.y, utl::d_v(p1.x, p1.y, p2.x, p2.y));
        Physix motor;

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
                motor.spawn_particle(GetMouseX(), GetMouseY(), default_mass);
            }

            if (IsKeyPressed(KEY_SPACE))
            {
                // int random_x = GetRandomValue(0, screenHeight);
                // int random_y = GetRandomValue(0, screenWidth);
                // motor.apply_random_force();
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
