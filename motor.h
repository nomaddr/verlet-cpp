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
    void spawn_stick(int particle_a_index, int particle_b_index, float length);
    void check_collision_particles(float dt);
    void draw_debug();

private:
    void narrowPhaseResolve(int i, int j, float dt, float response_coef);
    // void applyConstraint()
};