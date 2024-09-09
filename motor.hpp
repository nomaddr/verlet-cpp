#include <cmath>
#include <iostream>
#include "raylib.h"

typedef struct Particle
{
    Vector2 position;
    Vector2 position_prev;
    Vector2 acceleration;
    float radius;
    float mass;
    float bounce;

    Particle() = default;

    Particle(Vector2 pos, float r, float m) : position{pos}, position_prev{pos}, acceleration{0,0}, radius{r}, mass{m}, bounce{0.9f}{}
    Particle(float x, float y, float r, float m) : position{x, y}, position_prev{x, y}, acceleration{0,0}, radius{r}, mass{m}, bounce{0.9f}{}

    void update(float dt)
    {
        // Verlet Integration
        const float velocity_x = position.x - position_prev.x;
        const float velocity_y = position.y - position_prev.y;
        // update previous position
        position_prev.x = position.x;
        position_prev.y = position.y;
        // compute next position
        position.x += velocity_x + acceleration.x * (dt * dt);
        position.y += velocity_y + acceleration.y * (dt * dt);
        acceleration.x = {};
        acceleration.y = {};
    }

    void draw()
    {
         DrawCircle(position.x, position.y, radius, RAYWHITE);
        //DrawCircle(position.x, position.y, radius, Color{81, 198, 232, 255});
    }

    // compute acceleration using a=F/m
    void accelerate(float force_x, float force_y)
    {
        acceleration.x = force_x / mass;
        acceleration.y = force_y / mass;
    }

    Vector2 get_velocity(float dt)
    {
        return Vector2{(position.x - position_prev.x) / dt, (position.y - position_prev.y) / dt};
    }

    // set velocity by changing previous (x, y)
    void set_velocity(float v_x, float v_y, float dt)
    {
        position_prev.x = position.x - (v_x * dt);
        position_prev.y = position.y - (v_y * dt);
    }

    void add_velocity(float scale, float dt)
    {
        position_prev.x -= (scale * dt);
        position_prev.y -= (scale * dt);
    }

    void constrain_rectangle(int width, int height)
    {
        const float velocity_x = position.x - position_prev.x;
        const float velocity_y = position.y - position_prev.y;

        if (position.x - radius <= 0)
        {
            position.x = radius;
            position_prev.x = position.x + velocity_x * bounce;
        }
        else if (position.y - radius <= 0.f)
        {
            position.y = radius;
            position_prev.y = position.y + velocity_y * bounce;
        }
        else if (position.x + radius >= width)
        {
            position.x = width - radius;
            position_prev.x = position.x + velocity_x * bounce;
        }

        else if (position.y + radius >= height)
        {
            position.y = height - radius;
            position_prev.y = position.y + velocity_y * bounce;
        }
    }
} Particle;

class Motor
{
public:
    std::vector<Particle> particles;
    // std::vector<Stick> sticks;
    float force_x;
    float force_y;

    Motor(float force_x, float force_y)
    {
        this->force_x = force_x;
        this->force_y = force_y;
    }

    void update(float dt)
    {
        apply_force(force_x, force_y);
        check_collision_particles(dt);
        // applyConstraint();
        update_positions(dt);
    }

    void draw()
    {
        // for (Stick &s : sticks)
        // {
        //     s.draw();
        // }
        for (Particle &p : particles)
        {
            p.draw();
        }
        draw_debug();
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

    void spawn_particle(float x, float y, float radius, float mass)
    {
        Particle particle{x, y, radius, mass};
        particles.push_back(particle);
    }

    // void spawn_stick(Vector2 a, Vector2 b, float length)
    // {
    //     Stick stick(a, b, length);
    //     sticks.push_back(stick);
    // }

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
                const float vel_x = p1.position.x - p2.position.y;
                const float vel_y = p1.position.y - p2.position.y;

                const float dist2 = vel_x * vel_x + vel_y * vel_y;
                const float min_dist = p1.radius + p2.radius;
                // Check overlapping
                if (dist2 < min_dist * min_dist)
                {
                    const float dist = sqrt(dist2);
                    const float n_x = vel_x / dist;
                    const float n_y = vel_y / dist;
                    const float mass_ratio_1 = p1.radius / (p1.radius + p2.radius);
                    const float mass_ratio_2 = p2.radius / (p1.radius + p2.radius);
                    const float delta = 0.5f * response_coef * (dist - min_dist);
                    
                    // Update positions
                    Vector2 pos1 = p1.position;
                    Vector2 pos2 = p2.position;
                    pos1.x = pos1.x - n_x * (mass_ratio_2 * delta);
                    pos1.y = pos1.y - n_y * (mass_ratio_2 * delta);
                    pos2.x = pos2.x + n_x * (mass_ratio_1 * delta);
                    pos2.y = pos2.y + n_y * (mass_ratio_1 * delta);
                    p1.position = {pos1.x, pos1.y};
                    p2.position = {pos2.x, pos2.y};
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

    void draw_debug()
    {
        Color dbg_col = ORANGE;
        DrawRectangleLines(10, 10, 200, 200, dbg_col);
        DrawText("Particles:", 15, 15, 18, dbg_col);
        DrawText(std::to_string(particles.size()).c_str(), 100, 15, 18, dbg_col);
        DrawText("FPS:", 15, 35, 18, dbg_col);
        DrawText(std::to_string(GetFPS()).c_str(), 100, 35, 18, dbg_col);
    }
};