#include "particle.h"
#include "raylib.h"

Particle::Particle(float x, float y, float mass, float radius)
{
    this->x = x;
    this->y = y;
    this->prev_x = x;
    this->prev_y = y;
    this->acceleration_x = 0;
    this->acceleration_y = 0;
    this->mass = mass;
    this->radius = radius;
    this->bounce = .9f;
}

Particle::Particle(float x, float y, float mass, float radius, float bounce)
{
    this->x = x;
    this->y = y;
    this->prev_x = x;
    this->prev_y = y;
    this->acceleration_x = 0;
    this->acceleration_y = 0;
    this->mass = mass;
    this->radius = radius;
    this->bounce = bounce;
}

Particle::~Particle()
{
}

void Particle::update(float dt)
{
    // Verlet Integration
    const float velocity_x = x - prev_x;
    const float velocity_y = y - prev_y;
    prev_x = x;
    prev_y = y;
    x += velocity_x + acceleration_x * (dt * dt);
    y += velocity_y + acceleration_y * (dt * dt);
    acceleration_x = {};
    acceleration_y = {};
    //cur = Vector2Add(Vector2Add(cur, vel), Vector2Scale(acc, dt * dt));
}

// compute acceleration using a=F/m
void Particle::accelerate(float force_x, float force_y)
{
    acceleration_x = force_x / mass;
    acceleration_y = force_y / mass;
}

// set velocity by changing previous (x, y)
void Particle::set_velocity(float v_x, float v_y, float dt)
{
    prev_x = x - (v_x * dt);
    prev_y = y - (v_y * dt);
}

void Particle::add_velocity(float scale, float dt)
{
    prev_x -= (scale * dt);
    prev_y -= (scale * dt);
}

float Particle::get_velocity_x(float dt)
{
    return (x - prev_x) / dt;
    //return Vector2Scale(Vector2Subtract(cur, last), 1 / dt);
}

float Particle::get_velocity_y(float dt)
{
    return (y - prev_y) / dt;
}

void Particle::draw()
{
    DrawCircle(x, y, radius, RAYWHITE);
    DrawCircleLines(x, y, radius + 0.2f, BLACK);
    DrawCircleLines(x, y, radius + 0.4f, BLACK);

}


bool Particle::check_collision_circle(float x, float y, float radius)
{
    return CheckCollisionCircles({this->x, this->y}, this->radius, {x, y}, radius);
}

void Particle::constrain(int width, int height)
{
    const float velocity_x = x - prev_x;
    const float velocity_y = y - prev_y;
 
    if (x - radius <= 0)
    {
        x = radius;
        prev_x = x + velocity_x * bounce;
    }
    if (y - radius <= 0.f)
    {
        y = radius;
        prev_y = y + velocity_y * bounce;
    }
    if (x + radius >= width)
    {
        x = width - radius;
        prev_x = x + velocity_x * bounce;
    }

    if (y + radius >= height)
    {
        y = height - radius;
        prev_y = y + velocity_y * bounce;
    }

}
