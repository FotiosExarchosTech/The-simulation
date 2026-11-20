#pragma once

#include <string>
#include "raylib.h"
#include <rlgl.h>

#include "creator_movement.h"

class Simulation
{
private:
    
    Creator_Movement creator_movement;
    Camera3D camera;

    int window_width = 1280;
    int window_height = 720;
    std::string window_title;
    float desired_fps = 60.0f;
    bool vsync_enabled = true;

    float camera_field_of_view_degrees = 60.0f;
    
    void Update();
    void Render();
    
public:
    Simulation( const char* window_title,int window_width = 1280, int window_height =780);
    ~Simulation();

    void Run();    
};