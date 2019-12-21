//////////////////////////////////////////////////////////////////////////
//    File        	: CameraManager.cpp
//    Created By    : Jack Spink
//    Created On 	: [16/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/CameraManager.h"
#include "GameFramework/Camera/Camera.h"

//////////////////////////////////////////////////////////////////////////

CameraShake::CameraShake(const Ref<CameraShakeData>& newData)
    : elapsedTime(0.0f)
{
    if (newData != nullptr)
    {
        data.looping = newData->looping;
        data.oscillationDuration = newData->oscillationDuration;

        for (u32 axis = 0u; axis < 3u; ++axis)
        {
            data.positionalOscillation[axis] = newData->positionalOscillation[axis];
            data.rotationalOscillation[axis] = newData->rotationalOscillation[axis];
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void CameraShake::Update(const f32 dt)
{
    if (!data.looping)
        elapsedTime = glm::clamp(elapsedTime + dt, 0.0f, data.oscillationDuration);
    else
        elapsedTime += dt;
}

//////////////////////////////////////////////////////////////////////////

CameraManager::CameraManager()
{

}

//////////////////////////////////////////////////////////////////////////

fmat4 CameraManager::GetViewMatrix() const
{
    fmat4 view = activeCamera->GetViewMatrix();
    view = shakeMatrix * view;

    return view;
}

//////////////////////////////////////////////////////////////////////////

fmat4 CameraManager::GetViewProjectionMatrix() const
{
    fmat4 projection = activeCamera->GetProjectionMatrix();
    fmat4 view = activeCamera->GetViewMatrix();

    return (projection * shakeMatrix * view);
}

//////////////////////////////////////////////////////////////////////////

void CameraManager::Update()
{
    static constexpr const f32 deltaTime = 0.01666666666666667f;

    // always try and get back to no offset
    positionOffset = glm::mix(positionOffset, float3(0.0f), glm::clamp(10.0f * deltaTime, 0.0f, 1.0f));

    float3 offset;
    float3 rotation;

    for (CameraShake& shake : shakes)
    {
        shake.Update(deltaTime);
        offset += shake.GetPositionalOscillation();
        rotation += shake.GetRotationalOscillation();
    }

    auto RemoveClause = [](const CameraShake& shake) -> bool
    {
        // remove if (we aren't looping AND have reached the end of the shake) OR (the shake has been forced to stop)
        return (!shake.data.looping && shake.HasElapsed()) || shake.WantsToStop();
    };

    // remove all shakes that have elapsed
    shakes.erase(std::remove_if(shakes.begin(), shakes.end(), RemoveClause), shakes.end());

    // add on to the current positional offset
    positionOffset += offset;
    
    // build the "shake" matrix
    shakeMatrix = fmat4(1.0f);
    shakeMatrix = glm::translate(shakeMatrix, positionOffset);
}

//////////////////////////////////////////////////////////////////////////

void CameraManager::PerformShake(const Ref<CameraShakeData>& shakeData)
{
    shakes.push_back(CameraShake(shakeData));
}

//////////////////////////////////////////////////////////////////////////
