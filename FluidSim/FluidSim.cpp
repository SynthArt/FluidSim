// FluidSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "raylib.h"
#include "Fluid.h" 
#include <iostream>

void doExplosion(Fluid &MyFluid, int x, int y) {
    float explodeX = 72 * GetRandomValue(-1, 1), explodeY = 72 * GetRandomValue(-1,1);
    MyFluid.addVelocity(x,y,explodeX,explodeY);
}

int main()
{
    const int screenWidth = N*SCALE;
    const int screenHeight = N*SCALE;
    InitWindow(screenWidth,screenHeight, "My Fluid Sim :)");
    RenderTexture2D Canvas = LoadRenderTexture(screenWidth, screenHeight);
    SetTargetFPS(60);
    //0.03125f - 3.5f

    Fluid MyFluid(0.03125f,3.5f,0);

    Vector2 pmouse = GetMousePosition();
    Vector2 mouse = GetMousePosition();
    Vector2 scaledMouse = {mouse.x / SCALE, mouse.y / SCALE};

    // create line of fluid
    for (int i = 1; i <= screenWidth/SCALE-2; i++) {
        MyFluid.addDensity( i, screenHeight / SCALE / 2, 1000);
        doExplosion(MyFluid, i, screenHeight / SCALE / 2);
    }
    
    
    while(!WindowShouldClose()) {
        
        pmouse = mouse;
        mouse = GetMousePosition();
        scaledMouse = { mouse.x / SCALE, mouse.y / SCALE };
        printf("FPS: %i \n", GetFPS());
        
        MyFluid.step();
        MyFluid.updateDeltaTime(GetFrameTime());
        //MyFluid.fadeD();

        BeginTextureMode(Canvas);
            MyFluid.render();
            //MyFluid.renderVelocity();
        EndTextureMode();
        
        BeginDrawing();
        Rectangle source = { 0, 0, (float)Canvas.texture.width, (float)-Canvas.texture.height };
        DrawTextureRec(Canvas.texture, source, { 0,0 }, WHITE);
        EndDrawing();
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            MyFluid.addDensity((int)scaledMouse.x, (int)scaledMouse.y, 500);
            float amntX = mouse.x - pmouse.x;
            float amntY = mouse.y - pmouse.y;
            MyFluid.addVelocity((int)scaledMouse.x, (int)scaledMouse.y, amntX, amntY);
            if (IsKeyDown(KEY_SPACE)) {
                MyFluid.addWall((int)scaledMouse.x,(int)scaledMouse.y);
            }
        }
    }
    CloseWindow();
    return 0;
}

