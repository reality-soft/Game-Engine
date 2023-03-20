#pragma once

#include "Engine_include.h"

namespace reality
{
	struct Property
	{
		void* value = nullptr;
		size_t type;
	};

	class TestWidget : public reality::GuiWidget
	{
	public:
		XMVECTOR picking_;
	public:
		virtual void Update() override;
		virtual void Render() override;
	public:
		void SetPickingVector(XMVECTOR picking);
	};

	class PropertyWidget : public reality::GuiWidget
	{
	public:
		virtual void Update() override;
		virtual void Render() override;

		template<typename T>
		void AddProperty(string name, T* data)
		{
			Property prop;
			prop.value = (void*)data;
			prop.type = typeid(T).hash_code();

			properties.insert(make_pair(name, prop));
		}

	private:
		std::map<string, Property> properties;
	};
}


