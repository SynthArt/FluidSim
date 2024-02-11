// FluidSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "raylib.h"
#include "Fluid.h"
#include "Timer.h"
#include <iostream>
#include <vector>
#include <random>
#include <map>
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


void drawLetter(Fluid &MyFluid, const char* text) {
    Font font = LoadFontEx("C:\\Windows\\Fonts\\Arialic Hollow.ttf", 48, NULL, 0); // change this to %WINDDIR%
    Image glyph = ImageTextEx(font, text, 48, 1.f, WHITE);
    ImageFormat(&glyph, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    
    /*
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "USERPROFILE") == 0 && buf != nullptr ) {
        printf("EnvName = %s\n", buf);
        
        std::string path = "\\Desktop\\Raylib_glyph.png";
        path.insert(0,buf);
        ExportImage(glyph, path.c_str());
        free(buf);
    }
    */
    int x = screenWidth / 2 - (glyph.width*3);
    int y = screenHeight / 2 - 100;
    unsigned char* outlineData = static_cast<unsigned char*>(glyph.data);
    std::map<int, Color> outlineFluid;
    for (int i = 0; i < glyph.width * glyph.height * glyph.format; i += 3) {
        Color c = { outlineData[i], outlineData[i + 1], outlineData[i + 2], 255};
        outlineFluid[i/3] = c;
    }

    for (auto& i : outlineFluid) {
        Color c = i.second;
        int brightness = (c.r + c.g + c.b) / 3.f / 255.0f; // Convert RGB to grayscale
        if (brightness == 1 && c.a == 255) {
            int fluidX = (x / SCALE) + (i.first % glyph.width); // Calculate fluid X position
            int fluidY = (y / SCALE) + (i.first / glyph.width); // Calculate fluid Y position
            MyFluid.addDensity(fluidX, fluidY, 255);
        }
    }
    UnloadImage(glyph);
    UnloadFont(font);
}

int main()
{
    
    InitWindow(screenWidth,screenHeight, "My Fluid Sim :)");
    RenderTexture2D Canvas = LoadRenderTexture(screenWidth, screenHeight);
    SetTargetFPS(60);
    //0.03125f - 3.5f
    Fluid MyFluid(0.03125f,3.5f,0);
    
    Vector2 pmouse = GetMousePosition();
    Vector2 mouse = GetMousePosition();
    Vector2 scaledMouse = {mouse.x / SCALE, mouse.y / SCALE};
    
    // create timer and a timer that handles all other timers
    Timer delayAnim;
    Timer delayDiff;
    // create ring of fluid
    int radius = 150;
    float angle = 0;
    printf("DRAWLETTER\n");
    // draw letter
    drawLetter(MyFluid, "Ami:3");
    
    
    Timer::startTimer(&delayAnim, 2.5f);
    Timer::startTimer(&delayDiff, 1.f);

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
            //MyFluid.addDensity((int)scaledMouse.x, (int)scaledMouse.y, 500);
            float amntX = mouse.x - pmouse.x;
            float amntY = mouse.y - pmouse.y;
            MyFluid.addVelocity((int)scaledMouse.x, (int)scaledMouse.y, amntX, amntY);
            if (IsKeyDown(KEY_SPACE)) {
                MyFluid.addWall((int)scaledMouse.x,(int)scaledMouse.y);
            }
        }

        if (Timer::timerDone(&delayAnim)) {
            // expolode animation
            angle = 0;
            for (int i = 0; i <= PI * radius * 2; i++) {

                int x = (radius * cos(i * angle) + screenWidth / 2) / SCALE;
                int y = (radius * sin(i * angle) + screenHeight / 2) / SCALE;

                doExplosion(MyFluid, x, y);
                angle += 0.01f;
            }
            Timer::endTimer(&delayAnim);
        }
        else
            Timer::updateTimer(&delayAnim);
    }
    CloseWindow();
    return 0;
}

