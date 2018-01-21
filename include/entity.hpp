#include <box.h>
#include "Vector2D.hpp"
#include <vector>
#include <math.h>
#include <string>

#define COLLISION_THRESHOLD (3500)

using namespace std;

class Entity
{
public:
	void addPosition(Vector2D p);
	Vector2D lastPosition();
	// Gets the speed in px per frame
	Vector2D getVelocity() const;
	string entityType;
    int lastFrame;
    int calculateRisk(const Entity& ent);

private:
	vector<Vector2D> history;
};