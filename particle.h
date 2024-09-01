#pragma once

#include "raylib.h"

class Particle 
{
public:
    float x,
        y,
        prev_x,
        prev_y,
        acceleration_x,
        acceleration_y;
    float radius,
        mass,
        bounce;

    Particle(float x, float y, float radius, float mass);
    Particle(float x, float y, float radius, float mass, float bounce);
    ~Particle();
    void update(float dt);
    void accelerate(float force_x, float force_y);
    void set_velocity(float v_x, float v_y, float dt);
    void add_velocity(float scale, float dt);
    float get_velocity_x(float dt);
    float get_velocity_y(float dt);
    void constrain(int width, int height);
    bool check_collision_circle(float x, float y, float radius);
    void draw();
};