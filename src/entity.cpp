#include "../include/entity.hpp"
#include <iostream>

const int HISTORY_SIZE = 2;

void Entity::addPosition(Vector2D p)
{
    while (this->history.size() >= HISTORY_SIZE)
        this->history.pop_back();
    this->history.insert(this->history.begin(), p);
}

Vector2D Entity::lastPosition()
{
    return this->history.front();
}

Vector2D Entity::getVelocity() const 
{
    Vector2D velocity = Vector2D(0, 0);
    for (int i = 1; i < this->history.size(); i++)
    {
        velocity.x += (this->history[i - 1].x - this->history[i].x)/this->history.size();
        velocity.y += (this->history[i - 1].y - this->history[i].y)/this->history.size();
    }

    return velocity;
}

int Entity::calculateRisk(const Entity& ent) {
    Vector2D pos1 = history[0];
    Vector2D pos2 = ent.history[0];

    Vector2D delta1 = this->getVelocity();
    Vector2D delta2 = ent.getVelocity();


    if (abs((delta1*delta2)/(sqrt((delta1.x*delta1.x + delta1.y*delta1.y)*(delta2.x*delta2.x + delta2.y*delta2.y)))) < 0.70710678118) {
        // Perpendicular
        Vector2D intersect(0, 0);
        if (delta1.x == 0) {
            double a2 = delta2.y / delta2.x;
            double b2 = pos2.y - a2 * pos2.x;
            intersect.x = pos1.x;
            intersect.y = a2 * intersect.x + b2;
        } else if (delta2.x == 0) {
            double a1 = delta1.y / delta1.x;
            double b1 = pos1.y - a1 * pos1.x;
            intersect.x = pos2.x;
            intersect.y = a1 * intersect.x + b1;
        }
        else
        {
            double a1= delta1.y / delta1.x;
            double a2 = delta2.y / delta2.x;
            double b1 = pos1.y - a1*pos1.x;
            double b2 = pos2.y - a2*pos2.x;
            intersect.x = (b2-b1)/(a1-a2);
            intersect.y = a1*intersect.x+b1;
        }

        double d1 = (pos1 - intersect).magnitude2()/delta1.magnitude2();
        double d2 = (pos2 - intersect).magnitude2()/delta2.magnitude2();

        if(d1 > 0 && d2 > 0)
        {
            if(abs(d1 - d2) < COLLISION_THRESHOLD)
            {
                return 1000/abs(d1 - d2);
            }
        }

        return 0;
    }
    else
    {
        // Parallel
        return 0;
    }
}