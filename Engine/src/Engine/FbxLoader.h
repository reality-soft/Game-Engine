#pragma once
#include "FbxOutData.h"

class FbxLoader
{
private:
	FbxManager* fbx_manager;
	FbxImporter* fbx_importer;
	FbxScene* fbx_scene;
	FbxNode* root_node;

	map<FbxNode*, OutMeshData*> out_mesh_map;
	map<FbxNode*, UINT> node_id_map;
	vector<FbxNode*> node_list;

public:
	vector<OutMeshData*> out_mesh_list;
	vector<OutAnimData*> out_anim_list;

public:
	bool LoadFromFbxFile(string filename);
	void LoadAnimation(FbxTime::EMode time_mode);
	void Destroy();

private:
	void PreProcess(FbxNode* fbx_node);
	void ParseMesh(FbxMesh* fbx_mesh, OutMeshData* out_mesh);
	bool ParseMeshSkinning(FbxMesh* fbx_mesh, OutMeshData* out_mesh);
	AnimFrame InitAnimation(int stack_index, FbxTime::EMode time_mode);

	FbxVector2 ReadUV(FbxMesh* fbx_mesh, FbxLayerElementUV* vertex_uv_set, int pos_index, int uv_index);
	FbxColor ReadColor(FbxMesh* fbx_mesh, FbxLayerElementVertexColor* vertex_color_set, int pos_index, int color_index);
	FbxVector4 ReadNormal(FbxMesh* fbx_mesh, FbxLayerElementNormal* vertex_normal_set, int pos_index, int color_index);

	XMMATRIX FbxToDxConvert(FbxAMatrix& fbx_matrix);
};						

