//////////////////////////////////////////////////////////////////////////
//    File        	: LightManager.cpp
//    Created By    : Jack Spink
//    Created On 	: [14/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Effects/LightManager.h"
#include "Core/Core.h"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>

LightObject::LightObject(u32 index /*= LightUniformBuffer::MaximumLights*/)
    : lightId(index)
{

}

//////////////////////////////////////////////////////////////////////////

bool LightObject::IsValid() const
{
    return lightId < GetLightManager()->buffer.count;
}

//////////////////////////////////////////////////////////////////////////

void LightObject::SetPosition(const vec3& newPos)
{
    GetLightManager()->buffer.lights.at(lightId).position = newPos;
}

//////////////////////////////////////////////////////////////////////////

void LightObject::SetRadius(const f32 newRad)
{
    GetLightManager()->buffer.lights.at(lightId).radius = newRad;
}

//////////////////////////////////////////////////////////////////////////

void LightObject::SetColor(const vec4& newCol)
{
    GetLightManager()->buffer.lights.at(lightId).color = newCol;
}

//////////////////////////////////////////////////////////////////////////

LightBuffer LightManager::GetBufferData() const
{
    return buffer;
}

//////////////////////////////////////////////////////////////////////////

Ref<LightObject> LightManager::InsertInstance(const vec3& pos/* = vec3(0.0f)*/, const f32 rad/*= 1.0f*/, const vec4& col/*= vec4(1.0f)*/)
{
    if (buffer.count >= LightBuffer::MaximumLights)
    {
        return nullptr;
    }

    const u32 lightId = buffer.count;
    buffer.count += 1;

    LightData& layout = buffer.lights.at(lightId);
    layout.position = pos;
    layout.radius = rad;
    layout.color = col;

    Ref<LightObject> newLightObject = std::make_shared<LightObject>(lightId);
    activeLights.push_back(newLightObject);

    return newLightObject;
}

//////////////////////////////////////////////////////////////////////////

void LightManager::RemoveInstance(const Ref<LightObject>& object)
{
    // if the object is not the last item in the array we need to swap the objects about
    // and pop the last item off, as we know that light is now invalid.

    if (object != activeLights.back())
    {
        Ref<LightObject> lastLightObject = activeLights.back();

        //
        // update the light data
        //

        const LightData& lightToCopy = buffer.lights.at(lastLightObject->lightId);
        LightData& lightToUpdate = buffer.lights.at(object->lightId);

        lightToUpdate.position = lightToCopy.position;
        lightToUpdate.radius = lightToCopy.radius;
        lightToUpdate.color = lightToCopy.color;

        //
        // swap the light pointers
        //

        std::swap(activeLights.at(object->lightId), activeLights.back());

        // 
        // update the light ids
        // 

        lastLightObject->lightId = object->lightId;
        object->lightId = LightBuffer::MaximumLights;
    }

    activeLights.pop_back();
    buffer.count = activeLights.size();
}

//////////////////////////////////////////////////////////////////////////

void LightManager::RemoveInstance(const u32 index)
{
    if (index >= activeLights.size())
        return;

    RemoveInstance(activeLights.at(index));
}

//////////////////////////////////////////////////////////////////////////
