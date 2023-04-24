#include <raylib.h>
#include <raymath.h>
#include <vector>

float H_ACCEL, H_COEFF, H_OPPOSITE, H_AIR, MAX_H_VEL, MIN_H_VEL,
    CUT_V_VEL, MAX_V_VEL, V_ACCEL, GAP, GRAVITY;
int V_SAFE, V_HOLD, CAM_TYPE;

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

struct Enemy
{
    Vector2 position;
    int width, height;
    bool isDead = false;
    bool isStopped = false;
    Color color;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 start;
    Vector2 end;

    void MoveEnemy(float accel)
    {
        acceleration.x = accel;
        velocity.x += acceleration.x;

        acceleration.y = 0.f;
        velocity.y += acceleration.y;
        
        if (velocity.y > MAX_V_VEL * 0.5)
        {
            velocity.y = MAX_V_VEL * 0.5;
        }

        if (velocity.y < -CUT_V_VEL * 0.5)
        {
            velocity.y = -CUT_V_VEL * 0.5;
        }

        if (velocity.x > MAX_H_VEL * 0.5)
        {
            velocity.x = MAX_H_VEL * 0.5;
        }

        if (velocity.x < -MAX_H_VEL * 0.5)
        {
            velocity.x = -MAX_H_VEL * 0.5;
        }

        position.y += velocity.y;
        position.x += velocity.x;
    }

    void Patrol()
    {
        float distance = Vector2Distance(start, end); // Calculate the distance between start and end points

        // Check if the enemy has reached the end point
        if (Vector2Distance(position, end) <= 1.0f)
        {
            // Swap start and end points
            Vector2 temp = start;
            start = end;
            end = temp;
        }

        // Calculate the direction vector from current position to end point
        Vector2 direction = Vector2Normalize(Vector2Subtract(end, position));

        // Update acceleration based on direction and desired speed
        acceleration = Vector2Scale(direction, H_ACCEL);

        // Call MoveEnemy function to update position based on acceleration and velocity
        MoveEnemy(acceleration.x);
    }

    void StopStart()
    {
        if (isStopped)
        {
            isStopped = false;
        }
        else
        {
            isStopped = true;
        }
        
    }

};

float clamp(float val, float min, float max)
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

Vector2 CircleToRectangleCollisionVector(Projectile &p, Enemy &e)
{
	Vector2 clampedPos = { 
		clamp(p.position.x, e.position.x, e.position.x + e.width), 
		clamp(p.position.y, e.position.y, e.position.y + e.height) 
	};

	return Vector2Subtract(p.position, clampedPos);
}

//Checks if a circle object is colliding with a rectangle
bool IsCircleToRectangleColliding(Projectile &p, Enemy &e)
{
	Vector2 collisionVector = CircleToRectangleCollisionVector(p, e);

	if (Vector2DotProduct(collisionVector, collisionVector) <= pow(p.radius, 2))
	{
		return true;
	}
	return false;
}