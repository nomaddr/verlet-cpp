#pragma once

#include "particle.h"

class Stick
{
public:

    Stick(Particle* a, Particle* b, float length);
    ~Stick();
    void update();
    void draw();
    
private:
    Particle* a;
    Particle* b;
    float length;
};
