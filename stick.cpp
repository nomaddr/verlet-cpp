#include "stick.h"
#include "particle.h"
#include "math.h"

    Stick::Stick(Particle* a, Particle* b, float length)
    {
        this->a = a;
        this->b = b;
        this->length = length;
    }

    Stick::~Stick()
    {
    }

    void Stick::update()
    {
        float dx = this->b->get_x() - this->a->get_x();
        float dy = this->b->get_y() - this->a->get_y();
        float dist = sqrtf(dx * dx + dy * dy);
        float diff = this->length - dist;
        float perc = (dist / diff) / 2;

        float offset_x = dx * perc;
        float offset_y = dy * perc;

        this->a->set_x(this->a->get_x() - offset_x);
        this->a->set_y(this->a->get_y() - offset_y);
        this->b->set_x(this->b->get_x() + offset_x);
        this->b->set_y(this->b->get_y() + offset_y);
    }

    void Stick::draw()
    {
        DrawLineEx({a->get_x(), a->get_y()}, {b->get_x(), b->get_y()}, 7.5f, PURPLE);
        DrawText("PARTICLE A", a->get_x(), a->get_y(), 4.f, RAYWHITE);
    }

