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

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const float H_ACCEL = 5.0f;
const float H_COEFF = 0.1f;
const float H_OPPOSITE = 0.25;
const float H_AIR = 3.5f;
const float MAX_H_VEL = 5.0f;
const float MIN_H_VEL = 0.05f;

const float MAX_VEL = 5;
const float MAX_ACCEL = 5;
const float V_ACCEL = 8.0f;
const int WALL_GAP = 1;
const float GRAVITY = 9.8f;

struct Player
{
    Rectangle rect;
    Vector2 position;
    int width, height;
    Color color;
    Vector2 velocity;
    Vector2 acceleration;
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

    if (p1.position.x - (p1.width/2) < w.position.x + (w.width/2) + WALL_GAP &&
        p1.position.x + (p1.width/2) + WALL_GAP > w.position.x - (w.width/2) &&
        p1.position.y - (p1.height/2) < w.position.y + (w.height/2) + WALL_GAP &&
        p1.position.y + (p1.height/2) + WALL_GAP > w.position.y - (w.height/2)
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

    float currAccel = 0.0f;
    float vertAccel = 0.0f;

    bool isGrounded = false;
    bool isJumping = false;

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
    wall3.position = Vector2{400, 550};
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

        if (IsKeyDown(KEY_W) && isGrounded && !isJumping)
        {
            vertAccel = -V_ACCEL;
            // if (vertAccel <= MAX_JUMP_ACCEL)
            // {
            //     vertAccel -= MAX_JUMP_ACCEL/5;
            // }
            // else if (vertAccel > MAX_JUMP_ACCEL)
            // {
            //     vertAccel = MAX_JUMP_ACCEL;
            // }
            std::cout << "Jumping" << std::endl;
            isGrounded = false;
            isJumping = true;
        }
        
        //Still Bunny hops if W is held down pls fix or not its not required
        if (IsKeyDown(KEY_D))
        {   
            float accel = H_ACCEL;
            if (isJumping)
            {
                accel = H_AIR;
            }
        
            currAccel += accel * H_COEFF;
        }

        else if (IsKeyDown(KEY_A))
        {
            float accel = H_ACCEL;
            if (isJumping)
            {
                accel = H_AIR;
            }
        
            currAccel -= accel * H_COEFF;
        }



        if (currAccel < MIN_H_VEL && currAccel > -MIN_H_VEL)
        {
            currAccel = 0;
        }

        else if (currAccel > 0)
        {
            currAccel -= H_OPPOSITE;
        }

        else if (currAccel < 0)
        {
            currAccel += H_OPPOSITE;
        }

        if (currAccel > MAX_H_VEL)
        {
            currAccel = MAX_H_VEL;
        }

        else if (currAccel < -MAX_H_VEL)
        {
            currAccel = -MAX_H_VEL;
        }

        if (vertAccel < 0.05 && vertAccel > -0.05)
        {
            vertAccel = 0;
        }

        else if (vertAccel < 0)
        {
            vertAccel += H_OPPOSITE;
        }

        if (player.position.y > WINDOW_HEIGHT - player.height)
        {
            player.position.y = WINDOW_HEIGHT - player.height;
            player.velocity.y = 0;
            player.acceleration.y = 0;
            // vertAccel = 0;
            isGrounded = true;
            isJumping = false;
        }

        if (player.position.x + player.width > WINDOW_WIDTH)
        {
            player.position.x = WINDOW_WIDTH - player.width;
        }
        if (player.position.x < 0)
        {
            player.position.x = 0;
        }

        if (player.velocity.y >= 0 && !isGrounded)
        {
            if (vertAccel <= GRAVITY)
            {
                vertAccel += 0.5;
            }
        }

        if (player.velocity.y > MAX_VEL)
        {
            player.velocity.y = MAX_VEL;
        }

        if (player.velocity.y < -MAX_VEL)
        {
            player.velocity.y = -MAX_VEL;
        }


        Vector2 prevPos = {player.position.x, player.position.y};
        player.color = BLUE;

        player.acceleration.x = currAccel;
        player.velocity.x = player.acceleration.x;

        player.acceleration.y = vertAccel;
        player.velocity.y = player.acceleration.y;

        for (auto &wall : walls)
        {
            if (CheckCollision(player, wall))
            {
                if (player.position.y < wall.position.y - (wall.height/2) && player.velocity.y >= 0)
                {
                    player.velocity.y = 0;
                    player.acceleration.y = 0;
                    player.position.y = prevPos.y ;
                    isGrounded = true;
                    isJumping = false;
                }

                else if(player.position.x < wall.position.x - (wall.width/2) && player.velocity.x > 0)   
                {
                    player.velocity.x = 0;
                    player.position.x = prevPos.x;
                }

                else if(player.position.x - (player.width/2) > wall.position.x  && player.velocity.x < 0)
                {
                    std::cout << "Colliding" << std::endl;
                    player.velocity.x = 0;
                    player.position.x = prevPos.x;
                }
            }
        }

        player.position.x += player.velocity.x;
        player.position.y += player.velocity.y;

        

        
        BeginDrawing();
            ClearBackground(WHITE);
            DrawRectangle(player.position.x - (player.width/2), player.position.y - (player.height/2), player.width, player.height, player.color);

            for (auto &wall : walls)
            {
                DrawRectangle(wall.position.x - (wall.width/2), wall.position.y - (wall.height/2), wall.width, wall.height, wall.color);
            }
        EndDrawing();

    }

    CloseWindow();
    return 0;
}