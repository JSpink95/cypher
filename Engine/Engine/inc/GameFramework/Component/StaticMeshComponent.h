//////////////////////////////////////////////////////////////////////////
//    File        	: StaticMeshComponent.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"
#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

class Material;
class VertexArray;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class StaticMeshComponent : public TransformComponent
{
    DECLARE_COMPONENT(StaticMeshComponent, TransformComponent)
public:
    StaticMeshComponent();
    virtual ~StaticMeshComponent() = default;

    virtual void OnConstruct() override;
    virtual void OnDestruct() override;

    void SetMaterial(Ref<Material> newMaterial) { material = newMaterial; }
    void SetMesh(Ref<VertexArray> newMesh) { mesh = newMesh; }

public:
    Ref<Material> material = nullptr;
    Ref<VertexArray> mesh = nullptr;

public:
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride) override;

public:
    static Ref<Component> Create(Ref<Object> object, pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
