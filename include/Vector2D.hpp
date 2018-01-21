class Vector2D
{
  public:
    Vector2D();
    Vector2D(int x, int y);
    int operator*(Vector2D vec);
    int distance(Vector2D);
    int x;
    int y;
};