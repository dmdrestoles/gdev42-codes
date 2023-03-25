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
#include <fstream>
#include <string>

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
    player.color = BLUE;
    player.velocity = Vector2{0,0};
    player.acceleration = Vector2{0,0};
    player.hitObstacle = 0;
    player.width = 24;
    player.height = 32;

    float currAccel = 0.0f;
    float vertAccel = 0.0f;

    int framesHoldingJump = 0;
    int framesNotGrounded = 0;

    bool isGrounded = false;
    bool isJumping = false;
    bool isJumpKeyReleased = false;
    bool isOnPlatform = false;
    std::ifstream constantsFile("settings.txt");

  std::string line;
    float H_ACCEL, H_COEFF, H_OPPOSITE, H_AIR, MAX_H_VEL, MIN_H_VEL,
        CUT_V_VEL, MAX_V_VEL, V_ACCEL, GAP, GRAVITY;
    int V_SAFE, V_HOLD;

    while (std::getline(constantsFile, line)) {
        std::string variable, value;
        std::size_t equalsPos = line.find('=');
        variable = line.substr(0, equalsPos);
        value = line.substr(equalsPos + 1);

        if (variable == "H_ACCEL") {
        H_ACCEL = std::stof(value);
        } else if (variable == "H_COEFF") {
        H_COEFF = std::stof(value);
        } else if (variable == "H_OPPOSITE") {
        H_OPPOSITE = std::stof(value);
        } else if (variable == "H_AIR") {
        H_AIR = std::stof(value);
        } else if (variable == "MAX_H_VEL") {
        MAX_H_VEL = std::stof(value);
        } else if (variable == "MIN_H_VEL") {
        MIN_H_VEL = std::stof(value);
        } else if (variable == "V_SAFE") {
        V_SAFE = std::stoi(value);
        } else if (variable == "V_HOLD") {
        V_HOLD = std::stoi(value);
        } else if (variable == "CUT_V_VEL") {
        CUT_V_VEL = std::stof(value);
        } else if (variable == "MAX_V_VEL") {
        MAX_V_VEL = std::stof(value);
        } else if (variable == "V_ACCEL") {
        V_ACCEL = std::stof(value);
        } else if (variable == "GAP") {
        GAP = std::stof(value);
        } else if (variable == "GRAVITY") {
        GRAVITY = std::stof(value);
        }
    }

     // Read from text file
    std::ifstream inputFile("level.txt");
    if (!inputFile.is_open())
    {
        std::cout << "Error opening file." << std::endl;
        return 0;
    }

    // Read player position from first line
    float playerX, playerY;
    inputFile >> playerX >> playerY;

    // Player dimensions
    const int playerWidth = 24;
    const int playerHeight = 32;

    // Instantiate player
    player.position = Vector2{playerX,playerY};

    // Read number of walls from second line
    int numWalls;
    inputFile >> numWalls;

    // Instantiate walls
    std::vector<Wall> walls;
    for (int i = 0; i < numWalls; i++)
    {
        // Read wall position and dimensions
        float wallX, wallY;
        float wallWidth, wallHeight;
        inputFile >> wallX >> wallY >> wallWidth >> wallHeight;

        // Instantiate wall
        Wall wall;
        wall.position = {wallX, wallY};
        wall.width = wallWidth;
        wall.height = wallHeight;
        // wall.rect = {wallX, wallY, wallWidth, wallHeight};
        wall.color = RED;

        // Add wall to walls vector
        walls.push_back(wall);
    }

    // Close input file
    inputFile.close();

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer");
    SetTargetFPS(60);
    Camera2D camera = {0};
    Vector4 cameraBounds {100, 100, 100, 100}; // {-100, 100, -50, 50}
    camera.target = {player.position.x, player.position.y};
    camera.offset = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};

    float driftX, driftY;
    driftX = 0;
    driftY = 0;
    float CAM_DRIFT = 2;

    while (!WindowShouldClose())
    {
        float leftEdge = camera.target.x - cameraBounds.x;
        float rightEdge = camera.target.x + cameraBounds.y;
        float topEdge = camera.target.y - cameraBounds.z;
        float bottomEdge = camera.target.y + cameraBounds.w;

        if (player.position.x < leftEdge)
        {
            int push = leftEdge - player.position.x;
            camera.target.x -= push;
        }
        else if (player.position.x > rightEdge)
        {
            int push = player.position.x - rightEdge;
            camera.target.x += push;
        }

        if (player.position.y > bottomEdge)
        {
            int push = player.position.y - bottomEdge;
            camera.target.y += push;
        }
        else if (player.position.y < topEdge)
        {
            int push = topEdge - player.position.y; 
            camera.target.y -= push;
        }

        if (camera.target.x <= 0)
        {
            camera.target.x = 0;
        }
        else if(camera.target.x >= WINDOW_WIDTH)
        {
            camera.target.x = WINDOW_WIDTH;
        }

        
        driftX = clamp(player.position.x - camera.target.x, -CAM_DRIFT, CAM_DRIFT);
        camera.zoom = 1.0f;

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
            isGrounded = false;
            framesHoldingJump += 1;
        } 
        else if(isGrounded)
        {
            isJumpKeyReleased = false;
            framesHoldingJump = 0;
            framesNotGrounded = 0;
            driftY = clamp(player.position.y - camera.target.y, -CAM_DRIFT, CAM_DRIFT);
            camera.target.y += driftY;
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
        
        // camera.target.x += driftX;

        Vector2 prevPos = {player.position.x, player.position.y};
        player.color = BLUE;

        int x = 0;
        for (auto &wall : walls)
        {
                if (player.position.y + player.height >= wall.position.y -GAP&&
                    player.position.y < wall.position.y + 5.0f &&
                    player.position.x + player.width > wall.position.x &&
                    player.position.x < wall.position.x + wall.width &&
                    (!IsKeyPressed(KEY_W) && framesHoldingJump >= 0))
                {
                    player.position.y = wall.position.y - player.height - GAP;
                    vertAccel = 0;
                    player.velocity.y = 0;
                    framesHoldingJump = 0;
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
                    vertAccel = GRAVITY;
                    isGrounded = false;
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

        player.position.y += player.velocity.y;
        player.position.x += player.velocity.x;

        
        BeginDrawing();
            ClearBackground(WHITE);
            BeginMode2D(camera);
            DrawRectangle(player.position.x , player.position.y, player.width, player.height, player.color);

            for (auto &wall : walls)
            {
                DrawRectangle(wall.position.x, wall.position.y, wall.width, wall.height, wall.color);
            }
            EndMode2D();
        EndDrawing();

    }

    CloseWindow();
    return 0;
}