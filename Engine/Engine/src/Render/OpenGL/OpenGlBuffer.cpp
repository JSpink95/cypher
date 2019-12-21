//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlBuffer.cpp
//    Created By    : Jack Spink
//    Created On 	: [4/9/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL

//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGL/OpenGlBuffer.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "glfw.h"

//////////////////////////////////////////////////////////////////////////

OpenGlVertexBuffer::OpenGlVertexBuffer(const size_t inCount, const size_t inStride, const void* vertices)
    : id(0u)
    , count(inCount)
    , stride(inStride)
    , layout({})
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, count * stride, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////

OpenGlVertexBuffer::~OpenGlVertexBuffer()
{

}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexBuffer::UpdateBuffer(const size_t newCount, const size_t newStride, const void* data)
{
    count = newCount;
    stride = newStride;

    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, count * stride, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlVertexBuffer::OnDestroy()
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

#endif // _OPENGL

//////////////////////////////////////////////////////////////////////////
