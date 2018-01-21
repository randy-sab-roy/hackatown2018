#include "../include/Vector2D.hpp"

Vector2D::Vector2D() : x(0), y(0) {}

Vector2D::Vector2D(double x, double y) : x(x), y(y) {}

Vector2D::Vector2D(const Vector2D& v){
    x = v.x;
    y = v.y;
}


double Vector2D::operator*(Vector2D vec)
{
    return this->x * vec.x + this->y * vec.y;
}

Vector2D Vector2D::operator-(Vector2D vec)
{
    return Vector2D(this->x - vec.x, this->y - vec.y);
}

double Vector2D::magnitude2()
{
    return (this->x * this->x) + (this->y * this->y);
}

double Vector2D::distance(Vector2D v) {
    double x = this->x - v.x;
    double y = this->y - v.y;
    return (x + y) * (x + y);
}

