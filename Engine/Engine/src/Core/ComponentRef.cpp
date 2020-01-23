//////////////////////////////////////////////////////////////////////////
//    File        	: ComponentRef.cpp
//    Created By    : Jack Spink
//    Created On 	: [16/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ComponentRefBase, RTTIObject)
    RTTI_PROPERTY(ComponentRefBase, std::string, componentName)
RTTI_END()

//////////////////////////////////////////////////////////////////////////
