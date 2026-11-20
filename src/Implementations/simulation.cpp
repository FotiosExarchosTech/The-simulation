#include "../Headers/simulation.hpp"


Simulation::Simulation( const char* window_title, int window_width, int window_height)
{
    this->window_width = window_width; 
    this->window_height = window_height;
    this->window_title = window_title;

    InitWindow(this->window_width, this->window_height, window_title);
    SetTargetFPS(this->desired_fps);
    
    this->creator_movement = Creator_Movement({ 0.0f, 3.0f, 3.0f }, 0.0f, -45.0f * DEG2RAD);
    this->creator_movement.SetMouseLook(true);
    
    this->camera = Camera3D{};
    this->camera.fovy = camera_field_of_view_degrees;
    this->camera.projection = CAMERA_PERSPECTIVE;
}

Simulation::~Simulation(){
    CloseWindow();
}

void Simulation::Update(){
    const float delta_seconds = GetFrameTime();
    if (IsKeyPressed(KEY_TAB)) this->creator_movement.ToggleMouseLook();
    this->camera.target = Vector3(0, 0, 0);
    this->creator_movement.Update(delta_seconds);
    this->creator_movement.ApplyTo(camera);
}

void Simulation::Render(){
    BeginDrawing();
    ClearBackground(RAYWHITE);
            
    BeginMode3D(camera);
    DrawGrid(20, 1.0f);
    //DrawTriangle3D(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(-1, 0, 0), BLUE);
    rlBegin(RL_QUADS);
    rlColor4ub(0, 0, 255, 255);
    rlVertex3f(-1.0f, 0.0f, -1.0f);
    rlVertex3f(-1.0f, 0.0f,  1.0f);
    rlVertex3f( 1.0f, 0.0f,  1.0f);
    rlVertex3f( 1.0f, 5.0f, -1.0f);
    rlEnd();
    EndMode3D();
            
    DrawText("WASD / arrows: move    SPACE / CTRL: up / down    SHIFT: boost", 20, 20, 18, GRAY);
    DrawText("mouse: look    TAB: release mouse    ESC: quit", 20, 44, 18, GRAY);
    DrawFPS(window_width - 100, 20);
    EndDrawing();
}

void Simulation::Run()
{
    while (!WindowShouldClose())
    {
      Update();
      Render();
    }
       
}