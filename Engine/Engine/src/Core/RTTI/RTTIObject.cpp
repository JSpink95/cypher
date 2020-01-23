//////////////////////////////////////////////////////////////////////////
//    File        	: RTTIObject.cpp
//    Created By    : Jack Spink
//    Created On 	: [22/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTIObject.h"
#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN(RTTIObject)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    //////////////////////////////////////////////////////////////////////////

    void DisplayProperty(RTTIObject* object, PropertyBase* prop)
    {
        if (prop->IsRTTIObjectProperty())
        {
            RTTIObject* propertyObject = (*(Ref<RTTIObject>*)prop->AsVoidPointer((void*)object)).get();
            DisplayObjectProperties(prop->propertyName, propertyObject);
        }
        else if (prop->IsListProperty())
        {
            Property_ListBase* propAsList = dynamic_cast<Property_ListBase*>(prop);
            const bool valueIsRef = propAsList->IsValueRefType();

            if (ImGui::TreeNode(prop->propertyName.c_str()))
            {
                auto predicate = [&](void* value) -> void
                {

                };

                propAsList->ForEachItem(object, predicate);

                ImGui::TreePop();
            }
        }
        else if (prop->IsMapProperty())
        {
            Property_MapBase* propAsMap = dynamic_cast<Property_MapBase*>(prop);
            TypeBase* valueType = propAsMap->GetValueType();
            const bool valueIsRef = propAsMap->IsValueRefType();

            if (ImGui::TreeNode(prop->propertyName.c_str()))
            {
                auto predicate = [&](const void* key, void* value) -> void
                {
                    if (valueIsRef && valueType)
                    {
                        Ref<RTTIObject>* object = (Ref<RTTIObject>*)value;
                        if (object)
                        {
                            DisplayObjectProperties((*object)->GetTypeName(), object->get());
                        }
                    }
                };

                propAsMap->ForEachItem(object, predicate);

                ImGui::TreePop();
            }
        }
        else
        {
            prop->DisplayEditBox((void*)object);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    void DisplayObjectProperties(const std::string& objectName, RTTIObject* object)
    {
        if (object != nullptr)
        {
            TypeBase* type = TypeRegister::GetRegisteredType(object->GetTypeName());
            if (type == nullptr)
            {
                ImGui::Text("No type information for %s", object->GetTypeName());
                return;
            }

            if (ImGui::TreeNode(objectName.c_str()))
            {
                do {
                    for (TypeBase::property_iterator it = type->property_begin(); it != type->property_end(); ++it)
                    {
                        DisplayProperty(object, it->second);
                    }
                } while (type = type->GetBaseType());

                ImGui::TreePop();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////
