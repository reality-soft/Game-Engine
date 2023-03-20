#pragma once

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

		map<UINT, XMVECTOR> line_nodes;
		vector<XMVECTOR> line_vectors;
	};
}


