// FluidSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "raylib.h"
#include "Fluid.h" 
#include <iostream>
int main()
{
    const int screenWidth = N*SCALE;
    const int screenHeight = N*SCALE;
    InitWindow(screenWidth,screenHeight, "My Fluid Sim :)");
    RenderTexture2D Canvas = LoadRenderTexture(screenWidth, screenHeight);
    SetTargetFPS(60);
    Fluid MyFluid(0.025f,2,0);

    Vector2 pmouse = GetMousePosition();
    Vector2 mouse = GetMousePosition();

    while(!WindowShouldClose()) {
        pmouse = mouse;
        mouse = GetMousePosition();
        MyFluid.step();
        
        BeginTextureMode(Canvas);
            MyFluid.renderDensity();
            MyFluid.renderVelocity();
        EndTextureMode();
        
        BeginDrawing();
            DrawTexture(Canvas.texture, 0, 0, WHITE);
        EndDrawing();

        /*
        int rectX = (screenWidth / 2) / SCALE;
        int rectY = (screenHeight - (screenHeight / 2)) / SCALE;
        MyFluid.addDensity(rectX, rectY, 255);
        */

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            MyFluid.addDensity(mouse.x / SCALE, (mouse.y) / SCALE, 15);
            float amntX = mouse.x - pmouse.x;
            float amntY = (screenHeight-mouse.y) - (screenHeight-pmouse.y);
            MyFluid.addVelocity(mouse.x / SCALE, (screenHeight-mouse.y) / SCALE, amntX, amntY);
        }
    }
    CloseWindow();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
