//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlVertexArray.h
//    Created By    : Jack Spink
//    Created On 	: [4/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

class OpenGlVertexArray : public VertexArray
{
public:
    OpenGlVertexArray();
    virtual ~OpenGlVertexArray();

    virtual void AddBuffer(const Ref<VertexBuffer>& buffer) override;

    virtual void Bind() override;
    virtual void Unbind() override;
    virtual void Draw(const RenderMode::Enum mode) override;

    virtual void OnDestroy() override;

private:
    u32 id;
    Ref<VertexBuffer> buffer;
};

//////////////////////////////////////////////////////////////////////////
