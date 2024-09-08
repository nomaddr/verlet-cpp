#pragma once

#include "raylib.h"

//Color light_blue = Color{81, 198, 232, 255};

class Particle 
{
public:
    Particle();
    Particle(float x, float y, float radius, float mass);
    Particle(Vector2 position, float radius, float mass);
    Particle(float x, float y, float radius, float mass, float bounce);
    ~Particle();
    void update(float dt);
    void draw();   
    
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
    Vector2 get_position();
    void set_position(float x, float y);
    Vector2 get_direction(float dt);
    float get_velocity_x(float dt);
    float get_velocity_y(float dt);
    void set_velocity(float v_x, float v_y, float dt);
    void add_velocity(float scale, float dt);
    void accelerate(float force_x, float force_y);
    void constrain_rectangle(int width, int height);
    bool check_collision_circle(float x, float y, float radius);
    bool check_collision_circle(Particle p);
    

private:
    Vector2 position;
    Vector2 position_prev;
    Vector2 acceleration;
    float radius;
    float mass;
    float bounce;
};