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

private:
    WeakRef<TransformComponent> ownerTransform;

public:
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride) override;

public:
    static Ref<Component> Create(Ref<Object> object, pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
