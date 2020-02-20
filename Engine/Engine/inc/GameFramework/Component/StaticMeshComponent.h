//////////////////////////////////////////////////////////////////////////
//    File        	: StaticMeshComponent.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

class Mesh;
class Material;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class StaticMeshComponent : public TransformComponent
{
    DECLARE_COMPONENT(StaticMeshComponent, TransformComponent)
public:
    StaticMeshComponent();
    virtual ~StaticMeshComponent() {}

public:
    virtual void OnConstruct() override;
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride) override;

public:
    void SetMaterial(const std::string& name);
    void SetMaterial(Ref<Material> newMaterial);

    void SetMesh(const std::string& name);
    void SetMesh(Ref<Mesh> mesh);

    void SetRenderPass(const HashedString& newRenderPassId);

public:
    Ref<Material> material = nullptr;
    Ref<Mesh> mesh;
    HashedString renderPassId;

public:
    static Ref<Component> Create(Ref<Object> object, pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
