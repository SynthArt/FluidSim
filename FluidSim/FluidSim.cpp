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
    SetTargetFPS(160);
    Fluid MyFluid(0.025f,2.0f,0);

    Vector2 pmouse = GetMousePosition();
    Vector2 mouse = GetMousePosition();

    while(!WindowShouldClose()) {
        pmouse = mouse;
        mouse = GetMousePosition();
        printf("FPS: %i \n", GetFPS());
        
        MyFluid.step();
        MyFluid.fadeD();

        BeginTextureMode(Canvas);
            MyFluid.renderDensity();
            //MyFluid.renderVelocity();
        EndTextureMode();
        
        BeginDrawing();
        Rectangle source = { 0, 0, (float)Canvas.texture.width, (float)-Canvas.texture.height };
        DrawTextureRec(Canvas.texture, source, { 0,0 }, WHITE);
        EndDrawing();
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            MyFluid.addDensity(mouse.x / SCALE, mouse.y / SCALE, 500);
            float amntX = mouse.x - pmouse.x;
            float amntY = mouse.y - pmouse.y;
            MyFluid.addVelocity(mouse.x / SCALE, mouse.y / SCALE, amntX, amntY);
        }
    }
    CloseWindow();
    return 0;
}

