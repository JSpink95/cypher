//////////////////////////////////////////////////////////////////////////
//    File        	: FpsCameraController.cpp
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Dungeon/FpsCameraController.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Application.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"
#include "GameFramework/Camera/PerspectiveCamera.h"
#include "GameFramework/Component/ProjectileMovementComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleSystem.h"
#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

static const std::string StrafeLeft = "StrafeLeft";
static const std::string StrafeRight = "StrafeRight";
static const std::string MoveForward = "MoveForward";
static const std::string MoveBackward = "MoveBackward";
static const std::string Jump = "Jump";
static const std::string Crouch = "Crouch";
static const std::string Shoot = "Shoot";

//////////////////////////////////////////////////////////////////////////

void FpsCameraController::OnConstruct()
{
    Super::OnConstruct();

    camera = std::make_shared<PerspectiveCamera>();
    camera->SetLookAt(vec3(8.0f, 0.0f, 8.0f));
    Camera::SetActiveCamera(camera);

    transform->position = float3(8.0f, 0.0f, 1.0f);

    GetGameThread()->AddObject(GetWeakRef());

    InputManager::RegisterInput(StrafeLeft, { KeyboardKey::A, KeyboardKey::LEFT });
    InputManager::RegisterInput(StrafeRight, { KeyboardKey::D, KeyboardKey::RIGHT });
    InputManager::RegisterInput(MoveForward, { KeyboardKey::W, KeyboardKey::UP });
    InputManager::RegisterInput(MoveBackward, { KeyboardKey::S, KeyboardKey::DOWN });
    InputManager::RegisterInput(Jump, { KeyboardKey::SPACE });
    InputManager::RegisterInput(Crouch, { KeyboardKey::LEFT_CONTROL, KeyboardKey::RIGHT_CONTROL });
    InputManager::RegisterInput(Shoot, { KeyboardKey::E });
}

//////////////////////////////////////////////////////////////////////////

void FpsCameraController::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    if (inputEnabled)
        HandleInput(dt);
}

//////////////////////////////////////////////////////////////////////////

void FpsCameraController::HandleInput(const f32 dt)
{
    static f32 moveSpeed = 4.0f;
    static f32 strafeSpeed = 3.25f;
    static f32 mouseSensitivity = 0.1f;

    float2 const mdt = Input::GetMouseDelta();
    transform->rotation.y += -mdt.x * mouseSensitivity; 
    transform->rotation.x = glm::clamp(transform->rotation.x + mdt.y * mouseSensitivity, -89.9f, 89.9f); // don't allow upside down viewing

    gravityMovement += (-9.81f * dt);
    bool grounded = (transform->position.y + (gravityMovement * dt)) < 0.0f;

    if (grounded && InputManager::GetInputState(Jump) == KeyboardState::Pressed)
    {
        gravityMovement = 5.0f;
        grounded = false;
    }

    transform->position.y += (gravityMovement * dt);

    if (transform->position.y < 0.0f)
    {
        gravityMovement = 0.0f;
        transform->position.y = 0.0f;
    }

    float3 const facingDirection = fquat(glm::radians(transform->rotation)) * float3(0.0f, 0.0f, 1.0f);

    float3 forward = glm::normalize(facingDirection * float3(1.0f, 0.0f, 1.0f));
    float3 left = glm::cross(float3(0.0f, 1.0f, 0.0f), forward);
    f32 airMultiplier = grounded ? 1.0f : 0.9f;

    float3 targetVelocity = float3(0.0f);
    if (InputManager::IsInputDown(MoveForward))
    {
        targetVelocity += forward * moveSpeed * airMultiplier;
    }

    if (InputManager::IsInputDown(MoveBackward))
    {
        targetVelocity -= forward * moveSpeed * airMultiplier;
    }

    if (InputManager::IsInputDown(StrafeLeft))
    {
        targetVelocity += left * strafeSpeed * airMultiplier;
    }

    if (InputManager::IsInputDown(StrafeRight))
    {
        targetVelocity -= left * strafeSpeed * airMultiplier;
    }

    if (InputManager::IsInputDown(Crouch))
    {
        eyeHeight -= dt * 5.0f;
    }
    else
    {
        eyeHeight += dt * 5.0f;
    }

    eyeHeight = glm::clamp(eyeHeight, 0.5f, 1.25f);

    velocity = glm::mix(velocity, targetVelocity, 10.0f * dt);
    transform->position += velocity * dt;

    camera->SetPosition(transform->position + float3(0.0f, eyeHeight, 0.0f));
    camera->SetLookDirection(facingDirection);
}

//////////////////////////////////////////////////////////////////////////
