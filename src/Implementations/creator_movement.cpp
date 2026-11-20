#include "../Headers/creator_movement.h"

#include "raymath.h"

#include <cmath>

// Reminder the namespaces use here is for declaring that these two are only for this file so there are no conflicts in the future.
namespace
{
    // How far the view may tilt up or down. Stopping just short of straight up
    // keeps the view from flipping over at the poles.
    const float maximum_vertical_radians = 89.0f * DEG2RAD;
    // The world's "up". Vertical movement follows this, and the camera uses it
    // to decide which way is up on screen.
    const Vector3 world_up_direction{ 0.0f, 1.0f, 0.0f };
}

Creator_Movement::Creator_Movement(
    Vector3 start_position,
    float start_horizontal_radians,
    float start_vertical_radians)
    :
    position(start_position),
    horizontal_radians(start_horizontal_radians),
    vertical_radians(start_vertical_radians)
{
    // Not empty like Planet's. The direction vectors are built from the angles,
    // so they would be stale until the first Update without this.
    RebuildDirectionVectors();
}

void Creator_Movement::Look(float new_horizontal_radians, float new_vertical_radians)
{
    horizontal_radians = new_horizontal_radians;
    vertical_radians   = Clamp(new_vertical_radians, -maximum_vertical_radians, maximum_vertical_radians);
    RebuildDirectionVectors();
}

void Creator_Movement::SetMouseLook(bool enabled)
{
    mouse_look_enabled = enabled;

    if (mouse_look_enabled) DisableCursor();
    else                    EnableCursor();
}

void Creator_Movement::Update(float delta_seconds)
{
    UpdateLookFromMouse();
    RebuildDirectionVectors();
    UpdatePositionFromKeyboard(delta_seconds);
}

void Creator_Movement::UpdateLookFromMouse()
{
    if (!mouse_look_enabled) return;

    // How many pixels the mouse moved since the previous frame.
    const Vector2 mouse_movement = GetMouseDelta();

    // Moving the mouse right should turn the view right, which means turning
    // the other way around the vertical axis - hence the subtraction.
    horizontal_radians -= mouse_movement.x * mouse_sensitivity;
    vertical_radians   -= mouse_movement.y * mouse_sensitivity;
    vertical_radians    = Clamp(vertical_radians, -maximum_vertical_radians, maximum_vertical_radians);
}

void Creator_Movement::RebuildDirectionVectors()
{
    // Turn the two angles back into a direction. The vertical angle shortens the
    // horizontal part of the vector as the view tilts, which is what this factor is.
    const float horizontal_shrink_factor = cosf(vertical_radians);

    forward_direction = Vector3Normalize(Vector3{
        -sinf(horizontal_radians) * horizontal_shrink_factor,
        sinf(vertical_radians),
        -cosf(horizontal_radians) * horizontal_shrink_factor
    });

    // "Right" is perpendicular to both where we look and world up.
    right_direction = Vector3Normalize(Vector3CrossProduct(forward_direction, world_up_direction));
}

void Creator_Movement::UpdatePositionFromKeyboard(float delta_seconds)
{
    // Sum of every direction the held keys are asking for this frame.
    Vector3 requested_direction{ 0.0f, 0.0f, 0.0f };

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    requested_direction = Vector3Add(requested_direction, forward_direction);
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  requested_direction = Vector3Subtract(requested_direction, forward_direction);
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) requested_direction = Vector3Add(requested_direction, right_direction);
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  requested_direction = Vector3Subtract(requested_direction, right_direction);
    if (IsKeyDown(KEY_SPACE))                     requested_direction = Vector3Add(requested_direction, world_up_direction);
    if (IsKeyDown(KEY_LEFT_CONTROL))              requested_direction = Vector3Subtract(requested_direction, world_up_direction);

    // Nothing held, or two opposite keys cancelling out.
    if (Vector3LengthSqr(requested_direction) <= 0.0f) return;

    const float current_speed = movement_speed_per_second
                              * (IsKeyDown(KEY_LEFT_SHIFT) ? boost_multiplier : 1.0f);

    // Normalising first means holding two keys at once does not travel faster
    // than holding one.
    const Vector3 travel_this_frame =
        Vector3Scale(Vector3Normalize(requested_direction), current_speed * delta_seconds);

    position = Vector3Add(position, travel_this_frame);
}

void Creator_Movement::ApplyTo(Camera3D &camera) const
{
    camera.position = position;
    camera.target   = Vector3Add(position, forward_direction);
    camera.up       = world_up_direction;
}
