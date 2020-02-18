//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlComputeShader.cpp
//    Created By    : Jack Spink
//    Created On 	: [6/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGl/OpenGlComputeShader.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "glfw.h"

//////////////////////////////////////////////////////////////////////////

OpenGlComputeShader::OpenGlComputeShader(const std::string& shaderpath)
{
	std::optional<std::string> source = File::LoadAsString(shaderpath);

	if (source)
	{
		const char* src = source.value().c_str();

		id = glCreateProgram();

		u32 shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		glAttachShader(id, shader);
		glDeleteShader(shader);

		glLinkProgram(id);
	}
}

//////////////////////////////////////////////////////////////////////////

OpenGlComputeShader::~OpenGlComputeShader()
{

}

//////////////////////////////////////////////////////////////////////////

void OpenGlComputeShader::Disptach(const uint3& workGroups)
{
	glUseProgram(id);
	glDispatchCompute(workGroups.x, workGroups.y, workGroups.z);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlComputeShader::WaitForFinish()
{
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

//////////////////////////////////////////////////////////////////////////

OpenGlComputeBuffer OpenGlComputeShader::CreateBuffer(const size_t allocSize)
{
    glUseProgram(id);

    u32 vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, allocSize, nullptr, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo);
    
    return { vbo };
}

//////////////////////////////////////////////////////////////////////////

void OpenGlComputeShader::UploadData(const OpenGlComputeBuffer& vbo, const void* data, const size_t byteSize)
{
    glUseProgram(id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
    glBufferData(GL_ARRAY_BUFFER, byteSize, data, GL_DYNAMIC_DRAW);
}

//////////////////////////////////////////////////////////////////////////
