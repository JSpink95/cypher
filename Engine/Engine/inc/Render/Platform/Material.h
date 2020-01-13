//////////////////////////////////////////////////////////////////////////
//    File        	: Material.h
//    Created By    : Jack Spink
//    Created On 	: [23/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Disposable.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/MaterialParameter.h"

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Shader;

//////////////////////////////////////////////////////////////////////////

namespace RenderMode
{
    enum Enum : u8;
}

//////////////////////////////////////////////////////////////////////////

using MaterialParameterMap = std::unordered_map<std::string, Ref<MaterialParameter>>;

//////////////////////////////////////////////////////////////////////////

class Material: public IDisposable
{
public:
    Material(const Ref<Shader>& associatedShader);
    ~Material();

    Ref<Material> Clone();

    virtual void OnDestroy() override;

public:

    template<typename TMaterialParameter>
    Ref<TMaterialParameter> FindOrAddParameter(const std::string& id);

    template<typename TMaterialParameter>
    void SetParameterBlock(const std::string& id, TMaterialParameter& data);

    template<typename TMaterialParameter>
    void SetParameterValue(const std::string& id, typename const TMaterialParameter::value_type& data);

public:

    inline const Ref<Shader>& GetShader() const { return shader; }
    inline RenderMode::Enum GetRenderMode() const { return renderMode; }

    void BindProperties();
    void Reload();

private:
    std::string path;
    Ref<Shader> shader;
    MaterialParameterMap parameters;
    RenderMode::Enum renderMode;

public:
    static Ref<Material> LoadFromFile(const std::string& filepath);
};

//////////////////////////////////////////////////////////////////////////

template<typename TMaterialParameter>
inline Ref<TMaterialParameter> Material::FindOrAddParameter(const std::string& id)
{
    auto it = parameters.find(id);
    if (it != parameters.end())
        return std::dynamic_pointer_cast<TMaterialParameter>(it->second);

    Ref<TMaterialParameter> parameter = std::make_shared<TMaterialParameter>(id);
    parameters.emplace(id, parameter);

    return parameter;
}

//////////////////////////////////////////////////////////////////////////

template<typename TMaterialParameter>
inline void Material::SetParameterBlock(const std::string& id, TMaterialParameter& data)
{
    auto parameter = parameters.find(id);
    if (parameter != parameters.end())
    {
        std::dynamic_pointer_cast<MaterialParameterBlock>(parameter->second)->SetData<TMaterialParameter>(data);
    }
}

//////////////////////////////////////////////////////////////////////////

template<typename TMaterialParameter>
void Material::SetParameterValue(const std::string& id, typename const TMaterialParameter::value_type& data)
{
    auto parameter = parameters.find(id);
    if (parameter != parameters.end())
    {
        std::dynamic_pointer_cast<TMaterialParameter>(parameter->second)->SetData(data);
    }
}

//////////////////////////////////////////////////////////////////////////
