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

Image traceOutline(Image& image, int outlineWidth) {
    // Sobel operation edge detection 
    // operator uses a 3x3 convolved kernal with the original image
    Image blurImg = ImageCopy(image);
    ImageBlurGaussian(&blurImg, outlineWidth);
    ExportImage(blurImg, "C:\\Users\\alla0\\Desktop\\Raylib_blur.png");

    const float sharpnessCoef = 1.f;
    Image dilateImg = ImageCopy(blurImg);
    // map pixels
    Color pixel;
    std::vector<unsigned char>pixels;
    pixels.reserve(blurImg.width * blurImg.height * blurImg.format);

    // increment by pixel size also known as image format.
    for (int i = 0; i < blurImg.width * blurImg.height * blurImg.format; i+=3) {
        pixel.r = static_cast<unsigned char*>(blurImg.data)[i];
        pixel.g = static_cast<unsigned char*>(blurImg.data)[i+1];
        pixel.b = static_cast<unsigned char*>(blurImg.data)[i+2];
        
        if ((pixel.r + pixel.g + pixel.b) / 3 / 255 > sharpnessCoef) // brightness is the average of the rgb values
            pixel = WHITE;
        else {
            pixel.r *= 1 / sharpnessCoef;
            pixel.g *= 1 / sharpnessCoef;
            pixel.b *= 1 / sharpnessCoef;
        }

        pixels[i] = pixel.r;
        pixels[i+1] = pixel.g;
        pixels[i+2] = pixel.b;
    }
    dilateImg.data = pixels.data();
    ExportImage(dilateImg, "C:\\Users\\alla0\\Desktop\\Raylib_dilate.png");
    
    Image smoothResult = ImageCopy(image);
    for (int i = 0; i < blurImg.width * blurImg.height * blurImg.format; i += 3) {
        Color sPixel; 
        sPixel.r = static_cast<unsigned char*>(image.data)[i]; sPixel.g = static_cast<unsigned char*>(image.data)[i+1];;
        sPixel.b = static_cast<unsigned char*>(image.data)[i + 2]; 
        Color dPixel;
        dPixel.r = static_cast<unsigned char*>(dilateImg.data)[i]; dPixel.g = static_cast<unsigned char*>(dilateImg.data)[i + 1];;
        dPixel.b = static_cast<unsigned char*>(dilateImg.data)[i + 2]; 
        
        pixels[i] = dPixel.r - sPixel.r;
        pixels[i + 1] = dPixel.g - sPixel.g;
        pixels[i + 2] = dPixel.b - sPixel.b;
    }
    smoothResult.data = pixels.data();
    ExportImage(smoothResult, "C:\\Users\\alla0\\Desktop\\Raylib_smooth.png");
    //UnloadImage(blurImg);
    //UnloadImage(dilateImg);
    return blurImg;
}

void drawLetter(Fluid &MyFluid, const char* text) {
    Font font = LoadFontEx("C:\\Windows\\Fonts\\arial.ttf", 32*8, NULL, 0); // %WINDDIR% doesnt work
    Image glyph = ImageTextEx(font, text, 32*8, 1.f, WHITE);
    ImageFormat(&glyph, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    ExportImage(glyph, "C:\\Users\\alla0\\Desktop\\Raylib_glyph.png");
    
    std::vector<unsigned char> outline; // idk how much are there so it's a vector
    outline.reserve(glyph.height*glyph.width*glyph.format);
    // search all neighbor pixels for a non-empty pixel
    glyph = traceOutline(glyph, 1);
    // end
    int x = screenWidth / 2;
    int y = screenHeight / 2;
    
    for (const auto& position : outline) {
        int i = position / glyph.width;
        int j = position % glyph.width;
        MyFluid.addDensity((x / SCALE) + i, (y/SCALE) + j, 255);
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
    Timer delay;
    // create ring of fluid
    int radius = 50;
    float angle = 0;
    printf("DRAWLETTER\n");
    // draw letter
    drawLetter(MyFluid, "Ami");
    

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
            //MyFluid.addDensity((int)scaledMouse.x, (int)scaledMouse.y, 500);
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

                //doExplosion(MyFluid, x, y);
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

