//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlBuffer.h
//    Created By    : Jack Spink
//    Created On 	: [4/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Buffer.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class OpenGlVertexBuffer: public VertexBuffer
{
public:
    OpenGlVertexBuffer(const size_t count, const size_t stride, const void* vertices);
    virtual ~OpenGlVertexBuffer();

    inline virtual const size_t GetCount() const override { return count; }
    inline virtual const size_t GetStride() const override { return stride; }

    inline virtual const VertexBufferLayout& GetLayout() const override { return layout; }
    inline virtual void SetLayout(const VertexBufferLayout& newLayout) override { layout = newLayout; }

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual void UpdateBuffer(const size_t newCount, const size_t newStride, const void* data) override;

    virtual void OnDestroy() override;

private:
    u32 id;
    size_t count, stride;
    VertexBufferLayout layout;
};

//////////////////////////////////////////////////////////////////////////
