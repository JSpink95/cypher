//////////////////////////////////////////////////////////////////////////
//    File        	: MaterialEditor.h
//    Created By    : Jack Spink
//    Created On 	: [06/02/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Application.h"

//////////////////////////////////////////////////////////////////////////

class TextEditor;

//////////////////////////////////////////////////////////////////////////

namespace ShaderInfo
{
    enum Enum: u8;
}

//////////////////////////////////////////////////////////////////////////

class MaterialEditorApplication : public Application
{
public:
    virtual void OnPostCreate() override;
    virtual void OnImGuiRender() override;

private:
    s32 selectedShaderDropDown = -1;
    ShaderInfo::Enum activeShaderEdit;
    std::unordered_map<ShaderInfo::Enum, std::string> sources;

private:
    Ref<TextEditor> editor;
};

//////////////////////////////////////////////////////////////////////////
