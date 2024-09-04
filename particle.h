#pragma once

#include "raylib.h"

struct Vector2f {
    float x{0};
    float y{0};
};

//Color light_blue = Color{81, 198, 232, 255};

class Particle 
{
public:
    Particle(float x, float y, float radius, float mass);
    Particle(float x, float y, float radius, float mass, float bounce);
    ~Particle();
    float get_radius();
    void set_radius(float radius);
    float get_mass();
    void set_mass(float mass);
    float get_bounce();
    void set_bounce(float bounce);
    float get_x();
    void set_x(float x);
    float get_y();
    void set_y(float y);
    Vector2f get_position();
    void set_position(float x, float y);
    Vector2f get_direction(float dt);

    void update(float dt);
    void draw();
    void accelerate(float force_x, float force_y);
    void set_velocity(float v_x, float v_y, float dt);
    void add_velocity(float scale, float dt);
    float get_velocity_x(float dt);
    float get_velocity_y(float dt);
    void constrain_rectangle(int width, int height);
    bool check_collision_circle(float x, float y, float radius);
    bool check_collision_circle(Particle p);
    

private:
    Vector2f position;
    Vector2f position_prev;
    Vector2f acceleration;
    float radius;
    float mass;
    float bounce;
};