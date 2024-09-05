#pragma once 


class Stick
{
public:
    Vector2 a;
    Vector2 b;
    float length;

    Stick(Vector2 a, Vector2 b, float length)
    {
        this->a = a;
        this->b = b;
        this->length = length;
    }

    ~Stick()
    {
    }

    void update()
    {
        
    }
    
    void draw()
    {
        DrawLineEx({a.x, a.y}, {b.x, b.y}, 5.f, GRAY);
    }
};
