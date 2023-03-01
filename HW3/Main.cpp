/**
 * Authors: Catherine Golles, Jared John Javillo, Dan Mark Restoles
 * Class: GDEV 42 F
 * Date: March 1, 2023
 
 This code asks for a series of inputs and displays a parametric Bezier curve, its tangents, and its normals based on given input.
**/

#include <raylib.h>
#include <raymath.h>
#include "rlgl.h"

#include <iostream>
#include <cmath>
#include <vector>

// Constants
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

struct Ball
{
    Vector2 center;
    float radius;
    Color color;
    float thresh;

    void Draw()
    {
        DrawCircleV(center, radius, color);
    }
};

float RandomFloat(float min, float max)
{
    float r = (float)rand() / (float)RAND_MAX;
    return min + r * (max - min);
}

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

    for (auto &p : pascalRows.back())
    {
        std::cout << p << std::endl;
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
            Vector2 cpt{0, 0};
            cpt = Vector2Scale(controlPoints[j], coefs[j]);
            cpt = Vector2Scale(cpt, pow(1 - (i * scaled), coefs.size() - j - 1));
            cpt = Vector2Scale(cpt, pow(i * scaled, j));

            pt = Vector2Add(pt, cpt);
        }
        (*curvePoints).push_back(pt);
    }
}
void GenerateBezierCurveTangents(std::vector<Vector2>* tangentPoints, std::vector<Vector2> controlPoints, std::vector<int> coefs, int steps, int order)
{
    float scaled = 1.0f / steps;

    for (int i = 0; i < steps; i++)
    {
        Vector2 pt{0, 0};
        
        for (int j = 0; j < order; j++)
        {
            Vector2 cpt{0, 0};

            cpt = Vector2Subtract(controlPoints[j + 1], controlPoints[j]);
            cpt = Vector2Scale(cpt, pow(1 - (i * scaled), order - j - 1));
            cpt = Vector2Scale(cpt, pow(i * scaled, j));
            cpt = Vector2Scale(cpt, coefs[j]);
            cpt = Vector2Scale(cpt, order);

            pt = Vector2Add(pt, cpt);

        }
        Vector2 normalizedPt = Vector2Normalize(pt);
        (*tangentPoints).push_back(normalizedPt);
    }
}

void Lerp(std::vector<Vector2>* normalPoints, Vector2 start, Vector2 end, int steps)
{
    float scale = 1.0f / steps;

    for (int i = 1; i < steps; i++)
    {
        Vector2 sub{ end.x - start.x, end.y - start.y };
        Vector2 scaled{ sub.x * i * scale, sub.y * i * scale };
        Vector2 translated{ scaled.x + start.x, scaled.y + start.y };

        (*normalPoints).push_back(translated);
    }
}

int main()
{
    int order;
    int steps, tangentSteps;
    int controlPoints;
    int radius = 5;
    int lineWidth = 2;

    int indexPointMoved = -1;
    int counter = 0;
    bool isDrawingBall = false;
    bool isFlyingOut = false;

    Ball redBall;
    redBall.color = RED;
    redBall.radius = radius * 2;

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

    std::cin >> tangentSteps;
    if (tangentSteps < 0)
    {
        std::cout << "Invalid number of steps, please try again." << std::endl;
        std::cin >> tangentSteps;
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
    std::vector<int> dCoefs = GeneratePascal(order);
    std::cout << "Coefficients: " << coefs.size() << std::endl;
    std::vector<Vector2> toCurve;
    std::vector<Vector2> curvePoints;
    std::vector<Vector2> tangentPoints;
    std::vector<Vector2> normalPoints;
    bool turning = false;
    bool onNormal = false;
    
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
            curvePoints.push_back(points.front());
            for (int i = 0; i < points.size(); i++)
            {
                DrawCircleV(points[i], radius, BLACK);
                toCurve.push_back(points[i]);

                if (toCurve.size() - 1 == order)
                {
                    GenerateBezierCurvePoints(&curvePoints, toCurve, coefs, steps);
                    GenerateBezierCurveTangents(&tangentPoints, toCurve, dCoefs, tangentSteps, order);

                    for (int j = order; j > 0; j--)
                    {
                        toCurve.erase(toCurve.begin());
                    }
                }
            }
            curvePoints.push_back(points.back());
            
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

            for (int i = 1; i < tangentSteps; i++)
            {
                Vector2 pointToAttach = curvePoints[i * (curvePoints.size() / tangentSteps)];
                Vector2 tanPoint = Vector2Add(Vector2Scale(tangentPoints[i], 50), pointToAttach);
                DrawLineEx(pointToAttach, tanPoint, lineWidth, YELLOW);
                Vector2 normalPt{pointToAttach.x - (tanPoint.y - pointToAttach.y), pointToAttach.y + (tanPoint.x - pointToAttach.x)};
                DrawLineEx(pointToAttach, normalPt,lineWidth,BLUE);
            }

            if (IsKeyDown(KEY_SPACE))
            {
                isDrawingBall = true;
                counter = 0;
            }
            if (counter >= curvePoints.size() - 1)
            {
                isDrawingBall = false;
                counter = 0;
            }
            else if (counter >= curvePoints.size() - 2 && turning == true) {
                isDrawingBall = false;
                counter = 0;
                turning = false;
                onNormal = false;
            }

            if (isDrawingBall)
            {
                if (!turning) {
                    redBall.center = curvePoints[counter];
                }
                else if (turning && !onNormal){
                    counter = 0;
                    onNormal = true;
                }

                for (int i = 1; i < tangentSteps; i++)
                {
                    Vector2 pointToAttach = curvePoints[i * (curvePoints.size() / tangentSteps)];
                    float normChance = RandomFloat(0.0f, 1.0f);
                    if (normChance <= .2f && redBall.center.x == pointToAttach.x && redBall.center.y == pointToAttach.y && !turning) {
                        turning = true;
                        Vector2 tanPoint = Vector2Add(Vector2Scale(tangentPoints[i], 50), pointToAttach);
                        Vector2 normalPt{ pointToAttach.x - (tanPoint.y - pointToAttach.y), pointToAttach.y + (tanPoint.x - pointToAttach.x) };
                        Lerp(&normalPoints, pointToAttach, normalPt, steps);
                        break;
                    }
                }

                if (onNormal) {
                    redBall.center = normalPoints[counter];
                }
                
                redBall.Draw();
                counter += 1;                
            }

            toCurve.clear();
            curvePoints.clear();
            tangentPoints.clear();
            if (!turning){
                normalPoints.clear();
            }
        EndDrawing();
    }
    return 0;
}