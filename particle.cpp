#include "particle.h"
#include "raylib.h"

Particle::Particle(float x, float y, float mass, float radius)
{
    this->position.x = x;
    this->position.y = y;
    this->position_prev.x = x;
    this->position_prev.y = y;
    this->acceleration.x = 0;
    this->acceleration.y = 0;
    this->mass = mass;
    this->radius = radius;
    this->bounce = .9f;
}

Particle::Particle(float x, float y, float mass, float radius, float bounce)
{
    this->position.x = x;
    this->position.y = y;
    this->position_prev.x = x;
    this->position_prev.y = y;
    this->acceleration.x = 0;
    this->acceleration.y = 0;
    this->mass = mass;
    this->radius = radius;
    this->bounce = bounce;
}

Particle::~Particle() {}

void Particle::update(float dt)
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

void Particle::draw()
{
    DrawCircle(position.x, position.y, radius, BLACK);
    DrawCircle(position.x, position.y, radius - 0.5f, Color{81, 198, 232, 255});

}

// compute acceleration using a=F/m
void Particle::accelerate(float force_x, float force_y)
{
    acceleration.x = force_x / mass;
    acceleration.y = force_y / mass;
}

// set velocity by changing previous (x, y)
void Particle::set_velocity(float v_x, float v_y, float dt)
{
    position_prev.x = position.x - (v_x * dt);
    position_prev.y = position.y - (v_y * dt);
}

void Particle::add_velocity(float scale, float dt)
{
    position_prev.x -= (scale * dt);
    position_prev.y -= (scale * dt);
}

float Particle::get_velocity_x(float dt)
{
    return (position.x - position_prev.x) / dt;
}

float Particle::get_velocity_y(float dt)
{
    return (position.y - position_prev.y) / dt;
}

Vector2f Particle::get_direction(float dt)
{
    return {this->get_velocity_x(dt), this->get_velocity_y(dt)};
}

bool Particle::check_collision_circle(float x, float y, float radius)
{
    return CheckCollisionCircles(Vector2{this->position.x, this->position.y}, this->radius, Vector2{x, y}, radius);
}

bool Particle::check_collision_circle(Particle p)
{
    return CheckCollisionCircles(Vector2{this->position.x, this->position.y}, this->radius, Vector2{p.get_x(), p.get_y()}, p.get_radius());
}

void Particle::constrain_rectangle(int width, int height)
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

float Particle::get_radius()
{
    return this->radius;
}

void Particle::set_radius(float radius)
{
    this->radius = radius;
}

float Particle::get_mass()
{
    return this->mass;
}

void Particle::set_mass(float mass)
{
    this->mass = mass;
}

float Particle::get_bounce()
{
    return this->bounce;
}

void Particle::set_bounce(float bounce)
{
    this->bounce = bounce;
}

float Particle::get_x()
{
    return this->position.x;
}

void Particle::set_x(float x)
{
    this->position.x = x;
}

float Particle::get_y()
{
    return this->position.y;
}

void Particle::set_y(float y)
{
    this->position.y = y;
}

Vector2f Particle::get_position()
{
    return this->position;
}

void Particle::set_position(float x, float y)
{
    this->position.x = x;
    this->position.y = y;
}
;