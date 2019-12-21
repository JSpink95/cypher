//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlComputeShader.h
//    Created By    : Jack Spink
//    Created On 	: [6/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ComputeShader.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

class OpenGlComputeShader: public ComputeShader
{
public:
	OpenGlComputeShader(const std::string& shaderpath);
	~OpenGlComputeShader();

	virtual void Disptach(const uint3& workGroups) override;
	virtual void WaitForFinish() override;

private:
	u32 id;
};

//////////////////////////////////////////////////////////////////////////
