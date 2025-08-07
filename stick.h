#pragma once

#include "particle.h"

class Stick
{
public:

    Stick(int particle_a_index, int particle_b_index, float length);
    ~Stick();
    void update(std::vector<Particle>& particles);
    void draw(std::vector<Particle>& particles);
    
private:
    int particle_a_index;
    int particle_b_index;
    float length;
};
