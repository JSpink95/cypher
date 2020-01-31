//////////////////////////////////////////////////////////////////////////
//    File        	: Skeleton.cpp
//    Created By    : Jack Spink
//    Created On 	: [30/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Animation/Skeleton.h"

//////////////////////////////////////////////////////////////////////////

fmat4 Joint::CalculateFullTransform() const
{
    fmat4 result = transform;

    if (parent != nullptr)
        result = parent->CalculateFullTransform() * result;

    return result;
}

//////////////////////////////////////////////////////////////////////////

Joint* Skeleton::FindJoint(const std::string& name)
{
    auto it = joints.find(name);
    if (it != joints.end())
    {
        return it->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

const Joint* Skeleton::FindJoint(const std::string& name) const
{
    auto it = joints.find(name);
    if (it != joints.end())
    {
        return it->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
