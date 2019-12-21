//////////////////////////////////////////////////////////////////////////
//    File        	: Buffer.h
//    Created By    : Jack Spink
//    Created On 	: [4/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////

#include "Core\Types.h"
#include "Core\Disposable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShaderData
{
    enum Type
    {
        Int, Int2, Int3, Int4,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Max,
    };

    static const size_t GetElementCount(const ShaderData::Type type)
    {
        static size_t TypeMap[ShaderData::Max] =
        {
            1,
            2,
            3,
            4,
            1,
            2,
            3,
            4,
            9,
            16,
        };

        return TypeMap[type];
    }

    static const size_t GetByteSize(const ShaderData::Type type)
    {
        static size_t TypeMap[ShaderData::Max] =
        {
            sizeof(s32) * 1,
            sizeof(s32) * 2,
            sizeof(s32) * 3,
            sizeof(s32) * 4,
            sizeof(f32) * 1,
            sizeof(f32) * 2,
            sizeof(f32) * 3,
            sizeof(f32) * 4,
            sizeof(f32) * 9,
            sizeof(f32) * 16,
        };

        return TypeMap[type];
    }

    static const bool IsIntegerType(const ShaderData::Type type)
    {
        static const bool TypeMap[Max] =
        {
            true, true, true, true,
            false, false, false, false,
            false, false,
        };

        return TypeMap[type];
    }

    static const bool IsFloatType(const ShaderData::Type type)
    {
        static const bool TypeMap[Max] =
        {
            false, false, false, false,
            true, true, true, true,
            true, true,
        };

        return TypeMap[type];
    }
}

//////////////////////////////////////////////////////////////////////////

struct VertexAttribute
{
    std::string name;
    ShaderData::Type dataType;

    VertexAttribute(const std::string& n, const ShaderData::Type t)
        : name(n)
        , dataType(t)
    {

    }
};

//////////////////////////////////////////////////////////////////////////

class VertexBufferLayout
{
public:
    VertexBufferLayout(const std::initializer_list<VertexAttribute>& inAttributes)
        : attributes(inAttributes.begin(), inAttributes.end())
    {

    }

    inline std::vector<VertexAttribute>::iterator begin() { return attributes.begin(); }
    inline std::vector<VertexAttribute>::iterator end() { return attributes.end(); }

    inline std::vector<VertexAttribute>::const_iterator begin() const { return attributes.begin(); }
    inline std::vector<VertexAttribute>::const_iterator end() const { return attributes.end(); }

private:

    std::vector<VertexAttribute> attributes;
};

//////////////////////////////////////////////////////////////////////////

class VertexBuffer: public IDisposable
{
public:
    virtual ~VertexBuffer() = default;

    // 
    // public interface
    // 

    virtual const size_t GetCount() const = 0;
    virtual const size_t GetStride() const = 0;

    virtual const VertexBufferLayout& GetLayout() const = 0;
    virtual void SetLayout(const VertexBufferLayout& layout) = 0;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual void UpdateBuffer(const size_t count, const size_t stride, const void* data) = 0;
};

//////////////////////////////////////////////////////////////////////////

// #todo_indexbuffer(implement)
class IndexBuffer
{

};

//////////////////////////////////////////////////////////////////////////
