//////////////////////////////////////////////////////////////////////////
//    File        	: Serialiser.cpp
//    Created By    : Jack Spink
//    Created On 	: [03/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/Serialise/Serialiser.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/PropertyRegister.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

#include <windows.h>

//////////////////////////////////////////////////////////////////////////

#define FUNCTION_TRACE "Class::Function"

//////////////////////////////////////////////////////////////////////////

//0 = Black 8 = Gray
//1 = Blue 9 = Light Blue
//2 = Green a = Light Green
//3 = Aqua b = Light Aqua
//4 = Red c = Light Red
//5 = Purple d = Light Purple
//6 = Yellow e = Light Yellow
//7 = White f = Bright White

namespace Console
{
    enum Color: s32
    {
        Black = 0x0,
        Blue = 0x1,
        Green = 0x2,
        Aqua = 0x3,
        Red = 0x4,
        Purple = 0x5,
        Yellow = 0x6,
        White = 0x7,
        Gray = 0x8,
        LightBlue = 0x9,
        LightGreen = 0xa,
        LightAqua = 0xb,
        LightRed = 0xc,
        LightPurple = 0xd,
        LightYellow = 0xe,
        BrightWhite = 0xf
    };

    void SetTextColor(Console::Color color)
    {
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, (s32)color);
    }

    template<typename...Args>
    void Write(const char* fmt, Args... args)
    {
        printf(fmt, args...);
    }
}

//////////////////////////////////////////////////////////////////////////

void Serialise_RTTIObjectToXMLNode(TypeBase* type, RTTIObject* base, pugi::xml_node root)
{
    if (type == nullptr)
    {
#ifdef DEBUG
        Console::SetTextColor(Console::Red);
        Console::Write("[%s] type is null!\n", FUNCTION_TRACE);
#endif
        return;
    }

    for (TypeBase::property_iterator it = type->property_begin(); it != type->property_end(); ++it)
    {
        const std::string propertyname = it->first;
        PropertyBase* prop = it->second;

        if (prop->IsMapProperty())
        {
            Property_MapBase* propMap = (Property_MapBase*)prop;
            const bool isValueRttiObject = propMap->IsValueRTTIObjectType();
            const bool isValueRefType = propMap->IsValueRefType();

            pugi::xml_node mapNode = root.append_child("property");
            mapNode.append_attribute("name").set_value(propertyname.c_str());
            mapNode.append_attribute("type").set_value("map");
            mapNode.append_attribute("count").set_value((u32)propMap->Count(base));

            propMap->ForEachItem(base, [&](const void* k, void* v) -> void
            {
                if (isValueRttiObject)
                {
                    RTTIObject* object = isValueRefType ? ((Ref<RTTIObject>*)v)->get() : (RTTIObject*)v;
                    const std::string objectTypeName = object->GetTypeName();
                    pugi::xml_node objectNode = mapNode.append_child("item");
                    objectNode.append_attribute("key").set_value(propMap->KeyToString(base, k).c_str());
                    objectNode.append_attribute("type").set_value(objectTypeName.c_str());

                    TypeBase* rttiType = TypeRegister::GetRegisteredType(objectTypeName);
                    do {
                        Serialise_RTTIObjectToXMLNode(rttiType, object, objectNode);
                        rttiType = rttiType->GetBaseType();
                    } while (rttiType != nullptr);
                }
                else
                {
                    pugi::xml_node objectNode = mapNode.append_child("item");
                    objectNode.append_attribute("key").set_value(propMap->KeyToString(base, k).c_str());
                    objectNode.append_attribute("value").set_value(propMap->ValueToString(base, k).c_str());
                }
            });
        }
        else if (prop->IsListProperty())
        {
            Property_ListBase* propList = (Property_ListBase*)prop;
            const bool isValueRttiObject = propList->IsValueRTTIObjectType();
            const bool isValueRefType = propList->IsValueRefType();

            pugi::xml_node listNode = root.append_child("property");
            listNode.append_attribute("name").set_value(propertyname.c_str());
            listNode.append_attribute("type").set_value("list");
            listNode.append_attribute("count").set_value((u32)propList->Count(base));

            propList->ForEachItem(base, [&](const s32 index, void* v) -> void
            {
                if (isValueRttiObject)
                {
                    RTTIObject* object = isValueRefType ? ((Ref<RTTIObject>*)v)->get() : (RTTIObject*)v;

                    pugi::xml_node objectNode = listNode.append_child("item");
                    objectNode.append_attribute("index").set_value(index);
                    objectNode.append_attribute("type").set_value(object->GetTypeName().c_str());

                    TypeBase* rttiType = TypeRegister::GetRegisteredType(object->GetTypeName());
                    do {
                        Serialise_RTTIObjectToXMLNode(rttiType, object, objectNode);
                        rttiType = rttiType->GetBaseType();
                    } while (rttiType != nullptr);
                }
                else
                {
                    pugi::xml_node objectNode = listNode.append_child("item");
                    objectNode.append_attribute("index").set_value(index);
                    objectNode.append_attribute("value").set_value(propList->ValueToString(base, index).c_str());
                }
            });
        }
        else if (prop->IsRTTIObjectProperty())
        {
            RTTIObject* object = prop->IsRefType() ? ((Ref<RTTIObject>*)prop->AsVoidPointer(base))->get() : prop->AsRTTIObject(base);

            if (object != nullptr)
            {
                const std::string objectTypeName = object->GetTypeName();
                TypeBase* objectType = TypeRegister::GetRegisteredType(object->GetTypeName());

                pugi::xml_node objectNode = root.append_child("property");
                objectNode.append_attribute("name").set_value(propertyname.c_str());
                objectNode.append_attribute("type").set_value(objectTypeName.c_str());

                do {
                    Serialise_RTTIObjectToXMLNode(objectType, object, objectNode);
                    objectType = objectType->GetBaseType();
                } while (objectType != nullptr);
            }
            else
            {
#ifdef DEBUG
                Console::SetTextColor(Console::Red);
                Console::Write("[%s] failed to find Object %s!\n", FUNCTION_TRACE, propertyname.c_str());
#endif
            }

        }
        else
        {
            pugi::xml_node propertyNode = root.append_child("property");
            propertyNode.append_attribute("name").set_value(propertyname.c_str());
            propertyNode.append_attribute("value").set_value(prop->ToString((void*)base).c_str());
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void Serialise::RTTIObjectToXML(Ref<RTTIObject> object, const std::string& filepath)
{
#ifdef DEBUG
    Console::SetTextColor(Console::BrightWhite);
    Console::Write("[%s] preparing to serialise object to '%s'!\n", FUNCTION_TRACE, object->GetTypeName().c_str());
#endif

    const std::string objectTypeName = object->GetTypeName();
    TypeBase* type = TypeRegister::GetRegisteredType(objectTypeName);
    if (type == nullptr)
    {
#ifdef DEBUG
        Console::SetTextColor(Console::Red);
        Console::Write("[%s] failed to find type %s!\n", FUNCTION_TRACE, objectTypeName.c_str());
#endif
        return;
    }

    pugi::xml_document file;

    pugi::xml_node root = file.append_child("object");
    root.append_attribute("type").set_value(objectTypeName.c_str());

    do {
        Serialise_RTTIObjectToXMLNode(type, object.get(), root);
        type = type->GetBaseType();
    } while (type != nullptr);

    const std::string realpath = FileVolumeManager::GetRealPathFromVirtualPath(filepath).fullpath;
    file.save_file(realpath.c_str());

#if DEBUG
    Console::SetTextColor(Console::BrightWhite);
#endif
}

//////////////////////////////////////////////////////////////////////////

void Deserialise_RTTIObjectFromXmlNode(TypeBase* type, RTTIObject* base, pugi::xml_node root)
{
    for (pugi::xml_node node : root)
    {
        const std::string nodeName = node.name();
        if (nodeName == "property")
        {
            const std::string name = node.attribute("name").as_string();
            PropertyBase* prop = type->FindProperty(name);

            if (prop == nullptr)
            {
#ifdef DEBUG
                Console::SetTextColor(Console::Red);
                Console::Write("[%s] failed to find property %s!\n", FUNCTION_TRACE, name.c_str());
#endif
                continue;
            }

            if (prop->IsMapProperty())
            {
                Property_MapBase* propMap = (Property_MapBase*)prop;

                const bool isValueRttiObject = propMap->IsValueRTTIObjectType();
                const bool isValueRefType = propMap->IsValueRefType();

                for (pugi::xml_node item : node)
                {
                    const std::string key = item.attribute("key").as_string();
                    const std::string itemTypeName = item.attribute("type").as_string();

                    TypeBase* itemType = TypeRegister::GetRegisteredType(itemTypeName);
                    if (itemType == nullptr)
                    {
#ifdef DEBUG
                        Console::SetTextColor(Console::Red);
                        Console::Write("[%s] type doesn't exist %s!\n", FUNCTION_TRACE, itemTypeName.c_str());
#endif
                        continue;
                    }

                    const void* createdKey = propMap->CreateKeyFromString(base, key);

                    if (isValueRttiObject)
                    {
                        RTTIObject* object = (RTTIObject*)itemType->New();
                        Deserialise_RTTIObjectFromXmlNode(itemType, object, item);

                        if (isValueRefType)
                        {
                            Ref<RTTIObject> managedObject;
                            managedObject.reset(object);

                            propMap->EmplaceValue(base, createdKey, &managedObject);
                        }
                        else
                        {
                            propMap->EmplaceValue(base, createdKey, object);
                        }
                    }
                    else
                    {
                        const std::string value = item.attribute("value").as_string();
                        propMap->EmplaceValueFromString(base, createdKey, value);
                    }

                }
            }
            else if (prop->IsListProperty())
            {
                Property_ListBase* propList = (Property_ListBase*)prop;
                const bool isValueRttiObject = propList->IsValueRTTIObjectType();
                const bool isValueRefType = propList->IsValueRefType();

                for (pugi::xml_node item : node)
                {
                    const std::string itemTypeName = item.attribute("type").as_string();

                    TypeBase* itemType = TypeRegister::GetRegisteredType(itemTypeName);
                    if (itemType == nullptr)
                    {
#ifdef DEBUG
                        Console::SetTextColor(Console::Red);
                        Console::Write("[%s] type doesn't exist %s!\n", FUNCTION_TRACE, itemTypeName.c_str());
#endif
                        continue;
                    }


                    if (isValueRttiObject)
                    {
                        RTTIObject* object = (RTTIObject*)itemType->New();
                        Deserialise_RTTIObjectFromXmlNode(itemType, object, item);

                        if (isValueRefType)
                        {
                            Ref<RTTIObject> managedObject;
                            managedObject.reset(object);

                            propList->AddValue(base, &managedObject);
                        }
                        else
                        {
                            propList->AddValue(base, object);
                        }
                    }
                    else
                    {
                        const std::string value = item.attribute("value").as_string();
                        propList->AddValue(base, value);
                    }
                }
#ifdef DEBUG
                Console::SetTextColor(Console::LightYellow);
                Console::Write("[%s] list property not supported yet %s!\n", FUNCTION_TRACE, name.c_str());
#endif
            }
            else if (prop->IsRTTIObjectProperty())
            {
                const std::string finalType = node.attribute("type").as_string("primitive");
                TypeBase* propertyType = finalType != "primitive" ? TypeRegister::GetRegisteredType(finalType) : prop->GetType();

                RTTIObject* object = (RTTIObject*)propertyType->New();
                Deserialise_RTTIObjectFromXmlNode(propertyType, object, node);

                if (prop->IsRefType())
                {
                    Ref<RTTIObject> managedObject;
                    managedObject.reset(object);

                    prop->SetValue(base, &managedObject);
                }
                else
                {
                    prop->SetValue(base, object);
                }
            }
            else
            {
                const std::string value = node.attribute("value").as_string();
                prop->SetValue(base, value);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<RTTIObject> Deserialise::RTTIObjectFromXML(const std::string& filepath)
{
    const std::string realpath = FileVolumeManager::GetRealPathFromVirtualPath(filepath).fullpath;

#ifdef DEBUG

    Console::SetTextColor(Console::Yellow);
    Console::Write("[%s] Loading %s...\n", FUNCTION_TRACE, filepath.c_str());

#endif

    pugi::xml_document file;
    if (!file.load_file(realpath.c_str()))
    {
#ifdef DEBUG
        Console::SetTextColor(Console::Red);
        Console::Write("[%s] Failed to load %s!\n", FUNCTION_TRACE, filepath.c_str());
#endif
        return nullptr;
    }

    pugi::xml_node root = file.root().child("object");

    const std::string typeName = root.attribute("type").as_string();

    TypeBase* type = TypeRegister::GetRegisteredType(typeName);
    if (type == nullptr)
    {
#ifdef DEBUG
        Console::SetTextColor(Console::Red);
        Console::Write("[%s] failed to find type %s!\n", FUNCTION_TRACE, typeName.c_str());
#endif
        return nullptr;
    }

    // create an instance of the rtti object
    RTTIObject* unmanagedObject = (RTTIObject*)type->New();
    Deserialise_RTTIObjectFromXmlNode(type, unmanagedObject, root);

    Ref<RTTIObject> managedObject;
    managedObject.reset(unmanagedObject);

    return managedObject;
}

//////////////////////////////////////////////////////////////////////////

Ref<Object> Deserialise::ObjectFromXML(const std::string& filepath)
{
    Ref<Object> object = std::dynamic_pointer_cast<Object>(RTTIObjectFromXML(filepath));

    object->OnConstruct();

    return object;
}

//////////////////////////////////////////////////////////////////////////
