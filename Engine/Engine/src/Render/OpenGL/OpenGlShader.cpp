//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlShader.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL

//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGL/OpenGlShader.h"
#include "Render/Platform/Texture2D.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileUtils.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include <glfw.h>
#include <glm/gtc/type_ptr.hpp>

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

static std::unordered_map<std::string, s32> ShaderTypeStringToIdMap =
{
    std::make_pair("vertex", GL_VERTEX_SHADER),
    std::make_pair("geometry", GL_GEOMETRY_SHADER),
    std::make_pair("fragment", GL_FRAGMENT_SHADER),
    std::make_pair("pixel", GL_FRAGMENT_SHADER),
};

//////////////////////////////////////////////////////////////////////////

struct GlslShaderCommandlineParser
{
public:
    GlslShaderCommandlineParser(const std::string& args)
        : commandline(args)
        , iterator(0u)
    {

    }

    bool HasReachedEnd() const { return iterator >= commandline.length(); }

    std::tuple<s32, std::string> ReadNext()
    {
        static std::unordered_map<std::string, s32> ShaderTypeStringMap =
        {
            std::make_pair("-vs", GL_VERTEX_SHADER),
            std::make_pair("-gs", GL_GEOMETRY_SHADER),
            std::make_pair("-ps", GL_FRAGMENT_SHADER),
            std::make_pair("-fs", GL_FRAGMENT_SHADER),
        };

        SkipWhitespaces(commandline, iterator);

        if (HasReachedEnd())
            return std::make_tuple(-1, "Reached end of stream");

        const std::string shaderTypeString = ReadUntilChar(commandline, '=', true, iterator);
        if (ShaderTypeStringMap.find(shaderTypeString) == ShaderTypeStringMap.end())
            return std::make_tuple(-1, "Invalid shader type.");

        const s32 shadertype = ShaderTypeStringMap.at(shaderTypeString);

        ++iterator;
        const std::string shaderpath = ReadUntilChar(commandline, ' ', true, iterator);
        return std::make_tuple(shadertype, shaderpath);
    }

private:

    std::string commandline;
    u32 iterator;

private:

    static bool IsWhitespaceChar(const char c)
    {
        return (c == ' ' || c == '\n' || c == '\t');
    }

    static void SkipWhitespaces(const std::string& commandline, u32& iterator)
    {
        while (iterator < commandline.length() && IsWhitespaceChar(commandline.at(iterator))) ++iterator;
    }

    static std::string ReadUntilChar(const std::string& commandline, const char target, const bool ignoreWhitespaces, u32& iterator)
    {
        std::string result;
        while (iterator < commandline.length() && commandline.at(iterator) != target)
        {
            if (!ignoreWhitespaces || !IsWhitespaceChar(commandline.at(iterator)))
                result += commandline.at(iterator);

            ++iterator;
        }

        return result;
    }
};

//////////////////////////////////////////////////////////////////////////

s32 LoadAndCompileShaderFromFile(const s32 shadertype, const std::string& filepath)
{
    std::optional<std::string> loadedsrc = File::LoadAsString(filepath);
    if (!loadedsrc)
    {
        printf("Couldn't load shader <%s>.\n", filepath.c_str());
        return -1;
    }

    u32 shader = glCreateShader(shadertype);

    const char* source = loadedsrc.value().c_str();
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    s32 shaderCompiledFlag;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiledFlag);

    if (shaderCompiledFlag == 0)
    {
        s32 infoLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

        std::vector<char> info(infoLength);
        glGetShaderInfoLog(shader, infoLength, nullptr, &info.at(0));

        printf("%s\n", &info.at(0));
        return -1;
    }

    return shader;
}

//////////////////////////////////////////////////////////////////////////

OpenGlShader::OpenGlShader(const std::string& filepath)
    : id(0)
{
    id = glCreateProgram();

    std::optional<std::string> commandline = File::LoadAsString(filepath);
    if (commandline)
    {
        GlslShaderCommandlineParser parser(commandline.value());
        while (!parser.HasReachedEnd())
        {
            auto[shadertype, shaderpath] = parser.ReadNext();
            if (shadertype == -1)
                continue;

            s32 shader = LoadAndCompileShaderFromFile(shadertype, shaderpath);
            glAttachShader(id, shader);
        }

        glLinkProgram(id);

        s32 programCompiledFlag;
        glGetProgramiv(id, GL_LINK_STATUS, (int*)&programCompiledFlag);

        if (programCompiledFlag == GL_FALSE)
        {
            s32 infoLength;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLength);

            std::vector<char> info(infoLength);
            glGetProgramInfoLog(id, info.size(), nullptr, &info.at(0));
            printf("%s\n", &info.at(0));
        }
    }
}

//////////////////////////////////////////////////////////////////////////

OpenGlShader::OpenGlShader(const pugi::xml_node& shaderRoot)
    : id(0)
{
    id = glCreateProgram();

    for (pugi::xml_node shaderNode : shaderRoot)
    {
        char const* shader_type_name = shaderNode.attribute("type").as_string();
        char const* shader_path = shaderNode.attribute("path").as_string();

        auto shader_type_itr = ShaderTypeStringToIdMap.find(shader_type_name);
        if (shader_type_itr != ShaderTypeStringToIdMap.end())
        {
            s32 shader = LoadAndCompileShaderFromFile(shader_type_itr->second, shader_path);
            glAttachShader(id, shader);
        }
    }

    glLinkProgram(id);

    s32 programCompiledFlag;
    glGetProgramiv(id, GL_LINK_STATUS, (int*)&programCompiledFlag);

    if (programCompiledFlag == GL_FALSE)
    {
        s32 infoLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLength);

        std::vector<char> info(infoLength);
        glGetProgramInfoLog(id, info.size(), nullptr, &info.at(0));
        printf("%s\n", &info.at(0));
    }
}

//////////////////////////////////////////////////////////////////////////

OpenGlShader::~OpenGlShader()
{

}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::Bind()
{
    glUseProgram(id);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::Unbind()
{
    glUseProgram(0);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::SetTexture(const std::string& name, Ref<Texture2D> texture, u32 binding)
{
    texture->Bind(binding);
    s32 location = FindOrAddParameter(name);
    glUniform1i(location, binding);
}

//////////////////////////////////////////////////////////////////////////

void* OpenGlShader::Lock(const std::string& name, const u32 size)
{
    UBO block = FindOrAddBlock(name, size);

    glBindBufferRange(GL_UNIFORM_BUFFER, block.binding, block.id, 0, size);
    glBindBufferBase(GL_UNIFORM_BLOCK, block.binding, block.id);

    glBindBuffer(GL_UNIFORM_BUFFER, block.id);
    return glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::Unlock(const std::string& name)
{
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0u);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::Upload(const std::string& name, const u32 size, const void* data)
{
    UBO block = FindOrAddBlock(name, size);
    if (block.index != GL_INVALID_INDEX)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, block.binding, block.id, 0, size);
        glBindBufferBase(GL_UNIFORM_BLOCK, block.binding, block.id);

        glBindBuffer(GL_UNIFORM_BUFFER, block.id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::OnDestroy()
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

void OpenGlShader::UploadParameter(const std::string& name, const float2& vector)
{
	s32 location = FindOrAddParameter(name);
	glUniform2fv(location, 1, glm::value_ptr(vector));
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::UploadParameter(const std::string& name, const float3& vector)
{
    s32 location = FindOrAddParameter(name);
    glUniform3fv(location, 1, glm::value_ptr(vector));
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::UploadParameter(const std::string& name, const fmat4& matrix)
{
    s32 location = FindOrAddParameter(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::UploadParameter(const std::string& name, const uint textureId, const uint textureLocation)
{
    s32 location = FindOrAddParameter(name);
    glUniform1i(location, textureLocation);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::UploadParameterArray(const std::string& name, const std::vector<f32>& floatArray)
{
	if (floatArray.size() == 0)
		return;

	s32 location = FindOrAddParameter(name);
	glUniform1fv(location, floatArray.size(), &floatArray.at(0));
}

//////////////////////////////////////////////////////////////////////////

void OpenGlShader::UploadParameterArray(const std::string& name, const std::vector<float3>& float3Array)
{
	if (float3Array.size() == 0)
		return;

	s32 location = FindOrAddParameter(name);
	glUniform3fv(location, float3Array.size(), glm::value_ptr(float3Array.at(0)));
}

//////////////////////////////////////////////////////////////////////////

s32 OpenGlShader::FindOrAddParameter(const std::string& name)
{
    if (uniforms.find(name) == uniforms.end())
    {
        uniforms.emplace(std::make_pair(name, glGetUniformLocation(id, name.c_str())));
    }

    return uniforms.at(name);
}

//////////////////////////////////////////////////////////////////////////

OpenGlShader::UBO OpenGlShader::FindOrAddBlock(const std::string& name, const u32 allocSize)
{
    if (uniformBlocks.find(name) == uniformBlocks.end())
    {
        UBO ubo;
        glGenBuffers(1, &ubo.id);

        glBindBuffer(GL_UNIFORM_BUFFER, ubo.id);
        glBufferData(GL_UNIFORM_BUFFER, allocSize, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        ubo.index = glGetUniformBlockIndex(id, name.c_str());
        if (ubo.index != GL_INVALID_INDEX)
        {
            glGetActiveUniformBlockiv(id, ubo.index, GL_UNIFORM_BLOCK_BINDING, &ubo.binding);
        }

        uniformBlocks.emplace(std::make_pair(name, ubo));
    }

    return uniformBlocks.at(name);
}

//////////////////////////////////////////////////////////////////////////

#endif // _OPENGL

//////////////////////////////////////////////////////////////////////////
