//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlVertexArray.cpp
//    Created By    : Jack Spink
//    Created On 	: [4/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGL/OpenGlVertexArray.h"
#include "Render/Platform/Buffer.h"
#include "Core/Thread/GameThread.h"

#include "glfw.h"

//////////////////////////////////////////////////////////////////////////

inline s32 GetAttributeDataType(const ShaderData::Type type)
{
    static s32 TypeMap[ShaderData::Max] = 
    {
        GL_INT, GL_INT, GL_INT, GL_INT,
        GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT,
        GL_FLOAT, GL_FLOAT,
    };
    
    return TypeMap[type];
}

//////////////////////////////////////////////////////////////////////////

OpenGlVertexArray::OpenGlVertexArray()
{
    glCreateVertexArrays(1, &id);
}

//////////////////////////////////////////////////////////////////////////

OpenGlVertexArray::~OpenGlVertexArray()
{
    //glDeleteVertexArrays(1, &id);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexArray::AddBuffer(const Ref<VertexBuffer>& newBuffer)
{
    buffer = newBuffer;

    glBindVertexArray(id);
    buffer->Bind();

    u32 attributeIndex = 0u;
    const VertexBufferLayout& layout = buffer->GetLayout();

    u32 offset = 0u;
    for (const VertexAttribute& attribute : layout)
    {
        glEnableVertexAttribArray(attributeIndex);

        if (ShaderData::IsFloatType(attribute.dataType))
            glVertexAttribPointer(attributeIndex, ShaderData::GetElementCount(attribute.dataType), GetAttributeDataType(attribute.dataType), GL_FALSE, buffer->GetStride(), (void*)offset);
        else if (ShaderData::IsIntegerType(attribute.dataType))
            glVertexAttribIPointer(attributeIndex, ShaderData::GetElementCount(attribute.dataType), GetAttributeDataType(attribute.dataType), buffer->GetStride(), (void*)offset);

        offset += ShaderData::GetByteSize(attribute.dataType);
        ++attributeIndex;
    }

    buffer->Unbind();
    glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexArray::Bind()
{
    glBindVertexArray(id);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexArray::Unbind()
{
    glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexArray::Draw(const s32 mode)
{
    glDrawArrays(mode, 0, buffer->GetCount());
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexArray::OnDestroy()
{
    GetGameThread()->PushThreadTask([this]() -> void
    {
        glDeleteVertexArrays(1, &id);
        buffer->OnDestroy();
        buffer = nullptr;
    });
}

//////////////////////////////////////////////////////////////////////////
