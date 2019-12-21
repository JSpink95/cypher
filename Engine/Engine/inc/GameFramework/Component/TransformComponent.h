//////////////////////////////////////////////////////////////////////////
//    File        	: TransformComponent.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"

//////////////////////////////////////////////////////////////////////////

class TransformComponent : public Component
{
    DECLARE_DERIVED_COMPONENT(TransformComponent, Component)
public:
    // {F44B07F4-BD4E-4D59-8340-7C27B443FDF6}
    static inline const GUID ClassID = { 0xf44b07f4, 0xbd4e, 0x4d59, { 0x83, 0x40, 0x7c, 0x27, 0xb4, 0x43, 0xfd, 0xf6 } };

public:
    float3 position = float3(0.0f);    // 
    float3 rotation = float3(0.0f);    // rotation is in euler angles (degrees)
    float3 scale    = float3(1.0f);    // 

public:
    virtual fmat4 CalculateTransformMatrix() const;
};

//////////////////////////////////////////////////////////////////////////
