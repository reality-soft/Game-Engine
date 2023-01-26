#pragma once

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnRelease() = 0;
};

