/**
 * Authors: Dan Mark Restoles, Jared Javillo, Catherine Golles
 * Class: GDEV 42 F
 * SHOOTY SHOOTY BANG BANG
**/

#include "physics.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int MAX_PLAYER_PROJECTILES = 1;
int TARGET_FPS = 60;

Vector2 CAM_DRIFT;

float aimLength = 10;
// Declaration of game
Player player;

float currAccel = 0.0f;
float vertAccel = 0.0f;
float timerTarget = 1.0f; // 1 second
float timerElapsed = 0.0f;

int framesHoldingJump = 0;
int framesNotGrounded = 0;
int enemyDeadCount = 0;

char timerText[10];

bool isGrounded = false;
bool isJumping = false;
bool isJumpKeyReleased = false;
bool isOnPlatform = false;
std::ifstream constantsFile("settings.txt");

std::string line;

Vector4 cameraBounds;

std::vector<Projectile> projectiles;
std::vector<Projectile> enemyProjectiles;
std::vector<Enemy> enemies;
// Camera 0, follows player position by default
void CameraPositionLock(Camera2D &camera, Player &player)
{
    camera.target.x = player.position.x;
    camera.target.y = player.position.y;
}

// Camera 1, follows player position until it reaches specific points
void CameraEdgeSnapping(Camera2D &camera,  Player &player, Vector4 camEdges)
{
    camera.target.x = player.position.x;
    camera.target.y = player.position.y;

    if (camera.target.x <= camEdges.x)
    {
        camera.target.x = camEdges.x;
    }
    else if(camera.target.x >= camEdges.z)
    {
        camera.target.x = camEdges.z;
    }

    if (camera.target.y <= camEdges.y)
    {
        camera.target.y = camEdges.y;
    }
    else if (camera.target.y >= camEdges.w)
    {
        camera.target.y = camEdges.w;
    }
}

// Camera 2, camera does not move until player moves on a certain distance away from a window
void CameraWindow(Camera2D &camera, Player &player, Vector4 camEdges)
{
    int cameraWidth = camEdges.z - camEdges.x;
    int cameraHeight = camEdges.w - camEdges.y;

    float leftEdge = camera.target.x - (cameraWidth/2);
    float rightEdge = camera.target.x + (cameraWidth/2);
    float topEdge = camera.target.y - (cameraHeight/2);
    float bottomEdge = camera.target.y + (cameraHeight/2);

    if (player.position.x < leftEdge)
    {
        int push = leftEdge - player.position.x;
        camera.target.x -= push;
    }
    else if (player.position.x > rightEdge - player.width)
    {
        int push = player.position.x - (rightEdge - player.width);
        camera.target.x += push;
    }

    if (player.position.y > bottomEdge - player.height)
    {
        int push = player.position.y - (bottomEdge - player.height);
        camera.target.y += push;
    }
    if (player.position.y < topEdge)
    {
        int push = topEdge - player.position.y; 
        camera.target.y -= push;
    }

    //std::cout << leftEdge << " " << rightEdge << " " << topEdge << " " << bottomEdge << std::endl;
}

void CameraPlatformSnapping(Camera2D &camera, Player &player, Vector4 camEdges, Vector2 driftFactor, bool isGrounded)
{
    float driftY = 0;

    CameraWindow(camera, player, camEdges);
    
    if (isGrounded)
    {
        driftY = clamp(player.position.y - camera.target.y, -driftFactor.y, driftFactor.y);
        camera.target.y += driftY;
    }
}

void CameraPositionSnapping(Camera2D &camera, Player &player, Vector4 camEdges, Vector2 driftFactor)
{
    float driftX, driftY = 0;

    CameraWindow(camera, player, camEdges);
    
    driftX = clamp(player.position.x - camera.target.x, -driftFactor.x, driftFactor.x);
    driftY = clamp(player.position.y - camera.target.y, -driftFactor.y, driftFactor.y);
    camera.target.x += driftX;
    camera.target.y += driftY;
}

void FireProjectile(Player &player, std::vector<Projectile> &projectiles)
{
    if (projectiles.size() <= MAX_PLAYER_PROJECTILES)
    {
        Projectile p;
        p.radius = 10.0f;
        p.uptime = 0.0f;
        p.timeToDisappear = 0.5f;
        p.isActive = true;
        p.color = YELLOW;
        p.position = player.aim;
        float velocity = 8.0f;

        if (player.orientation == 0)
        {
            p.velocity = Vector2{velocity, 0.0f};
        }
        else if (player.orientation == 1)
        {
            p.velocity = Vector2{0.0f, -velocity};
        }
        else if (player.orientation == 2)
        {
            p.velocity = Vector2{-velocity, 0.0f};
        }
        else if (player.orientation == 3)
        {
            p.velocity = Vector2{0.0f, velocity};
        }
        else
        {
            p.velocity = Vector2{0.0f, 0.0f};
        }
        projectiles.push_back(p);
        std::cout << "Shooting " << projectiles.size() << " bullets!" << std::endl;
    }
    
}

void HandleTimer()
{
    // Get current time
    float currentTime = GetTime();

    // Update elapsed time
    timerElapsed += (currentTime - GetFrameTime());

    int hours = (int)timerElapsed / 3600;
    int minutes = ((int)timerElapsed % 3600) / 60;
    int seconds = ((int)timerElapsed % 3600) % 60;

    // Format the elapsed time as a string in timer format
    
    sprintf(timerText, "%02d:%02d:%02d", hours, minutes, seconds);

}

void CheckFacingDirection(int &orientation)
{
    if (IsKeyPressed(KEY_D))
    {
        orientation = 0;
    }
    if (IsKeyPressed(KEY_W))
    {
        orientation = 1;
    }
    if (IsKeyPressed(KEY_A))
    {
        orientation = 2;
    }
    if (IsKeyPressed(KEY_S))
    {
        orientation = 3;
    }
}

void DrawAimOrientation(Player &player)
{
    if (player.orientation == 0)
    {
        Vector2 rightEdge = Vector2{player.position.x + player.width, player.position.y + (player.height/2)};
        player.aim = Vector2Add(rightEdge, Vector2{aimLength, 0});
        DrawLineEx(rightEdge, player.aim, 3, GREEN);
    }
    else if (player.orientation == 1)
    {
        Vector2 topEdge = Vector2{player.position.x + (player.width / 2), player.position.y};
        player.aim = Vector2Add(topEdge, Vector2{0, -aimLength});
        DrawLineEx(topEdge, player.aim, 3, GREEN);
    }
    else if (player.orientation == 2)
    {
        Vector2 leftEdge = Vector2{player.position.x, player.position.y + (player.height/2)};
        player.aim = Vector2Add(leftEdge, Vector2{-aimLength, 0});
        DrawLineEx(leftEdge, player.aim, 3, GREEN);
    }
    else if (player.orientation == 3)
    {
        Vector2 bottomEdge = Vector2{player.position.x + (player.width/2), player.position.y + player.height};
        player.aim = Vector2Add(bottomEdge, Vector2{0, aimLength});
        DrawLineEx(bottomEdge, player.aim, 3, GREEN);
    }
}

void Reset()
{
    std::cout << "Resetting" << std::endl;
    player.color = BLUE;
    player.velocity = Vector2{0,0};
    player.acceleration = Vector2{0,0};
    player.hitObstacle = 0;
    player.width = 24;
    player.height = 32;
    player.orientation = 0;
    enemyDeadCount = 0;
    timerElapsed = 0.0f;

    
    projectiles.clear();
    enemyProjectiles.clear();
    enemies.clear();

    // Read from Level text file
    std::ifstream inputFile("level.txt");
    if (!inputFile.is_open())
    {
        std::cout << "Error opening file." << std::endl;
    }

    // Read player position from first line
    float playerX, playerY;
    inputFile >> playerX >> playerY;


    // Instantiate player
    player.position = Vector2{playerX,playerY};

    // Close input file
    inputFile.close();

        // Read from enemy file
    std::ifstream enemyFile("enemy.txt");
    if (!enemyFile.is_open())
    {
        std::cout << "Error enemy opening file." << std::endl;
    }
    // Read number of walls from second line
    int numEnemies;
    enemyFile >> numEnemies;

    // Instantiate Enemies
    for (int i = 0; i < numEnemies; i++)
    {
        // Read wall position and dimensions
        float enemyX, enemyY;
        float enemyWidth, enemyHeight;
        float pStartX, pStartY;
        float pEndX, pEndY;
        enemyFile >> enemyX >> enemyY >> enemyWidth >> enemyHeight >> pStartX >> pStartY >> pEndX >> pEndY;

        // Instantiate wall
        Enemy enemy;
        enemy.position = {enemyX, enemyY};
        enemy.width = enemyWidth;
        enemy.height = enemyHeight;
        enemy.color = BLACK;
        enemy.start = {pStartX, pStartY};
        enemy.end = {pEndX, pEndY};

        // Add wall to walls vector
        enemies.push_back(enemy);
    }

    // Close input file
    enemyFile.close();
    
}

int main()
{
    player.color = BLUE;
    player.velocity = Vector2{0,0};
    player.acceleration = Vector2{0,0};
    player.hitObstacle = 0;
    player.width = 24;
    player.height = 32;

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
        } else if(variable == "CAM_TYPE"){
            CAM_TYPE = std::stoi(value);
        } else if(variable == "CAM_DRIFT"){
            std::istringstream iss(value);
            iss >> CAM_DRIFT.x >> CAM_DRIFT.y;
        } else if (variable == "CAM_EDGES"){
            std::istringstream iss(value);
            iss >> cameraBounds.x >> cameraBounds.y >> cameraBounds.z >> cameraBounds.w;
        }
        
    }

     // Read from Level text file
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
        wall.color = RED;

        // Add wall to walls vector
        walls.push_back(wall);
    }

    // Close input file
    inputFile.close();

    // Read from enemy file
    std::ifstream enemyFile("enemy.txt");
    if (!enemyFile.is_open())
    {
        std::cout << "Error enemy opening file." << std::endl;
        return 0;
    }
    // Read number of walls from second line
    int numEnemies;
    enemyFile >> numEnemies;

    // Instantiate Enemies
    for (int i = 0; i < numEnemies; i++)
    {
        // Read wall position and dimensions
        float enemyX, enemyY;
        float enemyWidth, enemyHeight;
        float pStartX, pStartY;
        float pEndX, pEndY;
        enemyFile >> enemyX >> enemyY >> enemyWidth >> enemyHeight >> pStartX >> pStartY >> pEndX >> pEndY;

        // Instantiate wall
        Enemy enemy;
        enemy.position = {enemyX, enemyY};
        enemy.width = enemyWidth;
        enemy.height = enemyHeight;
        enemy.color = BLACK;
        enemy.start = {pStartX, pStartY};
        enemy.end = {pEndX, pEndY};

        // Add wall to walls vector
        enemies.push_back(enemy);
    }

    // Close input file
    enemyFile.close();

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer");
    SetTargetFPS(TARGET_FPS);
    Camera2D camera = {0};
    //Vector4 cameraBounds {200, 200, 500, 500}; // {UL.x, UL.y DR.x, DR.y}, CAMERA_EDGES, requires from file, might require new computations due to absolute position on edge snapping and relative position on camera window beyond
    camera.target = {player.position.x, player.position.y};
    camera.offset = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    int cameraWidth = cameraBounds.z - cameraBounds.x;
    int cameraHeight = cameraBounds.w - cameraBounds.y;
    
    const float TIMESTEP = 1.0f / TARGET_FPS;
    float accumulator = 0.0f;

    int directionalFace = 0; // 0 = right, 1 = up, 2 = left, 3 = down
    player.orientation = 0;

    //Vector2 CAM_DRIFT{2, 2};    // {DriftX, DriftY}, CAM_DRIFT requires from file

    int toggleCamera = CAM_TYPE; // 0 = position lock; 1 = edge snap; 2 = camera window; 3 = position snap; 4 = platform snap, CAM_TYPE, requires from file

    while (!WindowShouldClose())
    {
        // Toggles on keyboard
        if (IsKeyPressed(KEY_ONE))
        {
            toggleCamera = 0;
        }
        else if (IsKeyPressed(KEY_TWO))
        {
            toggleCamera = 1;
        }
        else if (IsKeyPressed(KEY_THREE))
        {
            toggleCamera = 2;
        }
        else if (IsKeyPressed(KEY_FOUR))
        {
            toggleCamera = 3;
        }
        else if (IsKeyPressed(KEY_FIVE))
        {
            toggleCamera = 4;
        }
        //std::cout << toggleCamera << std::endl;
        // Sets the camera
        if (toggleCamera == 0)
        {
            CameraPositionLock(camera, player);
        }
        else if (toggleCamera == 1)
        {
            CameraEdgeSnapping(camera, player, cameraBounds);
        }
        else if (toggleCamera == 2)
        {
            CameraWindow(camera, player, cameraBounds);
        }
        else if (toggleCamera == 3)
        {
            CameraPositionSnapping(camera, player, cameraBounds, CAM_DRIFT);
        }
        else if (toggleCamera == 4)
        {
            CameraPlatformSnapping(camera, player, cameraBounds, CAM_DRIFT, isGrounded);
        }
        
        camera.zoom = 1.0f;

        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
        }

        if (!isGrounded)
        {
            framesNotGrounded++;
        }
        
        if (IsKeyDown(KEY_SPACE) && framesHoldingJump < V_HOLD && !isJumpKeyReleased)
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
            // driftY = clamp(player.position.y - camera.target.y, -CAM_DRIFT, CAM_DRIFT);
            // camera.target.y += driftY;
        }
        else if (framesNotGrounded >= V_SAFE && vertAccel <= GRAVITY)
        {
            vertAccel = GRAVITY;
        }
        if(IsKeyReleased(KEY_SPACE))
        {
            vertAccel = 0;
            isJumpKeyReleased = true;
        }

        if(IsKeyReleased(KEY_R))
        {
            Reset();
        }
        
        //Still Bunny hops if W is held down pls fix or not its not required
        if (IsKeyDown(KEY_D))
        {   
            float accel = H_ACCEL;
            if (isJumping)
            {
                accel = H_AIR;
            }
        
            currAccel += accel;
        }

        else if (IsKeyDown(KEY_A))
        {
            float accel = H_ACCEL;
            if (isJumping)
            {
                accel = H_AIR;
            }
        
            currAccel -= accel;
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

        CheckFacingDirection(player.orientation);

        Vector2 prevPos = {player.position.x, player.position.y};
        player.color = BLUE;

        int x = 0;
        isOnPlatform = false;
        for (auto &wall : walls)
        {
            if (player.position.y + player.height >= wall.position.y -GAP&&
                player.position.y < wall.position.y + 5.0f &&
                player.position.x + player.width > wall.position.x &&
                player.position.x < wall.position.x + wall.width &&
                (!IsKeyPressed(KEY_SPACE) && framesHoldingJump >= 0))
            {
                player.position.y = wall.position.y - player.height - GAP;
                vertAccel = 0;
                player.velocity.y = 0;
                framesHoldingJump = 0;
                isJumpKeyReleased = false;
                isGrounded = true;
                isOnPlatform = true;
                // std::cout << "TOP WALL COLL" << std::endl;
            }
            else if(
                    player.position.y  >= wall.position.y + wall.height &&
                    player.position.y -5.0f < wall.position.y + wall.height + 2 &&
                    player.position.x + player.width > wall.position.x &&
                    player.position.x < wall.position.x + wall.width
                )
            {
                player.position.y = wall.position.y + wall.height + GAP;
                isJumpKeyReleased = true;
                vertAccel = GRAVITY;
                isGrounded = false;
                // std::cout << "BOTT WALL COLL" << std::endl;
            }
            // check for collision with left side of wall
            else if (player.position.x + player.width > wall.position.x &&
                        player.position.x < wall.position.x &&
                        player.position.y + player.height > wall.position.y &&
                        player.position.y < wall.position.y + wall.height)
            {
                // player hits the left side of the wall
                player.velocity.x = 0;
                currAccel = 0;
                player.position.x = wall.position.x - player.width - GAP;
                isOnPlatform = false;
                // std::cout << "LEFT WALL COLL" << std::endl;
            }
            
            // check for collision with right side of wall
            else if (player.position.x < wall.position.x + wall.width &&
                player.position.x + player.width > wall.position.x + wall.width &&
                player.position.y + player.height > wall.position.y &&
                player.position.y < wall.position.y + wall.height)
            {
                // player hits the right side of the wall
                player.velocity.x = 0;
                currAccel = 0;
                player.position.x = wall.position.x + wall.width + GAP;
                isOnPlatform = false;
                // std::cout << "RIGHT WALL COLL" << std::endl;
            }
            x++;
        }
        if (!isOnPlatform && isGrounded)
        {
            isGrounded = false;
        }
        

        if (player.position.y + player.height >= WINDOW_HEIGHT && !IsKeyPressed(KEY_SPACE))
        {
            player.position.y = WINDOW_HEIGHT - player.height;
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
        //std::cout << "Player Position: " << player.position.x << " | " << player.position.y << std::endl;
        float deltaTime = GetFrameTime();

        accumulator += deltaTime;

        while (accumulator >= TIMESTEP)
        {
            for (int i = 0; i < projectiles.size(); i++)
            {
                Projectile &p = projectiles[i];

                if (p.isActive)
                {
                    p.uptime += TIMESTEP;
                    p.position = Vector2Add(p.position, p.velocity);
                }

                if (p.uptime >= p.timeToDisappear)
                {
                    p.isActive = false;
                    projectiles.erase(projectiles.begin() + i);
                }

                for (auto &enemy : enemies)
                {
                    if (IsCircleToRectangleColliding(p, enemy))
                    {
                        std::cout << "Hit!" << std::endl;
                        enemy.isDead = true;
                    }
                }
            }

            for (int i = 0; i < enemyProjectiles.size(); i++)
            {
                Projectile &p = enemyProjectiles[i];

                if (p.isActive)
                {
                    p.uptime += TIMESTEP;
                    p.position = Vector2Add(p.position, p.velocity);
                }

                if (p.uptime >= p.timeToDisappear)
                {
                    p.isActive = false;
                    enemyProjectiles.erase(enemyProjectiles.begin() + i);
                }

                if (IsCircleToRectangleColliding(p, player))
                {
                    std::cout << "Hit! Player" << std::endl;
                    Reset();
                }
            }
            accumulator -= TIMESTEP;
        }

        //Enemy Movements
        enemyDeadCount = 0;
        for (auto &enemy : enemies)
        {
            if (!enemy.isDead)
            {
                            
                float level = abs(player.position.y - enemy.position.y);
                if (Vector2Distance(player.position ,enemy.position) < 200 && level < 30 && !enemy.isStopped )
                {
                    enemy.StopStart();
                }
                else if (Vector2Distance(player.position ,enemy.position) >= 200 &&  enemy.isStopped)
                {
                    enemy.StopStart();
                }

                if (!enemy.isStopped)
                {         
                    enemy.Patrol();
                }
                else
                {
                    enemy.acceleration.x = 0;
                    enemy.velocity.x = 0;

                    enemy.currentTime = GetTime();
                    enemy.elapsedTime = enemy.currentTime - enemy.previousTime;

                    // Check if the desired time interval has elapsed
                    if (enemy.elapsedTime >= enemy.interval)
                    {
                        // Call the function to run every second
                        enemy.Shoot(player, enemyProjectiles);

                        // Update previous time to current time
                        enemy.previousTime = enemy.currentTime;
                    }
                }

                // Check enemy collision
                if (player.position.y + player.height >= enemy.position.y -GAP&&
                    player.position.y < enemy.position.y + 5.0f &&
                    player.position.x + player.width > enemy.position.x &&
                    player.position.x < enemy.position.x + enemy.width &&
                    (!IsKeyPressed(KEY_SPACE) && framesHoldingJump >= 0))
                {

                    Reset();
                }
                else if(
                        player.position.y  >= enemy.position.y + enemy.height &&
                        player.position.y -5.0f < enemy.position.y + enemy.height + 2 &&
                        player.position.x + player.width > enemy.position.x &&
                        player.position.x < enemy.position.x + enemy.width
                    )
                {

                    Reset();
                }
                // check for collision with left side of wall
                else if (player.position.x + player.width > enemy.position.x &&
                            player.position.x < enemy.position.x &&
                            player.position.y + player.height > enemy.position.y &&
                            player.position.y < enemy.position.y + enemy.height)
                {
                    Reset();
                }
                

                else if (player.position.x < enemy.position.x + enemy.width &&
                    player.position.x + player.width > enemy.position.x + enemy.width &&
                    player.position.y + player.height > enemy.position.y &&
                    player.position.y < enemy.position.y + enemy.height)
                {
                    Reset();
                }
            }
            else
            {
                enemyDeadCount++;
                if (enemyDeadCount >= enemies.size())
                {
                    Reset();
                }
                
            }
        }

        HandleTimer();
        BeginDrawing();
            ClearBackground(WHITE);
            BeginMode2D(camera);

            DrawRectangle(player.position.x, player.position.y, player.width, player.height, player.color);
            DrawAimOrientation(player);
            
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                FireProjectile(player, projectiles);
            }

            for (Projectile &p : projectiles)
            {
                if (p.isActive)
                {
                    // std::cout << "Drawing projectile!" << std::endl;
                    DrawCircleV(p.position, p.radius, p.color);
                }
            }

            for (Projectile &p : enemyProjectiles)
            {
                if (p.isActive)
                {
                    // std::cout << "Drawing projectile!" << std::endl;
                    DrawCircleV(p.position, p.radius, p.color);
                }
            }

            for (auto &wall : walls)
            {
                DrawRectangle(wall.position.x, wall.position.y, wall.width, wall.height, wall.color);
            }

            for (auto &enemy : enemies)
            {
                if (!enemy.isDead)
                {
                    DrawRectangle(enemy.position.x, enemy.position.y, enemy.width, enemy.height, enemy.color);
                }
            }

            DrawText("Kill All Enemies", camera.target.x - 175 , camera.target.y+200, 50, BLUE);
            DrawText(timerText, camera.target.x - 100 , camera.target.y+150, 50, BLUE);
            
            EndMode2D();
        EndDrawing();

    }

    CloseWindow();
    return 0;
}