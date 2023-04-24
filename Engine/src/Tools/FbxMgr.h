#pragma once
#include "FbxLoader.h"
#include "Mesh.h"

namespace reality {

	class DLL_API FbxMgr
	{
		SINGLETON(FbxMgr)
#define FBX FbxMgr::GetInst()

	public:
#ifdef _DEBUG
		bool ImportAndSaveFbx(string filename, FbxImportOption options = FbxImportOption(), FbxVertexOption vertex_option = FbxVertexOption::BY_CONTROL_POINT);
#endif

	private:
		void SaveStaticMesh(const StaticMesh& static_mesh, string filename);
		void SaveSkeletalMesh(const SkeletalMesh& skeletal_mesh, string filename);
		void SaveAnimation(const map<string, OutAnimData>& animation);

	public:
		StaticMesh LoadStaticMesh(string filename);
		SkeletalMesh LoadSkeletalMesh(string filename);
		reality::OutAnimData LoadAnimation(string filename);

	private:
		bool CreateBuffers(SingleMesh<Vertex>& mesh);
		bool CreateBuffers(SingleMesh<SkinnedVertex>& mesh);
#ifdef _DEBUG
		void ReCalculateNormal(StaticMesh& static_mesh, FbxVertexOption vertex_option);
		void ReCalculateNormal(SkeletalMesh& skeletal_mesh, FbxVertexOption vertex_option);
#endif
	};
}


