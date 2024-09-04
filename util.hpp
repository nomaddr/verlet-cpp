#pragma once

#include <math.h>

namespace utl
{
    


        // Vector Magnitude / dist of 2 points
        float d_v(float p1_x, float p1_y, float p2_x, float p2_y)
        {
            float dx = p2_x - p1_x;
            float dy = p2_y - p1_y;
            return sqrtf(dx * dx + dy * dy);
        }
        // Vector Length /
        float d_p(float p1_x, float p1_y, float p2_x, float p2_y)
        {
            float dx = p2_x - p1_x;
            float dy = p2_y - p2_y;
            return (dx * dx+ dy * dy);
        }

}

class Stick
{
public:
    Vector2f a;
    Vector2f b;
    float length;

    Stick(Vector2f a, Vector2f b, float length)
    {
        this->a = a;
        this->b = b;
        this->length = length;
    }

    ~Stick()
    {
    }
    
    void draw()
    {
        DrawLineEx({a.x, a.y}, {b.x, b.y}, 5.f, GRAY);
    }
};