//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlTexture2D.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Texture2D.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

class OpenGlTexture2D : public Texture2D
{
public:
    OpenGlTexture2D(const u32 inId);
    OpenGlTexture2D(const std::string& texturepath);
    OpenGlTexture2D(const uint2& resolution, const float3* pixels);
    OpenGlTexture2D(const uint2& resolution, const float4* pixels);
	~OpenGlTexture2D();

	virtual void Bind(const u32 location = 0) override;
	virtual void Unbind() override;

	virtual void SetTextureParameter(const TextureParameterType::Enum type, const TextureParameterValue::Enum value) override;

    inline u32 GetTextureId() const { return id; }

    virtual void OnDestroy() override;

private:
	u32 id;
};

//////////////////////////////////////////////////////////////////////////
