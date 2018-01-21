#include <box.h>
#include "Vector2D.hpp"
#include <vector>

using namespace std;

class Entity
{
public:
	void addPosition(Vector2D p);
	Vector2D lastPosition();
	// Gets the speed in px per frame
	Vector2D getVelocity();
	int entityType;

private:
	vector<Vector2D> history;
};