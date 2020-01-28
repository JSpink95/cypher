//////////////////////////////////////////////////////////////////////////
//    File        	: StaticMeshComponent.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

class Material;
class VertexArray;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class StaticMesh
{
public:
    std::string name;
    Ref<VertexArray> vertexList;
};

//////////////////////////////////////////////////////////////////////////

class StaticMeshComponent : public TransformComponent
{
    DECLARE_COMPONENT(StaticMeshComponent, TransformComponent)
public:
    StaticMeshComponent();
    virtual ~StaticMeshComponent() {}

public:
    inline void SetMaterial(Ref<Material> newMaterial) { material = newMaterial; }
    void SetMesh(const std::string& name);

public:
    Ref<Material> material = nullptr;
    StaticMesh mesh;

public:
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride) override;

public:
    static Ref<Component> Create(Ref<Object> object, pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
