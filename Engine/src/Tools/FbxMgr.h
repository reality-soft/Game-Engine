#pragma once
#include "FbxLoader.h"
#include "Mesh.h"

namespace reality {

	class DLL_API FbxMgr
	{
		SINGLETON(FbxMgr)
#define FBX FbxMgr::GetInst()

	public:
		bool ImportAndSaveFbx(string filename, FbxImportOption options = FbxImportOption());

	private:
		void SaveLightMesh(const LightMesh& light_mesh, string filename);
		void SaveStaticMesh(const StaticMesh& static_mesh, string filename);
		void SaveSkeletalMesh(const SkeletalMesh& skeletal_mesh, string filename);
		void SaveAnimation(const map<string, OutAnimData>& animation);

	public:
		LightMesh LoadLightMesh(string filename);
		StaticMesh LoadStaticMesh(string filename);
		SkeletalMesh LoadSkeletalMesh(string filename);
		reality::OutAnimData LoadAnimation(string filename);

	private:
		bool CreateBuffers(SingleMesh<Vertex>& mesh);
		bool CreateBuffers(SingleMesh<LightVertex>& mesh);
		bool CreateBuffers(SingleMesh<SkinnedVertex>& mesh);
	};
}


