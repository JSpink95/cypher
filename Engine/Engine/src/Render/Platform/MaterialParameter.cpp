//////////////////////////////////////////////////////////////////////////
//    File        	: MaterialParameter.cpp
//    Created By    : Jack Spink
//    Created On 	: [23/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/MaterialParameter.h"

//////////////////////////////////////////////////////////////////////////

MaterialParameter::MaterialParameter(const std::string& id)
    : parameterId(id)
{

}

//////////////////////////////////////////////////////////////////////////

MaterialParameterTexture2D::MaterialParameterTexture2D(const std::string& id)
    : MaterialParameter(id)
{

}

//////////////////////////////////////////////////////////////////////////

void MaterialParameterTexture2D::SetParameter(const Ref<Shader>& shader)
{
    shader->SetTexture(parameterId, texture, binding);
}

//////////////////////////////////////////////////////////////////////////

Ref<MaterialParameter> MaterialParameterTexture2D::Clone()
{
    Ref<MaterialParameterTexture2D> parameter = std::make_shared<MaterialParameterTexture2D>(parameterId);
    parameter->SetBlockBinding(binding);
    parameter->SetData(texture);

    return parameter;
}

//////////////////////////////////////////////////////////////////////////

void MaterialParameterTexture2D::SetBlockBinding(const u32 newBinding)
{
    binding = newBinding;
}

//////////////////////////////////////////////////////////////////////////

void MaterialParameterTexture2D::SetData(const Ref<Texture2D>& newTexture)
{
    texture = newTexture;
}

//////////////////////////////////////////////////////////////////////////

MaterialParameterBlock::MaterialParameterBlock(const std::string& id)
    : MaterialParameter(id)
{

}

//////////////////////////////////////////////////////////////////////////

void MaterialParameterBlock::SetParameter(const Ref<Shader> &shader)
{
    if (buffer.data != nullptr)
        shader->Upload(parameterId, buffer.size, buffer.data);
}

//////////////////////////////////////////////////////////////////////////

Ref<MaterialParameter> MaterialParameterBlock::Clone()
{
    Ref<MaterialParameterBlock> parameter = std::make_shared<MaterialParameterBlock>(parameterId);
    parameter->SetBlockBinding(binding);
    parameter->SetDataGeneric(buffer);

    return parameter;
}

//////////////////////////////////////////////////////////////////////////

void MaterialParameterBlock::SetBlockBinding(const u32 newBinding)
{
    binding = newBinding;
}

//////////////////////////////////////////////////////////////////////////

void MaterialParameterBlock::SetDataGeneric(const BlockBuffer& newBuffer)
{
    if (buffer.data != nullptr)
    {
        std::free(buffer.data);
        buffer.data = nullptr;
    }

    buffer.size = newBuffer.size;
    buffer.data = std::malloc(buffer.size);

    if (newBuffer.data != nullptr)
        std::memcpy(buffer.data, newBuffer.data, buffer.size);
}

//////////////////////////////////////////////////////////////////////////
