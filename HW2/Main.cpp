/**
 * Authors:
 * Class: GDEV 42 F
 * Date: February 20, 2023
 
 This code asks for a series of inputs and displays a parametric Bezier curve based on given input.
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

// Generate Pascal by generating rows and getting the sum of entries from previous rows (breaks at 35th row)
std::vector<int> GeneratePascal(int rows)
{
    std::vector<std::vector<int>> pascalRows;
    std::vector<int> v1, v2;
    std::vector<int> prevRow;

    v1.push_back(1);
    v2.push_back(1);
    v2.push_back(1);

    pascalRows.push_back(v1);
    pascalRows.push_back(v2);
    prevRow = v2;

    for (int i = 1; i < rows - 1; i++)
    {
        std::vector<int> newRow;
        newRow.push_back(1);
        for (int j = 0; j < i; j++)
        {
            newRow.push_back(prevRow[j] + prevRow[j+1]);
        }
        newRow.push_back(1);

        pascalRows.push_back(newRow);
        prevRow = newRow;
    }

    return pascalRows.back();
}

void GenerateBezierCurvePoints(std::vector<Vector2>* curvePoints, std::vector<Vector2> controlPoints, std::vector<int> coefs, int steps)
{
    float scaled = 1.0f / steps;

    for (int i = 0; i < steps; i++)
    {
        Vector2 pt{0, 0};
        
        for (int j = 0; j < coefs.size(); j++)
        {
            pt.x += coefs[j] * controlPoints[j].x * pow(1 - (i * scaled), coefs.size() - j - 1) * pow(i * scaled, j);
            pt.y += coefs[j] * controlPoints[j].y * pow(1 - (i * scaled), coefs.size() - j - 1) * pow(i * scaled, j);
        }
        (*curvePoints).push_back(pt);
    }
}

int main()
{
    int order;
    int steps;
    int controlPoints;
    int radius = 5;
    int lineWidth = 2;

    int indexPointMoved = -1;

    std::vector<Vector2> points;

    std::cin >> order;
    if (order <= 0)
    {
        std::cout << "Invalid order, please try again." << std::endl;
        std::cin >> order;
    }

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

    std::vector<int> coefs = GeneratePascal(order+1);
    std::cout << "Coefficients: " << coefs.size() << std::endl;
    std::vector<Vector2> toCurve;
    std::vector<Vector2> curvePoints;
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Parametric Bezier Curves");
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
            ClearBackground(WHITE);

            for (int i = 0; i < points.size(); i++)
            {
                DrawCircleV(points[i], radius, BLACK);
                toCurve.push_back(points[i]);

                if (toCurve.size() - 1 == order)
                {
                    GenerateBezierCurvePoints(&curvePoints, toCurve, coefs, steps);

                    for (int j = order; j > 0; j--)
                    {
                        toCurve.erase(toCurve.begin());
                    }
                }
            }
        
            Vector2 prevPoint = curvePoints[0];

            for (Vector2 &point : curvePoints)
            {
                // Just for check to skip the first point from connecting to itself
                if (&prevPoint == &point)
                {
                    continue;
                }
                Vector2 newPoint = point;
                DrawLineEx(newPoint, prevPoint, lineWidth, BLACK);
                prevPoint = point;
            }
            toCurve.clear();
            curvePoints.clear();
        EndDrawing();
    }
    return 0;
}