//////////////////////////////////////////////////////////////////////////
//    File        	: MeshPrimitiveBuilder.cpp
//    Created By    : Jack Spink
//    Created On 	: [24/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshPrimitiveBuilder.h"

//////////////////////////////////////////////////////////////////////////

namespace MeshPrimitiveBuilder
{
    void CreateBox(const float3& centre, const float3& halfSize, std::vector<PosNorTex>& pnt, FaceFlag const flags)
    {
        static auto BitCheck = [](s32 flags, s32 test) -> bool
        {
            return (flags & test) == test;
        };

        // add top face

        if (BitCheck(flags, TopFace))
        {
            pnt.push_back({ centre + float3(halfSize.x, halfSize.y, -halfSize.z), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, halfSize.y, -halfSize.z), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, halfSize.y,  halfSize.z), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) });

            pnt.push_back({ centre + float3(halfSize.x, halfSize.y, -halfSize.z), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, halfSize.y,  halfSize.z), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x, halfSize.y,  halfSize.z), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) });
        }

        // add bottom face

        if (BitCheck(flags, BottomFace))
        {
            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, -halfSize.z), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y,  halfSize.z), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y, -halfSize.z), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) });

            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, -halfSize.z), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y,  halfSize.z), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y,  halfSize.z), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) });
        }

        // add left face

        if (BitCheck(flags, LeftFace))
        {
            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, -halfSize.z), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x,  halfSize.y, -halfSize.z), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x,  halfSize.y,  halfSize.z), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });

            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, -halfSize.z), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x,  halfSize.y,  halfSize.z), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y,  halfSize.z), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
        }

        // add right face

        if (BitCheck(flags, RightFace))
        {
            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y, -halfSize.z), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x,  halfSize.y,  halfSize.z), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x,  halfSize.y, -halfSize.z), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });

            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y, -halfSize.z), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y,  halfSize.z), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x,  halfSize.y,  halfSize.z), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) });
        }

        // add front face

        if (BitCheck(flags, FrontFace))
        {
            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, halfSize.z), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x,  halfSize.y, halfSize.z), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y, halfSize.z), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) });

            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, halfSize.z), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x,  halfSize.y, halfSize.z), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x,  halfSize.y, halfSize.z), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) });
        }

        // add back face

        if (BitCheck(flags, BackFace))
        {
            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, -halfSize.z), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x, -halfSize.y, -halfSize.z), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x,  halfSize.y, -halfSize.z), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) });

            pnt.push_back({ centre + float3(halfSize.x, -halfSize.y, -halfSize.z), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(-halfSize.x,  halfSize.y, -halfSize.z), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) });
            pnt.push_back({ centre + float3(halfSize.x,  halfSize.y, -halfSize.z), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) });
        }
    }
}

//////////////////////////////////////////////////////////////////////////
