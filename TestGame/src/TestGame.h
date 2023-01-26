#pragma once
#include <Engine_include.h>

class TestGame : public Scene
{
private:
	//FbxLoader fbx_loader;

public:
	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnRelease() override;
};