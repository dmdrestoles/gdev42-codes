/**
 * Authors:
 * Class: GDEV 42 F
 * Date: January 30, 2023
 
 Code description
**/

#include <raylib.h>

#include <iostream>
#include <cmath>
#include <vector>

// Constants
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

float GetDistanceVector2(Vector2 point1, Vector2 point2)
{
    float result;

    result = pow(point2.x - point1.x,2) + pow(point2.y - point1.y,2);
    result = sqrt(result);

    return result;
}
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
    int steps;
    int controlPoints;
    int radius = 5;
    int lineWidth = 2;

    int indexPointMoved = -1;

    std::vector<Vector2> points;

    std::cin >> steps;
    if (steps < 0)
    {
        std::cout << "Invalid number of steps, please try again." << std::endl;
        std::cin >> steps;
    }

    std::cin >> controlPoints;
    if (controlPoints < 3)
    {
        std::cout << "Control points lacking, please try again." << std::endl;
        std::cin >> controlPoints;
    }

    for (int i = 0; i < controlPoints; i++)
    {
        Vector2 coords;
        
        std::cin >> coords.x >> coords.y;

        points.push_back(coords);
    }
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bezier Curves");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
        }

        // Only selected point is moved
        if (indexPointMoved > -1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            points[indexPointMoved] = GetMousePosition();
        } 
        // if mouse button released indexPoint is released
        else if (IsMouseButtonReleased && indexPointMoved != -1)
        {
            indexPointMoved = -1;
        }
        
        // Select the point index if no point is being moved
        if (indexPointMoved < 0)
        {
           for (int i = 0; i < points.size(); i++)
            {
                if (GetDistanceVector2(points[i],GetMousePosition()) <= 20 &&
                    IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
                {
                    indexPointMoved = i;               
                }
            }
        }
           
        BeginDrawing();
            ClearBackground(BLACK);
            int j = 0;
            std::vector<Vector2> toCurve;
            std::vector<Vector2> bezierPoints;

            for (Vector2 &pt : points)
            {
                // Draw the points, alternating green or red.
                if (j % 2 == 0)
                {
                    DrawCircleV(pt, radius, GREEN);
                }
                else
                {
                    DrawCircleV(pt, radius, RED);
                }
                
                // Once there are three points, perform a Bezier curve on those three points.
                toCurve.push_back(pt);
                if (toCurve.size() == 3)
                {
                    bezierPoints = GenerateBezierCurvePoints(toCurve[0], toCurve[1], toCurve[2], steps);
                    Vector2 prevPoint = bezierPoints[0];
                    for (Vector2 &point : bezierPoints)
                    {
                        // Just for check to skip the first point from connecting to itself
                        if (&prevPoint == &point)
                        {
                            continue;
                        }
                        Vector2 newPoint = point;
                        DrawLineEx(newPoint, prevPoint, lineWidth, WHITE);
                        prevPoint = point;
                    }

                    
                    // Remove the first two points from the list, retain the last to make it the first point for Bezier curve of the next set of points.
                    toCurve.erase(toCurve.begin());
                    toCurve.erase(toCurve.begin());
                    bezierPoints.clear();
                }
                j += 1;
            }
        EndDrawing();
    }
    return 0;
}