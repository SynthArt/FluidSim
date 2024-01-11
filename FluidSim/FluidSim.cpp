// FluidSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "raylib.h"
#include "Fluid.h"
#include "Timer.h"
#include <iostream>
#include <random>
#define PI 3.142857

void doExplosion(Fluid &MyFluid, int x, int y) {
    float explodeX = (float)GetRandomValue(70,80), explodeY = (float)GetRandomValue(70, 80);
    std::mt19937_64 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(0.0f, 2.0f * PI);
    float direction = dist(rng);
    MyFluid.addVelocity(x, y, explodeX * cos(direction), explodeY * sin(direction));
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
    
    // create timer and a timer that handles all other timers
    Timer delay;
    // create ring of fluid
    int radius = 50;
    float angle = 0;
    for (int i = 0; i <= radius*2; i++) {

        int x = (radius * cos(i * angle) + screenWidth / 2) / SCALE;
        int y = (radius * sin(i * angle) + screenHeight / 2) / SCALE;
        MyFluid.addDensity(x, y, 500);
        
        //doExplosion(MyFluid, x, y);
        angle+=0.01f;
    }
    Timer::startTimer(&delay, 2.f);

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

        if (Timer::timerDone(&delay)) {
            // expolode animation
            angle = 0;
            for (int i = 0; i <= radius * 2; i++) {

                int x = (radius * cos(i * angle) + screenWidth / 2) / SCALE;
                int y = (radius * sin(i * angle) + screenHeight / 2) / SCALE;

                doExplosion(MyFluid, x, y);
                angle += 0.01f;
            }
            Timer::endTimer(&delay);
        }
        else
            Timer::updateTimer(&delay);
    }
    CloseWindow();
    return 0;
}

