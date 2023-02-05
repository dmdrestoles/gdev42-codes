/**
 * Authors:
 * Class: GDEV 42 F
 * Date: January 30, 2023
 
 Code description
**/

#include <raylib.h>

#include <iostream>
#include <vector>

// Constants
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

Vector2 Lerp(Vector2 start, Vector2 end, float scale)
{
    Vector2 sub{end.x - start.x, end.y - start.y};
    Vector2 scaled{sub.x * scale, sub.y * scale};
    Vector2 translated{scaled.x + start.x, scaled.y + start.y};

    return translated;
}

std::vector<Vector2> GenerateBezierCurvePoints(Vector2 start, Vector2 mid, Vector2 end, int steps)
{
    std::vector<Vector2> points;
    
    float scaled = 1.0f / steps;
    
    for (int i = 1; i < steps; i++)
    {
        Vector2 stepPointOne = Lerp(start, mid, i * scaled);
        Vector2 stepPointTwo = Lerp(mid, end, i * scaled);
        Vector2 stepPointThree = Lerp(stepPointOne, stepPointTwo, i*scaled);

        points.push_back(stepPointThree);
    }

    return points;
}

int main()
{
    int radius = 8;
    int lineWidth = 3;
    Vector2 p1;
    Vector2 p2;
    Vector2 p3;
    int steps;

    std::cin >> steps >> p1.x >> p1.y >> p2.x >> p2.y >> p3.x >> p3.y;

    std::vector<Vector2> points = GenerateBezierCurvePoints(p1, p2, p3, steps);

    for (Vector2 &point : points)
    {
        std::cout << point.x << " " << point.y << std::endl;
    }
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Linear Interpolation");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
        }
        BeginDrawing();
            ClearBackground(WHITE);
            DrawCircleV(p1, radius, BLACK);
            Vector2 prevPoint = p1;
            int i = 0;
            for (Vector2 &point : points)
            {
                Vector2 newPoint = point;
                switch (i)
                {
                    case 0:
                        DrawCircleV(point, radius, RED);
                        break;
                    case 1:
                        DrawCircleV(point, radius, GREEN);
                        break;
                    case 2:
                        DrawCircleV(point, radius, BLUE);
                        break;
                    default:
                        DrawCircleV(point, radius, BLACK);
                }
                DrawLineEx(newPoint, prevPoint, lineWidth, BLACK);
                prevPoint = point;
                i += 1;
            }
            DrawCircleV(p3, radius, BLACK);
            DrawLineEx(p3, prevPoint, lineWidth, BLACK);
        EndDrawing();
    }

    // CloseWindow();
    return 0;


}