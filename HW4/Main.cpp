/**
 * Authors: Dan Mark Restoles
 * Class: GDEV 42 F
 * Date: February 20, 2023
 
 This code outputs the derivative equation given a polynomial order and number of points.
**/

#include <raylib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include <settings.txt>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;


struct Player
{
    Rectangle rect;
    Vector2 position;
    int width, height;
    Color color;
    Vector2 velocity;
    Vector2 acceleration;
    int hitObstacle;
};

struct Wall
{
    Vector2 position;
    int width, height;
    Rectangle rect;
    Color color;
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
    // if (p1.rect.x < w.rect.x + w.rect.width + WALL_GAP &&
    //     p1.rect.x + p1.rect.width + WALL_GAP > w.rect.x &&
    //     p1.rect.y < w.rect.y + w.rect.height + WALL_GAP &&
    //     p1.rect.y + p1.rect.height + WALL_GAP > w.rect.y)
    // {
    //     return true;
    // }
    // return false;
}

int main()
{

    // Declaration of game
    Player player;
    player.position = Vector2{WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    player.width = 24;
    player.height = 32;
    player.color = BLUE;
    player.velocity = Vector2{0,0};
    player.acceleration = Vector2{0,0};
    player.hitObstacle = 0;

    float currAccel = 0.0f;
    float vertAccel = 0.0f;

    int framesHoldingJump = 0;
    int framesNotGrounded = 0;

    bool isGrounded = false;
    bool isJumping = false;
    bool isJumpKeyReleased = false;
    bool isOnPlatform = false;

    std::vector<Wall> walls;
    Wall wall1, wall2, wall3;
    
    wall1.position = Vector2{50, 500};
    wall1.width = 50;
    wall1.height = 200;
    wall1.color = RED;
    wall2.position = Vector2{650, 500};
    wall2.width = 50;
    wall2.height = 200;
    wall2.color = RED;
    wall3.position = Vector2{350, 350};
    wall3.width = 150;
    wall3.height = 50;
    wall3.color = RED;

    walls.push_back(wall1);
    walls.push_back(wall2);
    walls.push_back(wall3);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        
        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
        }

        if (!isGrounded)
        {
            framesNotGrounded++;
        }
        
        if (IsKeyDown(KEY_W) && framesHoldingJump < V_HOLD && !isJumpKeyReleased)
        {
            vertAccel = -V_ACCEL;
            //std::cout << "Jumping | " << vertAccel << std::endl;
            isGrounded = false;
            framesHoldingJump += 1;
        } 
        else if(isGrounded)
        {
            isJumpKeyReleased = false;
            framesHoldingJump = 0;
            framesNotGrounded = 0;
        }
        else if (framesNotGrounded >= V_SAFE && vertAccel <= GRAVITY)
        {
            vertAccel = GRAVITY;
        }
        if(IsKeyReleased(KEY_W))
        {
            vertAccel = 0;
            isJumpKeyReleased = true;
        }
        
        //Still Bunny hops if W is held down pls fix or not its not required
        if (IsKeyDown(KEY_D))
        {   
            float accel = H_ACCEL;
            if (isJumping)
            {
                accel = H_AIR;
            }
        
            currAccel += accel ;
        }

        else if (IsKeyDown(KEY_A))
        {
            float accel = H_ACCEL;
            if (isJumping)
            {
                accel = H_AIR;
            }
        
            currAccel -= accel ;
        }

        else if (player.velocity.x >= MIN_H_VEL && player.velocity.x <= -MIN_H_VEL)
        {
            player.velocity.x = 0;
        }

        else 
        {
            currAccel *= H_COEFF;
            player.velocity.x *= H_COEFF;
        }

        if (currAccel > MAX_H_VEL)
        {
            currAccel = MAX_H_VEL;
        }

        else if (currAccel < -MAX_H_VEL)
        {
            currAccel = -MAX_H_VEL;
        }

        if (player.position.x + player.width > WINDOW_WIDTH)
        {
            player.position.x = WINDOW_WIDTH - player.width;
        }
        if (player.position.x < 0)
        {
            player.position.x = 0;
        }




        Vector2 prevPos = {player.position.x, player.position.y};
        player.color = BLUE;

        int x = 0;
        for (auto &wall : walls)
        {
                if (player.position.y + player.height >= wall.position.y &&
                    player.position.y < wall.position.y + 2.0f &&
                    player.position.x + player.width > wall.position.x &&
                    player.position.x < wall.position.x + wall.width &&
                    !isOnPlatform)
                {
                    std::cout << "TOPSIDE " << x << " : " << isJumpKeyReleased  << std::endl;
                    player.position.y = wall.position.y - player.height - GAP;
                    vertAccel = 0;
                    player.velocity.y = 0;
                    isJumpKeyReleased = false;
                    isGrounded = true;
                    isOnPlatform = true;
                }
                else if(
                        player.position.y  >= wall.position.y + wall.height/2 &&
                        player.position.y -5.0f < wall.position.y + wall.height + 2 &&
                        player.position.x + player.width > wall.position.x &&
                        player.position.x < wall.position.x + wall.width
                    )
                {
                    player.position.y = wall.position.y + wall.height + GAP;
                    isJumpKeyReleased = true;
                    isGrounded = false;
                    std::cout << "BOTTOMSIDE " << x << "|" << isJumpKeyReleased << std::endl;
                }
                // check for collision with left side of wall
                else if (player.position.x + player.width > wall.position.x &&
                    player.position.x < wall.position.x &&
                    player.position.y + player.height > wall.position.y &&
                    player.position.y < wall.position.y + wall.height)
                {
                    // player hits the left side of the wall
                    player.velocity.x = 0;
                    player.position.x = wall.position.x - player.width - GAP;
                    std::cout << "LEFTSIDE " << x << "|" << isJumpKeyReleased << std::endl;
                }
                
                // check for collision with right side of wall
                else if (player.position.x < wall.position.x + wall.width &&
                    player.position.x + player.width > wall.position.x + wall.width &&
                    player.position.y + player.height > wall.position.y &&
                    player.position.y < wall.position.y + wall.height)
                {
                    // player hits the right side of the wall
                    player.velocity.x = 0;
                    player.position.x = wall.position.x + wall.width + GAP;
                    std::cout << "RIGHT " << x << "|" << isJumpKeyReleased << std::endl;
                }
                    else if (isOnPlatform && player.position.y + player.height < wall.position.y)
                {
                    // Player is on top of the wall, but not colliding with it
                    isGrounded = false;
                    isOnPlatform = false;
                }

            x++;
        }

        if (player.position.y + player.height >= WINDOW_HEIGHT )
        {
            player.position.y = WINDOW_HEIGHT - player.height - GAP;
            player.velocity.y = 0;
            player.acceleration.y = 0;
            vertAccel = 0;
            isGrounded = true;
            //isJumping = false;
        }

        player.acceleration.x = currAccel;
        player.velocity.x += player.acceleration.x;

        player.acceleration.y = vertAccel;
        player.velocity.y += player.acceleration.y;
        //std::cout << "player.velocity.y += : " << player.velocity.y << std::endl;
        
        if (player.velocity.y > MAX_V_VEL)
        {
            player.velocity.y = MAX_V_VEL;
        }

        if (player.velocity.y < -CUT_V_VEL)
        {
            player.velocity.y = -CUT_V_VEL;
        }

        if (player.velocity.x > MAX_H_VEL)
        {
            player.velocity.x = MAX_H_VEL;
        }

        if (player.velocity.x < -MAX_H_VEL)
        {
            player.velocity.x = -MAX_H_VEL;
        }

        std::cout << "Velocity: " << player.velocity.x << " | " << player.velocity.y << std::endl;
        player.position.y += player.velocity.y;
        player.position.x += player.velocity.x;

        
        BeginDrawing();
            ClearBackground(WHITE);
            DrawRectangle(player.position.x , player.position.y, player.width, player.height, player.color);

            for (auto &wall : walls)
            {
                DrawRectangle(wall.position.x, wall.position.y, wall.width, wall.height, wall.color);
            }
        EndDrawing();

    }

    CloseWindow();
    return 0;
}