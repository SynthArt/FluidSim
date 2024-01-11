// FluidSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "raylib.h"
#include "Fluid.h"
#include "Timer.h"
#include <iostream>
#include <vector>
#include <random>
#define PI 3.142857
const int screenWidth = N * SCALE;
const int screenHeight = N * SCALE;

void doExplosion(Fluid &MyFluid, int x, int y) {
    float explodeX = 6, explodeY = 6;
    std::mt19937_64 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(0.0f, 2.0f * PI);
    float direction = dist(rng);
    MyFluid.addVelocity(x, y, explodeX * cos(direction), explodeY * sin(direction));
}

void drawLetter(Fluid &MyFluid, std::vector<std::vector<bool>> &letter, int position) {
    int spacing = position * (5 + 1); // 5 is the total width of a letter; add that by the amnt of space needed
    int x = screenWidth / SCALE / 2;
    int y = screenHeight / SCALE / 2;
    for (int i = 1; i <= letter.size(); i++) {
        for (int j = 1; j <= letter[0].size(); j++) {
            MyFluid.addDensity( (x-18) + j + spacing, y + i, 255 * letter[i - 1][j - 1]);
        }
    }
}

int main()
{
    
    InitWindow(screenWidth,screenHeight, "My Fluid Sim :)");
    RenderTexture2D Canvas = LoadRenderTexture(screenWidth, screenHeight);
    std::vector<std::vector<bool>> A = {
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1}
    };
    std::vector<std::vector<bool>> M = {
        {1, 0, 0, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1}
    };

    std::vector<std::vector<bool>> I = {
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0}
    };

    std::vector<std::vector<bool>> F = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0}
    }; 
    std::vector<std::vector<bool>> U = {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    };

    std::vector<std::vector<bool>> C = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1}
    };

    std::vector<std::vector<bool>> K = {
        {1, 0, 0, 1, 0},
        {1, 0, 1, 0, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 0, 0, 1, 0}
    };

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
    drawLetter(MyFluid, F, 0);
    drawLetter(MyFluid, U, 1);
    drawLetter(MyFluid, C, 2);
    drawLetter(MyFluid, K, 3);
    drawLetter(MyFluid, U, 5);
    /*
    for (int i = 0; i <= PI*radius*2; i++) {

        int x = (radius * cos(i * angle) + screenWidth / 2) / SCALE;
        int y = (radius * sin(i * angle) + screenHeight / 2) / SCALE;
        MyFluid.addDensity(x, y, 255);
        
        //doExplosion(MyFluid, x, y);
        angle+=0.01f;
    }
    */
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
            for (int i = 0; i <= PI * radius * 2; i++) {

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

