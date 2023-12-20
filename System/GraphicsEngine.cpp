#include"GUI_Util.h"

// windows
#include<d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include<dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

// my lib
#include"System.h"

using namespace System;
using namespace System::GUI;


GraphicsEngine::GraphicsEngine() {}
GraphicsEngine::~GraphicsEngine() {}


Result GraphicsEngine::Init(const ParentWindow& parent)
{


	return Result::SUCCESS;
}

void GraphicsEngine::Draw
(
	const float clearR,
	const float clearG,
	const float clearB,
	const float clearA
)
{
	
}
