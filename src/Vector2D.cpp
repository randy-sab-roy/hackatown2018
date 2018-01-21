#include "../include/Vector2D.hpp"

Vector2D::Vector2D() : x(0), y(0) {}

Vector2D::Vector2D(int x, int y) : x(x), y(y) {}

int Vector2D::distance(Vector2D v) {
    int x = this->x - v.x;
    int y = this->y - v.y;
    return (x + y) * (x + y);
}