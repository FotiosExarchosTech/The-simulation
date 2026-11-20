#pragma once

#include "raylib.h"

class Creator_Movement
{
public:
    Creator_Movement() = default; // Reminder: declaring the constructor below is what kills the free no-argument one. This hands it back, so a Creator_Movement can exist before you know where it starts.
    Creator_Movement
    (
        Vector3 start_position,
        float start_horizontal_radians,
        float start_vertical_radians // Tilt up and down. Gets clamped in the cpp so the view cannot flip over at the poles.
    );

    // Reads keyboard and mouse input, then advances the view by delta_seconds.
    void Update(float delta_seconds);

    // Copies the current position and orientation into a camera.
    void ApplyTo(Camera3D &camera) const;

    // One line functions.
    // There was no point for them to exist in the cpp file.
    Vector3 Position() const { return position; }
    Vector3 ForwardDirection() const { return forward_direction; }
    Vector3 RightDirection() const { return right_direction; }
    float HorizontalRadians() const { return horizontal_radians; }
    float VerticalRadians() const { return vertical_radians; }

    void SetPosition(Vector3 new_position) { position = new_position; }

    bool MouseLookEnabled() const { return mouse_look_enabled; }
    void ToggleMouseLook() { SetMouseLook(!mouse_look_enabled); } // Calling SetMouseLook before it is declared is fine, the compiler reads the whole class first.

    // Complex functions. Implementation in cpp file.
    void Look(float new_horizontal_radians, float new_vertical_radians);
    void SetMouseLook(bool enabled);

    // Tunables, public on purpose so they can be adjusted while running.
    float movement_speed_per_second = 12.0f;    // world units travelled per second
    float boost_multiplier = 4.0f;     // applied while the boost key is held
    float mouse_sensitivity = 0.0025f;  // radians turned per pixel of mouse movement

private:
    // Complex functions. Implementation in cpp file.
    // Private because they are steps of Update, not things to call from outside.
    void UpdateLookFromMouse();
    void UpdatePositionFromKeyboard(float delta_seconds);
    void RebuildDirectionVectors();

    // Default values for the default constructor.

    // Where the creator is in the world.
    Vector3 position{ 0.0f, 0.0f, 12.0f };

    // Which way the creator is looking. horizontal_radians turns left and right
    // around the world's vertical axis; vertical_radians tilts up and down.
    // Both are zero when looking straight down the -Z axis.
    float horizontal_radians = 0.0f;
    float vertical_radians   = 0.0f;

    // Recomputed from the two angles every frame, kept here so movement and the
    // camera can share them without recalculating.
    Vector3 forward_direction{ 0.0f, 0.0f, -1.0f };
    Vector3 right_direction{ 1.0f, 0.0f, 0.0f };

    // True while the mouse is captured and controlling the view.
    bool mouse_look_enabled = true;
};
