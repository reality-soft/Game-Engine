#pragma once

namespace reality
{
	enum class GuideType
	{
		eBlocking,
		eNpcTrack,
		eSpawnPoint,
	};

	class DLL_API GuideLine
	{
	public:
		GuideType guide_type_;
		void Init(GuideType guide_type);
		void AddNode(XMVECTOR node_pos);
		void DeleteNode(UINT index);
		void UpdateLines();
		void Import(string mapdat_file, GuideType type);

		string mapdata_dir = "../../Contents/BinaryPackage/";
		map<UINT, XMVECTOR> line_nodes;
		vector<XMVECTOR> line_vectors;
	};
}


