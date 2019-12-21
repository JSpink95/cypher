//////////////////////////////////////////////////////////////////////////
//    File        	: ApiManager.cpp
//    Created By    : Jack Spink
//    Created On 	: [2/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"

#ifdef _OPENGL
    #include "Render/OpenGL/OpenGlApiManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _DX11
    #include "Render/Dx11/Dx11ApiManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

void ApiManager::Create()
{
    if (instance != nullptr)
    {
        // attempted to create another instance of ApiManager when one already existed!
        return;
    }

#if defined _OPENGL
    instance = std::make_shared<OpenGlApiManager>();
#elif defined _DX11
    instance = new Dx11ApiManager;
#else
    instance = new NullApiManager;
#endif

}

//////////////////////////////////////////////////////////////////////////

void ApiManager::Delete()
{
    if (instance == nullptr)
    {
        // instance is null!
        return;
    }

    instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////
