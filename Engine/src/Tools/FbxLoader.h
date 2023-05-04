#pragma once
#include "FbxOutData.h"
#include "DllMacro.h"

#ifdef _DEBUG
namespace reality {
	struct FbxImportOption
	{
		float import_scale = 1.0f;
		FbxVector4 import_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		bool recalculate_normal = false;
	};

	enum class FbxVertexOption
	{
		BY_CONTROL_POINT,
		BY_POLYGON_VERTEX
	};

	class DLL_API FbxLoader
	{
	public:
		FbxLoader() = default;
		~FbxLoader() = default;

	public:
		vector<OutMeshData*> out_mesh_list;
		map<string, OutAnimData> out_anim_map;
		FbxImportOption import_options;

	public:
		bool LoadFromFbxFile(string filename);
		void LoadAnimation(FbxTime::EMode time_mode, const string& filename);
		void Destroy();

	private:
		FbxManager* fbx_manager = nullptr;
		FbxImporter* fbx_importer = nullptr;
		FbxScene* fbx_scene = nullptr;
		FbxNode* root_node = nullptr;

		map<FbxNode*, OutMeshData*> out_mesh_map;
		unordered_map<UINT, Bone> skeleton_bone_map;
		unordered_map<UINT, string> skeleton_name_map;
		map<FbxNode*, UINT> node_id_map;
		vector<FbxNode*> node_list;

		FbxAMatrix import_transform;
		FbxAMatrix local_matrix;
	private:
		void PreProcess(FbxNode* fbx_node);
		void ParseMesh(FbxMesh* fbx_mesh, OutMeshData* out_mesh);
		bool ParseMeshSkinning(FbxMesh* fbx_mesh, OutMeshData* out_mesh);
		AnimFrame InitAnimation(int stack_index, FbxTime::EMode time_mode, string& anim_name);

		FbxVector2 ReadUV(FbxMesh* fbx_mesh, FbxLayerElementUV* vertex_uv_set, int pos_index, int uv_index);
		FbxColor ReadColor(FbxMesh* fbx_mesh, FbxLayerElementVertexColor* vertex_color_set, int pos_index, int color_index);
		FbxVector4 ReadNormal(FbxMesh* fbx_mesh, FbxLayerElementNormal* vertex_normal_set, int pos_index, int color_index);

		XMMATRIX FbxToDxConvert(FbxAMatrix& fbx_matrix);
		void CreateMaterialFile(string mesh_name);
		void RefineMeshName(string& mesh_name);
	};

}
#endif