//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEditorApplication.h
//    Created By    : Jack Spink
//    Created On 	: [08/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Application.h"

//////////////////////////////////////////////////////////////////////////

class Object;
class GameObject;

//////////////////////////////////////////////////////////////////////////

class ParticleEditorApplication : public Application
{
protected:

    virtual void OnRenderCreate() override;
    virtual void OnPostCreate() override;
    virtual void OnPostUpdate() override;
    virtual void OnImGuiRender() override;

private:
    void AddNewDefaultEffect(const std::string& id, bool makeActive = false, const float3& atLocation = float3(0.0f));

private:
    Ref<GameObject> gizmoObject;
    Ref<GameObject> editorController;
	Ref<GameObject> lightObject;
    Ref<GameObject> gridObject;
    Ref<Object> activeParticleSystem;

private:
    std::vector<Ref<Object>> editableParticleSystems;
    std::unordered_map<ObjectId, Ref<Object>> allWorldObjects;
    f32 audioBeepTimer = 0.0f;
};

//////////////////////////////////////////////////////////////////////////
