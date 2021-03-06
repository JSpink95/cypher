//////////////////////////////////////////////////////////////////////////
//    File        	: VertexArray.h
//    Created By    : Jack Spink
//    Created On 	: [4/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Disposable.h"

//////////////////////////////////////////////////////////////////////////

namespace RenderMode
{
    enum Enum: u8;
}

//////////////////////////////////////////////////////////////////////////

class VertexBuffer;

//////////////////////////////////////////////////////////////////////////

class VertexArray: public IDisposable
{
public:
    virtual ~VertexArray() = default;

    virtual void AddBuffer(const Ref<VertexBuffer>& buffer) = 0;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Draw(const RenderMode::Enum mode) = 0;
};

//////////////////////////////////////////////////////////////////////////
