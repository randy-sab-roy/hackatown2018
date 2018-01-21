#import <ostream>
class Vector2D
{
  public:
    Vector2D();
    Vector2D(double x, double y);
    Vector2D(const Vector2D& v);

    Vector2D operator-(Vector2D vec);
    double operator*(Vector2D vec);
    double magnitude2();
    double distance(Vector2D);

    double x;
    double y;

    friend std::ostream& operator<<(std::ostream& o, const Vector2D& v) {
        o << "{" << v.x << ", " << v.y << "}";
        return o;
    }
};