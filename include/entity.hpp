#include <box.h>
#include "Vector2D.hpp"
#include <vector>

using namespace std;

class Entity
{
public:
	void addPosition(box b);
	// Gets the speed in px per frame
	Vector2D getVelocity();

private:
	vector<Vector2D> history;
};