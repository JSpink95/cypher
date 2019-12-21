//////////////////////////////////////////////////////////////////////////
//    File        	: Player.cpp
//    Created By    : Jack Spink
//    Created On 	: [11/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Player.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Input/Input.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/PhysicsWorld.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/Camera.h"
#include "GameFramework/Camera/PerspectiveCamera.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

static constexpr const f32 MouseSensitivity = 0.1f;

//////////////////////////////////////////////////////////////////////////

void Player::OnConstruct()
{
    Super::OnConstruct();

    camera = std::make_shared<PerspectiveCamera>();
    Camera::SetActiveCamera(camera);
}

//////////////////////////////////////////////////////////////////////////

void Player::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    const float2 mdt = Input::GetMouseDelta();
    cameraRotation.y += -mdt.x * MouseSensitivity;
    cameraRotation.x = glm::clamp(cameraRotation.x + mdt.y * MouseSensitivity, -89.9f, 89.9f); // don't allow upside down viewing

    input = float2(0.0f);
    if (Input::IsKeyDown(KeyboardKey::A))
    {
        input.x = -1.0f;
    }

    if (Input::IsKeyDown(KeyboardKey::D))
    {
        input.x = 1.0f;
    }

    if (Input::IsKeyDown(KeyboardKey::W))
    {
        input.y = 1.0f;
    }

    if (Input::IsKeyDown(KeyboardKey::S))
    {
        input.y = -1.0f;
    }
}

//////////////////////////////////////////////////////////////////////////

void Player::OnPhysicsUpdate(const f32 dt)
{
    const float3 facing = fquat(glm::radians(cameraRotation)) * float3(0.0f, 0.0f, 1.0f);
    const float3 forward = glm::normalize(facing * float3(1.0f, 0.0f, 1.0f));
    const float3 right = glm::cross(forward, float3(0.0f, 1.0f, 0.0f));

    const float3 movement = (forward * input.y) + (right * input.x);
    if (glm::dot(movement, movement) > 0.0f)
        AddMovementInput(glm::normalize(movement), 5.0f);

    Super::OnPhysicsUpdate(dt);

    float3 eye = transform->position + float3(0.0f, 0.7f, 0.0f);
    camera->SetPosition(eye);
    camera->SetLookDirection(facing);

    const bool grabbing = Input::IsKeyDown(KeyboardKey::F);
    if (grabbing && !holdingObject)
    {
        PhysicsWorld::RayHit hit = Physics::RayCast(eye, forward, 10.0f, { this });
        if (hit.valid)
        {
            printf("We hit something about %.3f units away...\n", hit.distance);
        }
    }
    else if (!grabbing)
    {
        holdingObject = false;
    }
}

//////////////////////////////////////////////////////////////////////////
