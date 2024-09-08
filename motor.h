#pragma once

#include "particle.h"
#include "stick.h"
#include <vector>

class Motor
{
public:
    Motor(float force_x, float force_y);

    std::vector<Particle> particles;
    std::vector<Stick> sticks;
    float force_x;
    float force_y;

    void update(float dt);
    void draw();
    void update_positions(float dt);
    void update_sticks();
    void apply_force(float force_x, float force_y);
    void apply_random_force();
    void spawn_particle(float x, float y, float radius, float mass);
    void spawn_stick(Particle* a, Particle* b, float length);
    void check_collision_particles(float dt);
    void draw_debug();

    // void applyConstraint()
};