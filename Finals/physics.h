#include <raylib.h>
#include <raymath.h>
#include <vector>

struct Player
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 aim;
    int width, height;
    Color color;
    int hitObstacle;
    int orientation;
};

struct Wall
{
    Vector2 position;
    int width, height;
    Rectangle rect;
    Color color;
};

struct Projectile
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float radius;
    Color color;
    float timeToDisappear;
    float uptime;
    bool isActive;
};

int clamp(int val, int min, int max)
{
	if (val < min)
	{
		return min;
	}
	else if (val > max)
	{
		return max;
	}
	return val;
}


bool CheckCollision(Player &p1, Wall &w)
{

    if (p1.position.x - (p1.width/2) < w.position.x + (w.width/2)  &&
        p1.position.x + (p1.width/2)  > w.position.x - (w.width/2) &&
        p1.position.y - (p1.height/2) < w.position.y + (w.height/2)  &&
        p1.position.y + (p1.height/2) > w.position.y - (w.height/2)
    ){
        return true;
    }
    return false;
}

// Vector2 CircleToRectangleCollisionVector(Projectile &p, RectObject &rect)
// {
// 	Vector2 clampedPos = { 
// 		clamp(c1.center.x, rect.bottomLeftPixel.x, rect.bottomLeftPixel.x + rect.width), 
// 		clamp(c1.center.y, rect.bottomLeftPixel.y, rect.bottomLeftPixel.y + rect.height) 
// 	};

// 	return VectorSubtract(c1.center, clampedPos);
// }

// //Checks if a circle object is colliding with a rectangle
// bool IsCircleToRectangleColliding(CircleObject &c1, RectObject &rect)
// {
// 	Vector2 clampedPos = { 
// 		clamp(c1.center.x, rect.bottomLeftPixel.x, rect.bottomLeftPixel.x + rect.width), 
// 		clamp(c1.center.y, rect.bottomLeftPixel.y, rect.bottomLeftPixel.y + rect.height) 
// 	};

// 	Vector2 collisionVector = CircleToRectangleCollisionVector(c1, rect);

// 	if (DotProduct(collisionVector, collisionVector) <= pow(c1.radius, 2))
// 	{
// 		return true;
// 	}
// 	return false;
// }