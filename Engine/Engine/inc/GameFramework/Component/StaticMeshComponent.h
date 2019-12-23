//////////////////////////////////////////////////////////////////////////
//    File        	: StaticMeshComponent.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"
#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

class Material;
class VertexArray;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class StaticMeshComponent : public Component
{
    DECLARE_DERIVED_COMPONENT(StaticMeshComponent, Component)
public:
    // {BD3AA522-30E0-429D-91CA-A8E1EA9790B7}
    static inline GUID ClassId = { 0xbd3aa522, 0x30e0, 0x429d, { 0x91, 0xca, 0xa8, 0xe1, 0xea, 0x97, 0x90, 0xb7 } };

public:
    StaticMeshComponent() = default;
    virtual ~StaticMeshComponent() = default;

    virtual void OnConstruct() override;
    virtual void OnDestruct() override;

    void SetScale(const float3& newScale) { scale = newScale; }
    void SetMaterial(Ref<Material> newMaterial) { material = newMaterial; }
    void SetMesh(Ref<VertexArray> newMesh) { mesh = newMesh; }

public:
    Ref<Material> material = nullptr;
    Ref<VertexArray> mesh = nullptr;
    float3 scale = float3(1.0f);
    WeakRef<TransformComponent> ownerTransform;

public:
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride) override;

public:
    static Ref<Component> Create(Ref<Object> object, pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
