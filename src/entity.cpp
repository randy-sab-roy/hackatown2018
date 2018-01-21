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

Vector2D Entity::getVelocity()
{
    Vector2D velocity = Vector2D(0, 0);
    for (int i = 1; i < this->history.size(); i++)
    {
        velocity.x += (this->history[i - 1].x - this->history[i].x)/this->history.size();
        velocity.y += (this->history[i - 1].y - this->history[i].y)/this->history.size();
    }

    return velocity;
}

int Entity::calculateRisk(Entity ent) {
    Vector2D pos1 = this->history[0];
    Vector2D pos2 = ent.history[0];
    if ((this->getVelocity()*ent.getVelocity())/(sqrt((pos1.x*pos1.x + pos1.y*pos1.y)*(pos2.x*pos2.x + pos2.y*pos2.y))) < 0.70710678118)
    {
        // Perpendicular
//        cout << "perpendiculaire" << endl;

        return 1;
    }
    else
    {
        // Parallel
//        cout << "parallele" << endl;
        return 0;
    }
}