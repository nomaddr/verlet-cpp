#include <cmath>
#include <iostream>
#include "particle.h"
#include "stick.h"
#include "motor.h"

Motor::Motor(float force_x, float force_y)
{
    this->force_x = force_x;
    this->force_y = force_y;
}

void Motor::update(float dt)
{
    apply_force(force_x, force_y);
    check_collision_particles(dt);
    // applyConstraint();
    update_positions(dt);
}

void Motor::draw()
{
    for (Stick &s : sticks)
    {
        s.draw();
    }
    for (Particle &p : particles)
    {
        p.draw();
    }
    draw_debug();
}

void Motor::update_positions(float dt)
{
    for (Particle &p : particles)
    {
        p.update(dt);
        p.constrain_rectangle(GetScreenWidth(), GetScreenHeight());
    }
}

void Motor::apply_force(float force_x, float force_y)
{
    for (Particle &p : particles)
    {
        p.accelerate(force_x, force_y);
    }
}

void Motor::apply_random_force()
{
    SetRandomSeed(GetTime());
    for (Particle &p : particles)
    {
        p.accelerate(GetRandomValue(0, GetScreenHeight()), GetRandomValue(0, GetScreenHeight()));
    }
}

void Motor::spawn_particle(float x, float y, float radius, float mass)
{
    Particle particle(x, y, radius, mass);
    particles.push_back(particle);
}

void Motor::spawn_stick(Vector2 a, Vector2 b, float length)
{
    Stick stick(a, b, length);
    sticks.push_back(stick);
}

void Motor::check_collision_particles(float dt)
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
            const float vel_x = p1.get_x() - p2.get_x();
            const float vel_y = p1.get_y() - p2.get_y();

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

                Vector2 pos1 = p1.get_position();
                Vector2 pos2 = p2.get_position();
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

// void Motor::applyConstraint()
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

void Motor::draw_debug()
{
    Color dbg_col = ORANGE;
    DrawRectangleLines(10, 10, 200, 200, dbg_col);
    DrawText("Particles:", 15, 15, 18, dbg_col);
    DrawText(std::to_string(particles.size()).c_str(), 100, 15, 18, dbg_col);
    DrawText("FPS:", 15, 35, 18, dbg_col);
    DrawText(std::to_string(GetFPS()).c_str(), 100, 35, 18, dbg_col);
}