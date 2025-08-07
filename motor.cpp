#include <cmath>
#include <iostream>
#include <string>
#include "raylib.h"
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
    // Correct physics update order:
    // 1. Apply forces to particles
    apply_force(force_x, force_y);
    
    // 2. Update particle positions using Verlet integration
    update_positions(dt);
    
    // 3. Apply constraints (sticks) to maintain structural integrity
    update_sticks();
    
    // 4. Resolve collisions between particles
    check_collision_particles(dt);
    
    // applyConstraint(); // TODO: Implement boundary constraints
}

void Motor::draw()
{
    
    for (Particle &p : particles)
    {
        p.draw();
    }
    for (Stick &s : sticks)
    {
        s.draw(particles);
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

void Motor::update_sticks()
{
    
    for (Stick &s : sticks)
    {
        s.update(particles);
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
        // Apply random force in a more reasonable range
        float random_force_x = GetRandomValue(-500, 500);
        float random_force_y = GetRandomValue(-500, 500);
        p.accelerate(random_force_x, random_force_y);
    }
}

void Motor::spawn_particle(float x, float y, float radius, float mass)
{
    // Add bounds checking and reasonable limits
    if (radius <= 0.0f || radius > 100.0f) {
        radius = 5.0f; // Default radius
    }
    if (mass <= 0.0f || mass > 1000.0f) {
        mass = 1.0f; // Default mass
    }
    
    // Limit total number of particles to prevent performance issues
    if (particles.size() >= 500) {
        return; // Don't add more particles
    }
    
    Particle particle(x, y, radius, mass);
    particles.push_back(particle);
}

void Motor::spawn_stick(int particle_a_index, int particle_b_index, float length)
{
    // Bounds checking to ensure valid indices
    if (particle_a_index < 0 || particle_a_index >= particles.size() ||
        particle_b_index < 0 || particle_b_index >= particles.size() ||
        particle_a_index == particle_b_index) {
        return; // Invalid indices or same particle, don't create stick
    }
    
    Stick stick(particle_a_index, particle_b_index, length);
    sticks.push_back(stick);
}

void Motor::check_collision_particles(float dt)
{
    const float response_coef = 0.75f;
    const int particle_count = particles.size();
    
    // Early exit if not enough particles for collision
    if (particle_count < 2) {
        return;
    }

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
                
                // Avoid division by zero
                if (dist < 0.001f) {
                    continue;
                }
                
                const float n_x = vel_x / dist;
                const float n_y = vel_y / dist;
                
                // Use actual mass for proper physics response
                const float total_mass = p1.get_mass() + p2.get_mass();
                const float mass_ratio_1 = p2.get_mass() / total_mass; // p1 moves based on p2's mass
                const float mass_ratio_2 = p1.get_mass() / total_mass; // p2 moves based on p1's mass
                
                const float overlap = min_dist - dist;
                const float separation = overlap * response_coef;
                
                // Update positions based on mass ratios
                Vector2 pos1 = p1.get_position();
                Vector2 pos2 = p2.get_position();
                
                // Move particles apart proportional to their mass ratios
                pos1.x = pos1.x - n_x * (mass_ratio_1 * separation);
                pos1.y = pos1.y - n_y * (mass_ratio_1 * separation);
                pos2.x = pos2.x + n_x * (mass_ratio_2 * separation);
                pos2.y = pos2.y + n_y * (mass_ratio_2 * separation);
                
                p1.set_position(pos1.x, pos1.y);
                p2.set_position(pos2.x, pos2.y);
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
    DrawRectangleLines(10, 10, 200, 80, dbg_col);
    DrawText("Particles:", 15, 15, 18, dbg_col);
    DrawText(std::to_string(particles.size()).c_str(), 100, 15, 18, dbg_col);
    DrawText("FPS:", 15, 35, 18, dbg_col);
    DrawText(std::to_string(GetFPS()).c_str(), 100, 35, 18, dbg_col);
    DrawText("Sticks:", 15, 55, 18, dbg_col);
    DrawText(std::to_string(sticks.size()).c_str(), 100, 55, 18, dbg_col);

    // Draw Particle IDs (only for reasonable number of particles to avoid performance issues)
    if (particles.size() <= 50) {
        for (int i = 0; i < particles.size(); i++)
        {
            // Use static buffer to avoid string allocation every frame
            char id_text[8];
            snprintf(id_text, sizeof(id_text), "%d", i);
            DrawText(id_text, particles[i].get_x() - 5, particles[i].get_y() - 5, 12, BLACK);
        }
    }
}
