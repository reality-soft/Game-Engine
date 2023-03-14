#pragma once
#include "DllMacro.h"
#include "DataTypes.h"
#include "Shader.h"
#include "InstancedObject.h"

namespace reality
{
	class DLL_API GuideLine
	{
	public:
		enum class GuideType
		{
			eBlocking,
			eNpcTrack,
		} guide_type_;

	public:
		void Init(GuideType guide_type);
		void AddNode(XMVECTOR node_pos);
		void DeleteNode(UINT index);

	private:
		map<UINT, XMVECTOR> line_nodes;
		vector<XMVECTOR> line_vectors;
	};
}


