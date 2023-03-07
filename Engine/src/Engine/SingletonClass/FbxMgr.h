#pragma once
#include "DllMacro.h"
#include "FbxLoader.h"

namespace reality {
	class DLL_API FbxMgr
	{
		SINGLETON(FbxMgr)
#define FBX FbxMgr::GetInst()

	public:
		bool ImportAndSaveFbx(string filename);

	private:
		void SaveLightMesh(const LightMesh& light_mesh, string filename);
		void SaveStaticMesh(const StaticMesh& static_mesh, string filename);
		void SaveSkeletalMesh(const SkeletalMesh& skeletal_mesh, string filename);
		void SaveAnimation(const vector<OutAnimData>& animation, string filename);

	public:
		LightMesh LoadLightMesh(string filename);
		StaticMesh LoadStaticMesh(string filename);
		SkeletalMesh LoadSkeletalMesh(string filename);
		vector<reality::OutAnimData> LoadAnimation(string filename);

	private:
		bool CreateBuffers(SingleMesh<Vertex>& mesh);
		bool CreateBuffers(SingleMesh<LightVertex>& mesh);
		bool CreateBuffers(SingleMesh<SkinnedVertex>& mesh);
	};
}


