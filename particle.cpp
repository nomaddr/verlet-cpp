#include "particle.h"
#include "raylib.h"
#include <raymath.h>

Particle::Particle() {};

Particle::Particle(float x, float y, float radius, float mass)
    : position{ x, y }, position_prev{ x, y }, acceleration{ 0, 0 }, radius(radius), mass(mass), bounce(0.9f) {}

Particle::Particle(Vector2 pos, float radius, float mass)
    : position{ pos }, position_prev{ pos }, acceleration{ 0, 0 }, radius(radius), mass(mass), bounce(0.9f) {}

Particle::Particle(float x, float y, float radius, float mass, float bounce)
    : position{ x, y }, position_prev{ x, y }, acceleration{ 0, 0 }, radius(radius), mass(mass), bounce(bounce) {}

Particle::~Particle() {}

void Particle::update(float dt) {
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

void Particle::draw() {
  // DrawCircle(position.x, position.y, radius, BLACK);
  DrawCircle(position.x, position.y, radius, Color{81, 198, 232, 255});
}

// compute acceleration using a=F/m
void Particle::accelerate(float force_x, float force_y) {
  acceleration.x = force_x / mass;
  acceleration.y = force_y / mass;
}

// Set velocity by adjusting previous position for Verlet integration
void Particle::set_velocity(float v_x, float v_y, float dt) {
  // Ensure dt is not zero to avoid invalid calculations
  if (dt < 0.001f) {
    return;
  }

  position_prev.x = position.x - (v_x * dt);
  position_prev.y = position.y - (v_y * dt);
}

void Particle::add_velocity(float scale, float dt) {
  position_prev.x -= (scale * dt);
  position_prev.y -= (scale * dt);
}

float Particle::get_velocity_x(float dt) {
  if (dt <= 1e-6f) return 0.0f;
  return (position.x - position_prev.x) / dt;
}

float Particle::get_velocity_y(float dt) {
  if (dt <= 1e-6f) return 0.0f;
  return (position.y - position_prev.y) / dt;
}

Vector2 Particle::get_velocity(float dt) {
  return {get_velocity_x(dt), get_velocity_y(dt)};
}

Vector2 Particle::get_direction(float dt) {
  if (dt <= 1e-6f) return Vector2{0.0f, 0.0f};
  return Vector2Scale(Vector2Subtract(position, position_prev), 1.0f / dt);
}

bool Particle::check_collision_circle(float x, float y, float r) {
  return CheckCollisionCircles(Vector2{position.x, position.y}, this->radius,
                               Vector2{x, y}, r);
}

bool Particle::check_collision_circle(Particle p) {
  return CheckCollisionCircles(Vector2{position.x, position.y}, radius,
                               Vector2{p.get_x(), p.get_y()}, p.get_radius());
}

void Particle::constrain_rectangle(int width, int height) {
  const float velocity_x = position.x - position_prev.x;
  const float velocity_y = position.y - position_prev.y;

  // Handle X axis independently
  if (position.x - radius <= 0.0f) {
    position.x = radius;
    position_prev.x = position.x + velocity_x * bounce;
  } else if (position.x + radius >= width) {
    position.x = width - radius;
    position_prev.x = position.x + velocity_x * bounce;
  }

  // Handle Y axis independently
  if (position.y - radius <= 0.0f) {
    position.y = radius;
    position_prev.y = position.y + velocity_y * bounce;
  } else if (position.y + radius >= height) {
    position.y = height - radius;
    position_prev.y = position.y + velocity_y * bounce;
  }
}

float Particle::get_radius() { return radius; }

void Particle::set_radius(float r) { this->radius = r; }

float Particle::get_mass() { return mass; }

void Particle::set_mass(float m) { this->mass = m; }

float Particle::get_bounce() { return bounce; }

void Particle::set_bounce(float b) { this->bounce = b; }

float Particle::get_x() { return position.x; }

void Particle::set_x(float x) { position.x = x; }

float Particle::get_y() { return position.y; }

void Particle::set_y(float y) { position.y = y; }

Vector2 Particle::get_position() { return position; }

void Particle::set_position(float x, float y) {
  position.x = x;
  position.y = y;
};
