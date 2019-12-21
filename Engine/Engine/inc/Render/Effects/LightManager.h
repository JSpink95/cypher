//////////////////////////////////////////////////////////////////////////
//    File        	: LightManager.h
//    Created By    : Jack Spink
//    Created On 	: [14/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Singleton.h"
#include "Render/Platform/CoreMaterialParameterBlocks.h"

//////////////////////////////////////////////////////////////////////////

#include <array>

//////////////////////////////////////////////////////////////////////////

struct LightData
{
    vec3 position;
    f32 radius;
    vec4 color;
};

//////////////////////////////////////////////////////////////////////////

struct LightBuffer
{
    static inline constexpr const u64 MaximumLights = 16u;

    std::array<LightData, MaximumLights> lights;
    int32 count;
};

//////////////////////////////////////////////////////////////////////////

class LightObject
{
    friend class LightManager;
public:
    LightObject(u32 index = LightBuffer::MaximumLights);

    bool IsValid() const;
    void SetPosition(const vec3& newPos);
    void SetRadius(const f32 newRad);
    void SetColor(const vec4& newCol);

private:
    u32 lightId;
};

//////////////////////////////////////////////////////////////////////////

class LightManager : public Singleton<LightManager>
{
    friend LightObject;
public:
    LightBuffer GetBufferData() const;

    Ref<LightObject> InsertInstance(const vec3& pos = vec3(0.0f), const f32 rad = 1.0f, const vec4& col = vec4(1.0f));
    void RemoveInstance(const Ref<LightObject>& instance);
    void RemoveInstance(const u32 index);

private:
    LightBuffer buffer;
    std::vector<Ref<LightObject>> activeLights;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<LightManager> GetLightManager()
{
    return GetSingleton<LightManager>();
}

//////////////////////////////////////////////////////////////////////////
