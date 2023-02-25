#pragma once
#include "Engine_include.h"
#include "Surface.h"

using namespace KGCA41B;

class SurfaceDefiner : public GuiWidget
{
public:
	virtual void Update() override;
	virtual void Render() override;

public:
	Surface test_surface;
};

