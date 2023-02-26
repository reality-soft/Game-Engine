#pragma once
#include "DllMacro.h"
#include "FbxLoader.h"

namespace KGCA41B {
	class DLL_API FbxMgr
	{
		SINGLETON(FbxMgr)
#define FBX FbxMgr::GetInst()

	public:
		bool ImportAndSaveFbx(string filename);

	private:
		void SaveStaticMesh(const StaticMesh& static_mesh, string filename);
		void SaveSkeletalMesh(const SkeletalMesh& skeletal_mesh, string filename);
		void SaveAnimation(const vector<OutAnimData>& animation, string filename);

	public:
		StaticMesh LoadStaticMesh(string filename);
		SkeletalMesh LoadSkeletalMesh(string filename);
		vector<KGCA41B::OutAnimData> LoadAnimation(string filename);

	private:
		bool CreateBuffers(SingleMesh<Vertex>& mesh);
		bool CreateBuffers(SingleMesh<SkinnedVertex>& mesh);
	};
}


