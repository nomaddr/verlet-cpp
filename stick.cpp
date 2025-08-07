#include "stick.h"
#include "particle.h"
#include "raylib.h"
#include <cmath>

    Stick::Stick(int particle_a_index, int particle_b_index, float length)
    {
        this->particle_a_index = particle_a_index;
        this->particle_b_index = particle_b_index;
        this->length = length;
    }

    Stick::~Stick()
    {
    }

    void Stick::update(std::vector<Particle>& particles)
    {
        // Bounds checking to prevent segfault
        if (particle_a_index < 0 || particle_a_index >= particles.size() ||
            particle_b_index < 0 || particle_b_index >= particles.size()) {
            return; // Invalid indices, skip this stick
        }
        
        Particle& a = particles[particle_a_index];
        Particle& b = particles[particle_b_index];
        
        float dx = b.get_x() - a.get_x();
        float dy = b.get_y() - a.get_y();
        float dist = sqrtf(dx * dx + dy * dy);
        
        // Avoid division by zero
        if (dist < 0.001f) {
            return;
        }
        
        float diff = this->length - dist;
        // Fixed constraint math: percentage of correction needed
        float perc = diff / (2.0f * dist);

        float offset_x = dx * perc;
        float offset_y = dy * perc;

        a.set_x(a.get_x() - offset_x);
        a.set_y(a.get_y() - offset_y);
        b.set_x(b.get_x() + offset_x);
        b.set_y(b.get_y() + offset_y);
    }

    void Stick::draw(std::vector<Particle>& particles)
    {
        // Bounds checking to prevent segfault
        if (particle_a_index < 0 || particle_a_index >= particles.size() ||
            particle_b_index < 0 || particle_b_index >= particles.size()) {
            return; // Invalid indices, skip drawing this stick
        }
        
        Particle& a = particles[particle_a_index];
        Particle& b = particles[particle_b_index];
        
        DrawLineEx({a.get_x(), a.get_y()}, {b.get_x(), b.get_y()}, 7.5f, PURPLE);
        DrawText("PARTICLE A", a.get_x(), a.get_y(), 4.f, RAYWHITE);
    }

