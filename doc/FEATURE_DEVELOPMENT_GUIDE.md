# 🎮 Feature Development Guide

## 🎯 **How to Add New Features**

This guide provides templates and patterns for extending your physics engine with new features. Each section includes complete code examples and integration steps.

---

## 🧪 **Template: Adding New Particle Types**

### **Step 1: Define the New Type**

Add to existing particle types or create new enum:
```cpp
// In particle.h - extend existing or create new enum
enum class ParticleType {
    BASIC,
    HEAVY,
    BOUNCY,
    MAGNETIC,    // New type
    EXPLOSIVE,   // New type
    LIQUID       // New type
};
```

### **Step 2: Create Specialized Particle Class**

```cpp
// magnetic_particle.h
#pragma once
#include "particle.h"

class MagneticParticle : public Particle {
private:
    float magnetic_strength;
    bool is_positive_pole;
    
public:
    MagneticParticle(float x, float y, float radius, float mass, 
                    float magnetic_strength, bool positive = true);
    
    // Override update to add magnetic behavior
    void update(float dt) override;
    
    // Magnetic-specific methods
    Vector2 calculateMagneticForce(const MagneticParticle& other) const;
    void setMagneticStrength(float strength) { magnetic_strength = strength; }
    float getMagneticStrength() const { return magnetic_strength; }
    bool isPositivePole() const { return is_positive_pole; }
    
    // Override draw for visual distinction
    void draw() override;
};
```

### **Step 3: Implement the Specialized Behavior**

```cpp
// magnetic_particle.cpp
#include "magnetic_particle.h"
#include "raymath.h"

MagneticParticle::MagneticParticle(float x, float y, float radius, float mass,
                                  float magnetic_strength, bool positive)
    : Particle(x, y, radius, mass), magnetic_strength(magnetic_strength), 
      is_positive_pole(positive) {
}

void MagneticParticle::update(float dt) {
    // Call base particle update first
    Particle::update(dt);
    
    // Add magnetic-specific behavior here if needed
    // (Most magnetic behavior will be handled in force calculation)
}

Vector2 MagneticParticle::calculateMagneticForce(const MagneticParticle& other) const {
    Vector2 direction = Vector2Subtract(other.get_position(), this->get_position());
    float distance = Vector2Length(direction);
    
    if (distance < 0.001f) return {0, 0};
    
    // Normalize direction
    direction = Vector2Scale(direction, 1.0f / distance);
    
    // Calculate magnetic force (inverse square law)
    float force_magnitude = (magnetic_strength * other.magnetic_strength) / (distance * distance);
    
    // Same poles repel, opposite poles attract
    if (is_positive_pole == other.is_positive_pole) {
        force_magnitude *= -1; // Repel
    }
    
    return Vector2Scale(direction, force_magnitude);
}

void MagneticParticle::draw() {
    // Draw base particle
    Color particle_color = is_positive_pole ? RED : BLUE;
    DrawCircle(get_x(), get_y(), get_radius(), particle_color);
    
    // Draw magnetic field lines (simplified)
    if (magnetic_strength > 0) {
        DrawCircleLines(get_x(), get_y(), get_radius() + 5, particle_color);
    }
}
```

### **Step 4: Integration with Motor System**

```cpp
// Add to motor.h
class Motor {
private:
    std::vector<std::unique_ptr<MagneticParticle>> magnetic_particles;
    
public:
    void spawnMagneticParticle(float x, float y, float radius, float mass,
                              float magnetic_strength, bool positive = true);
    void updateMagneticForces();
};

// Add to motor.cpp
void Motor::spawnMagneticParticle(float x, float y, float radius, float mass,
                                 float magnetic_strength, bool positive) {
    auto magnetic_particle = std::make_unique<MagneticParticle>(
        x, y, radius, mass, magnetic_strength, positive);
    magnetic_particles.push_back(std::move(magnetic_particle));
}

void Motor::updateMagneticForces() {
    // Apply magnetic forces between magnetic particles
    for (size_t i = 0; i < magnetic_particles.size(); i++) {
        for (size_t j = i + 1; j < magnetic_particles.size(); j++) {
            Vector2 force = magnetic_particles[i]->calculateMagneticForce(*magnetic_particles[j]);
            
            // Apply equal and opposite forces
            magnetic_particles[i]->accelerate(force.x, force.y);
            magnetic_particles[j]->accelerate(-force.x, -force.y);
        }
    }
}

// Update the main update loop
void Motor::update(float dt) {
    apply_force(force_x, force_y);
    apply_gravity_forces();
    updateMagneticForces(); // Add this
    update_positions(dt);
    update_sticks();
    check_collision_particles_optimized(dt);
}
```

---

## 🌪️ **Template: Adding Environmental Effects**

### **Step 1: Create Wind System**

```cpp
// wind_system.h
#pragma once
#include "raylib.h"
#include <vector>

struct WindZone {
    Vector2 center;
    float radius;
    Vector2 direction;
    float strength;
    float turbulence; // Random variation
    bool enabled;
    
    WindZone(Vector2 center, float radius, Vector2 direction, float strength)
        : center(center), radius(radius), direction(direction), 
          strength(strength), turbulence(0.1f), enabled(true) {}
};

class WindSystem {
private:
    std::vector<WindZone> wind_zones;
    float global_wind_x, global_wind_y;
    
public:
    WindSystem() : global_wind_x(0), global_wind_y(0) {}
    
    void addWindZone(const WindZone& zone);
    void removeWindZone(size_t index);
    void setGlobalWind(float x, float y);
    
    Vector2 calculateWindForce(const Vector2& position) const;
    void update(float dt);
    void draw() const;
    
    // Controls
    void setEnabled(bool enabled);
    void setTurbulence(float turbulence);
};
```

### **Step 2: Implement Wind Effects**

```cpp
// wind_system.cpp
#include "wind_system.h"
#include "raymath.h"
#include <cmath>
#include <random>

void WindSystem::addWindZone(const WindZone& zone) {
    wind_zones.push_back(zone);
}

void WindSystem::removeWindZone(size_t index) {
    if (index < wind_zones.size()) {
        wind_zones.erase(wind_zones.begin() + index);
    }
}

void WindSystem::setGlobalWind(float x, float y) {
    global_wind_x = x;
    global_wind_y = y;
}

Vector2 WindSystem::calculateWindForce(const Vector2& position) const {
    Vector2 total_force = {global_wind_x, global_wind_y};
    
    for (const WindZone& zone : wind_zones) {
        if (!zone.enabled) continue;
        
        float distance = Vector2Distance(position, zone.center);
        if (distance <= zone.radius) {
            // Calculate falloff (stronger near center)
            float falloff = 1.0f - (distance / zone.radius);
            falloff = falloff * falloff; // Quadratic falloff
            
            // Add turbulence
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
            
            Vector2 turbulent_direction = {
                zone.direction.x + dis(gen) * zone.turbulence,
                zone.direction.y + dis(gen) * zone.turbulence
            };
            
            Vector2 zone_force = Vector2Scale(turbulent_direction, 
                                            zone.strength * falloff);
            total_force = Vector2Add(total_force, zone_force);
        }
    }
    
    return total_force;
}

void WindSystem::draw() const {
    for (const WindZone& zone : wind_zones) {
        if (!zone.enabled) continue;
        
        // Draw wind zone boundary
        DrawCircleLines(zone.center.x, zone.center.y, zone.radius, SKYBLUE);
        
        // Draw wind direction arrow
        Vector2 arrow_end = Vector2Add(zone.center, 
                                     Vector2Scale(zone.direction, 30.0f));
        DrawLineEx(zone.center, arrow_end, 3.0f, SKYBLUE);
        
        // Draw arrowhead
        Vector2 perp = {-zone.direction.y, zone.direction.x};
        Vector2 arrow_left = Vector2Add(arrow_end, 
                                      Vector2Scale(Vector2Add(Vector2Scale(zone.direction, -10), 
                                                            Vector2Scale(perp, 5)), 1));
        Vector2 arrow_right = Vector2Add(arrow_end, 
                                       Vector2Scale(Vector2Add(Vector2Scale(zone.direction, -10), 
                                                             Vector2Scale(perp, -5)), 1));
        DrawLineEx(arrow_end, arrow_left, 2.0f, SKYBLUE);
        DrawLineEx(arrow_end, arrow_right, 2.0f, SKYBLUE);
    }
}
```

### **Step 3: Integration with Physics**

```cpp
// Add to motor.h
#include "wind_system.h"

class Motor {
private:
    std::unique_ptr<WindSystem> wind_system;
    
public:
    void initializeWindSystem();
    WindSystem* getWindSystem() { return wind_system.get(); }
    
private:
    void applyWindForces();
};

// Add to motor.cpp
void Motor::initializeWindSystem() {
    wind_system = std::make_unique<WindSystem>();
}

void Motor::applyWindForces() {
    if (!wind_system) return;
    
    for (Particle& particle : particles) {
        Vector2 wind_force = wind_system->calculateWindForce(particle.get_position());
        
        // Wind force is proportional to surface area (radius²) but not mass
        float surface_area = particle.get_radius() * particle.get_radius() * 3.14159f;
        wind_force = Vector2Scale(wind_force, surface_area * 0.001f); // Scale factor
        
        particle.accelerate(wind_force.x, wind_force.y);
    }
}

// Update main loop
void Motor::update(float dt) {
    apply_force(force_x, force_y);
    apply_gravity_forces();
    applyWindForces(); // Add this
    update_positions(dt);
    update_sticks();
    check_collision_particles_optimized(dt);
}
```

---

## 🎨 **Template: Adding Visual Effects**

### **Step 1: Particle Trail System**

```cpp
// particle_trail.h
#pragma once
#include "raylib.h"
#include <deque>

struct TrailPoint {
    Vector2 position;
    float age;
    float alpha;
    
    TrailPoint(Vector2 pos) : position(pos), age(0), alpha(1.0f) {}
};

class ParticleTrail {
private:
    std::deque<TrailPoint> trail_points;
    int max_points;
    float max_age;
    Color trail_color;
    float width;
    
public:
    ParticleTrail(int max_points = 50, float max_age = 2.0f, 
                 Color color = WHITE, float width = 2.0f);
    
    void addPoint(Vector2 position);
    void update(float dt);
    void draw() const;
    void clear();
    
    // Configuration
    void setMaxPoints(int points) { max_points = points; }
    void setMaxAge(float age) { max_age = age; }
    void setColor(Color color) { trail_color = color; }
    void setWidth(float w) { width = w; }
};
```

### **Step 2: Implement Trail System**

```cpp
// particle_trail.cpp
#include "particle_trail.h"

ParticleTrail::ParticleTrail(int max_points, float max_age, Color color, float width)
    : max_points(max_points), max_age(max_age), trail_color(color), width(width) {
}

void ParticleTrail::addPoint(Vector2 position) {
    trail_points.emplace_front(position);
    
    // Remove excess points
    while (trail_points.size() > max_points) {
        trail_points.pop_back();
    }
}

void ParticleTrail::update(float dt) {
    // Age all points and remove old ones
    for (auto it = trail_points.begin(); it != trail_points.end();) {
        it->age += dt;
        it->alpha = 1.0f - (it->age / max_age);
        
        if (it->age > max_age) {
            it = trail_points.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleTrail::draw() const {
    if (trail_points.size() < 2) return;
    
    for (size_t i = 0; i < trail_points.size() - 1; i++) {
        const TrailPoint& current = trail_points[i];
        const TrailPoint& next = trail_points[i + 1];
        
        // Calculate alpha based on age
        unsigned char alpha = static_cast<unsigned char>(current.alpha * 255);
        Color segment_color = {trail_color.r, trail_color.g, trail_color.b, alpha};
        
        // Draw line segment with varying width
        float segment_width = width * current.alpha;
        DrawLineEx(current.position, next.position, segment_width, segment_color);
    }
}

void ParticleTrail::clear() {
    trail_points.clear();
}
```

### **Step 3: Add Trails to Particles**

```cpp
// Modify particle.h to include trails
class Particle {
private:
    std::unique_ptr<ParticleTrail> trail;
    bool trail_enabled;
    
public:
    void enableTrail(bool enabled = true, Color color = WHITE);
    void updateTrail(float dt);
    void drawTrail() const;
};

// Modify particle.cpp
void Particle::enableTrail(bool enabled, Color color) {
    trail_enabled = enabled;
    if (enabled && !trail) {
        trail = std::make_unique<ParticleTrail>(30, 1.5f, color, 3.0f);
    }
}

void Particle::updateTrail(float dt) {
    if (trail_enabled && trail) {
        trail->addPoint(position);
        trail->update(dt);
    }
}

void Particle::drawTrail() const {
    if (trail_enabled && trail) {
        trail->draw();
    }
}

void Particle::update(float dt) {
    // Existing update logic...
    
    // Update trail
    updateTrail(dt);
}

void Particle::draw() {
    // Draw trail first (behind particle)
    drawTrail();
    
    // Draw particle
    DrawCircle(position.x, position.y, radius, Color{81, 198, 232, 255});
}
```

---

## 🎯 **Template: Adding Game Mechanics**

### **Step 1: Destructible Objects**

```cpp
// destructible_object.h
#pragma once
#include "particle.h"
#include "stick.h"
#include <vector>

class DestructibleObject {
private:
    std::vector<int> particle_indices;
    std::vector<int> constraint_indices;
    float health;
    float max_health;
    bool destroyed;
    
public:
    DestructibleObject(float max_health = 100.0f);
    
    void addParticle(int particle_index);
    void addConstraint(int constraint_index);
    
    void takeDamage(float damage);
    void destroy(Motor& motor);
    
    bool isDestroyed() const { return destroyed; }
    float getHealthPercentage() const { return health / max_health; }
    
    void draw(const std::vector<Particle>& particles) const;
};
```

### **Step 2: Implement Destruction Logic**

```cpp
// destructible_object.cpp
#include "destructible_object.h"
#include "motor.h"

DestructibleObject::DestructibleObject(float max_health)
    : health(max_health), max_health(max_health), destroyed(false) {
}

void DestructibleObject::addParticle(int particle_index) {
    particle_indices.push_back(particle_index);
}

void DestructibleObject::addConstraint(int constraint_index) {
    constraint_indices.push_back(constraint_index);
}

void DestructibleObject::takeDamage(float damage) {
    if (destroyed) return;
    
    health -= damage;
    if (health <= 0) {
        destroyed = true;
    }
}

void DestructibleObject::destroy(Motor& motor) {
    if (!destroyed) return;
    
    // Remove constraints first
    for (int constraint_index : constraint_indices) {
        motor.removeConstraint(constraint_index);
    }
    
    // Apply explosion force to particles
    for (int particle_index : particle_indices) {
        Particle& particle = motor.getParticle(particle_index);
        
        // Apply random explosion force
        float explosion_force = 1000.0f;
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float force_x = cos(angle) * explosion_force;
        float force_y = sin(angle) * explosion_force;
        
        particle.accelerate(force_x, force_y);
    }
}

void DestructibleObject::draw(const std::vector<Particle>& particles) const {
    if (destroyed) return;
    
    // Draw health bar above object
    if (!particle_indices.empty()) {
        Vector2 center = {0, 0};
        for (int index : particle_indices) {
            Vector2 pos = particles[index].get_position();
            center.x += pos.x;
            center.y += pos.y;
        }
        center.x /= particle_indices.size();
        center.y /= particle_indices.size();
        
        // Health bar
        float bar_width = 50.0f;
        float bar_height = 5.0f;
        float health_percentage = getHealthPercentage();
        
        DrawRectangle(center.x - bar_width/2, center.y - 30, bar_width, bar_height, RED);
        DrawRectangle(center.x - bar_width/2, center.y - 30, 
                     bar_width * health_percentage, bar_height, GREEN);
    }
}
```

---

## 🔧 **Integration Checklist**

When adding any new feature, follow this checklist:

### **Code Integration**
- [ ] Add necessary includes to header files
- [ ] Update Motor class to manage new feature
- [ ] Add initialization in main.cpp
- [ ] Update the main update loop
- [ ] Add rendering calls in appropriate order

### **User Interface**
- [ ] Add keyboard/mouse controls
- [ ] Add debug visualization
- [ ] Add configuration options
- [ ] Update help text/documentation

### **Testing**
- [ ] Test basic functionality
- [ ] Test edge cases (boundary conditions)
- [ ] Test performance impact
- [ ] Test interaction with existing features

### **Documentation**
- [ ] Update code comments
- [ ] Add usage examples
- [ ] Update user documentation
- [ ] Add to feature list

This template system allows you to rapidly prototype and implement new features while maintaining code quality and consistency.