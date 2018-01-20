#include "../include/entity.hpp"

const int HISTORY_SIZE = 5;

void Entity::addPosition(box b)
{
    Vector2D p = Vector2D(b.x + (b.w / 2), b.y + (b.h / 2));

    while (this->history.size() >= HISTORY_SIZE)
        this->history.pop_back();
    this->history.insert(this->history.begin(), p);
}

Vector2D Entity::getVelocity()
{
    Vector2D velocity = Vector2D(0, 0);
    for (int i = 1; i < this->history.size(); i++)
    {
        velocity.x += this->history[i].x - this->history[i - 1].x;
        velocity.y += this->history[i].y - this->history[i - 1].y;
    }
    velocity.x /= this->history.size();
    velocity.y /= this->history.size();

    return velocity;
}