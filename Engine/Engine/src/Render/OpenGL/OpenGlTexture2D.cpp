//////////////////////////////////////////////////////////////////////////
//    File        	: Texture2D.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGL/OpenGlTexture2D.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "glfw.h"
#include "stb_image.h"

//////////////////////////////////////////////////////////////////////////

#include <array>

//////////////////////////////////////////////////////////////////////////

OpenGlTexture2D::OpenGlTexture2D(const u32 inId)
    : id(inId)
{

}

//////////////////////////////////////////////////////////////////////////

OpenGlTexture2D::OpenGlTexture2D(const std::string& texturepath)
{
    s32 w, h, c;
    stbi_uc* pixels = stbi_load(texturepath.c_str(), &w, &h, &c, 0);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, c == 3 ? GL_RGB : GL_RGBA, w, h, 0, c == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

}

//////////////////////////////////////////////////////////////////////////

OpenGlTexture2D::OpenGlTexture2D(const uint2& resolution, const float3* pixels)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////

OpenGlTexture2D::OpenGlTexture2D(const uint2& resolution, const float4* pixels)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////

OpenGlTexture2D::~OpenGlTexture2D()
{
}

//////////////////////////////////////////////////////////////////////////

void OpenGlTexture2D::Bind(const u32 location/* = 0*/)
{
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, id);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlTexture2D::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlTexture2D::SetTextureParameter(const TextureParameterType::Enum type, const TextureParameterValue::Enum value)
{
	static std::array<s32, TextureParameterValue::Max> valueMap =
	{
		GL_LINEAR,			// Linear,
		GL_NEAREST,			// Nearest,
		GL_REPEAT,			// Repeat,
		GL_CLAMP_TO_EDGE,	// ClampToEdge,
	};

	static std::array<void(*)(const s32), TextureParameterType::Max> setModeFuncs =
	{
		// set filter
		[](const s32 value)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value);
		},

		// set wrap
		[](const s32 value)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, value);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, value);
		},
	};

    glBindTexture(GL_TEXTURE_2D, id);
	setModeFuncs.at(type)(valueMap.at(value));
}

//////////////////////////////////////////////////////////////////////////

void OpenGlTexture2D::OnDestroy()
{
    if (id == 0)
        return;

    auto destroyTextureId = [](u32 id) -> void
    {
        glDeleteTextures(1, &id);
    };

    GetGameThread()->PushThreadTask(std::bind(destroyTextureId, id));
}

//////////////////////////////////////////////////////////////////////////
