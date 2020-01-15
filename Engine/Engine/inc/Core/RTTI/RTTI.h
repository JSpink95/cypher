//////////////////////////////////////////////////////////////////////////
//    File        	: RTTI.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/PropertyRegister.h"
#include "Core/RTTI/TypeRegister.h"

//////////////////////////////////////////////////////////////////////////

#define RTTI_BEGIN(Class)                                                                                                   \
static AutoTypeRegister<Class> RTTI_##Class;

//////////////////////////////////////////////////////////////////////////

#define RTTI_PROPERTY(Class, PropertyType, Property)                                                                        \
static AutoPropertyRegister<Class, PropertyType> RTTI_##Class##_##Property(#Property, offsetof(Class, Property));

//////////////////////////////////////////////////////////////////////////

#define RTTI_PROPERTY_WITH_NOTIFY(Class, PropertyType, Property)                                                            \
static AutoPropertyRegisterWithNotify<Class, PropertyType> RTTI_##Class##_##Property(#Property, offsetof(Class, Property));

//////////////////////////////////////////////////////////////////////////

#define RTTI_END()

//////////////////////////////////////////////////////////////////////////
