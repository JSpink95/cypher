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

#define RTTI_BEGIN_WITH_BASE(Class, Base)                                                                                   \
static AutoTypeRegisterWithBase<Class, Base> RTTI_##Class;                                                                   

//////////////////////////////////////////////////////////////////////////

#define RTTI_PROPERTY(Class, PropertyType, PropertyName)                                                                    \
static AutoPropertyRegister<Class, PropertyType> RTTI_##Class##_##PropertyName(                                             \
    #PropertyName,                                                                                                          \
    offsetof(Class, PropertyName)                                                                                           \
);

//////////////////////////////////////////////////////////////////////////

#define RTTI_PROPERTY_LIST(Class, ListType, ListValueType, PropertyName)                                                    \
static AutoPropertyRegister_List<Class, ListType, ListValueType> RTTI_##Class##_##PropertyName(                             \
    #PropertyName,                                                                                                          \
    offsetof(Class, PropertyName),                                                                                          \
    #ListValueType                                                                                                          \
);

#define RTTI_PROPERTY_MAP(Class, MapType, MapKeyType, MapValueType, PropertyName)                                           \
static AutoPropertyRegister_Map<Class, MapType, MapKeyType, MapValueType> RTTI_##Class##_##PropertyName(                    \
    #PropertyName,                                                                                                          \
    offsetof(Class, PropertyName),                                                                                          \
    #MapKeyType,                                                                                                            \
    #MapValueType                                                                                                           \
);

//////////////////////////////////////////////////////////////////////////

#define RTTI_END()

//////////////////////////////////////////////////////////////////////////
