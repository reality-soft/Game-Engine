#include "stdafx.h"
#include "FbxLoader.h"
#include "Material.h"

#ifdef _DEBUG
namespace reality {

	void FbxLoader::Destroy()
	{
		for (auto out_mesh : out_mesh_list)
		{
			delete out_mesh;
			out_mesh = nullptr;
		}
		out_mesh_list.clear();

		out_anim_map.clear();

		for (auto node : node_list)
		{
			node->Destroy();
			node = nullptr;
		}

		if (fbx_scene != nullptr)
		{
			fbx_scene->Destroy();
			fbx_scene = nullptr;
		}

		if (fbx_importer != nullptr)
		{
			fbx_importer->Destroy();
			fbx_importer = nullptr;
		}

		if (fbx_manager != nullptr)
		{
			fbx_manager->Destroy();
			fbx_manager = nullptr;
		}
	}

	bool FbxLoader::LoadFromFbxFile(string filename)
	{
		fbx_manager = FbxManager::Create();
		fbx_importer = FbxImporter::Create(fbx_manager, "");
		fbx_scene = FbxScene::Create(fbx_manager, "");

		if (fbx_importer->Initialize(filename.c_str()) == false)
			return false;

		fbx_importer->Import(fbx_scene);

		FbxAxisSystem axis_system = fbx_scene->GetGlobalSettings().GetAxisSystem();
		FbxSystemUnit system_uint = fbx_scene->GetGlobalSettings().GetSystemUnit();

		double scale_factor = fbx_scene->GetGlobalSettings().GetSystemUnit().GetScaleFactor();
		FbxGeometryConverter converter(fbx_manager);
		//converter.Triangulate(fbx_scene, true);
		root_node = fbx_scene->GetRootNode();
		PreProcess(root_node);

		// 초기 행렬 저장

		// 임포트 설정값
		FbxVector4 scale = { 1.0, 1.0, 1.0, 1.0 };
		scale *= import_options.import_scale;
		FbxVector4 rot = import_options.import_rotation;

		import_transform.SetS(scale);
		import_transform.SetR(rot);

		FbxAMatrix local_matrix = import_transform;
		local_matrix = local_matrix.Inverse();
		local_matrix = local_matrix.Transpose();

		int ID = 0;
		for (auto node : node_list)
		{
			node_id_map.insert(make_pair(node, ID++));
		}
		for (auto cur_node: node_id_map) {
			Bone bone;
			bone.bone_id = node_id_map[cur_node.first];
			int child_count = cur_node.first->GetChildCount();
			for (int child_index = 0; child_index < child_count; child_index++)
			{
				FbxNode* child_node = cur_node.first->GetChild(child_index);
				bone.child_bone_ids.push_back(node_id_map[child_node]);
			}

			skeleton_bone_map.insert({ node_id_map[cur_node.first], bone});
			skeleton_name_map.insert({ cur_node.second, cur_node.first->GetName() });
		}
		for (auto cur_node : node_id_map) {
			UINT node_id = cur_node.second;
			FbxNode* fbx_node = cur_node.first;

			int child_count = cur_node.first->GetChildCount();
			for (int child_index = 0; child_index < child_count; child_index++)
			{
				FbxNode* child_node = cur_node.first->GetChild(child_index);
				Bone& child_bone = skeleton_bone_map[node_id_map[child_node]];
				child_bone.parent_bone_id = node_id;
			}
		}
		for (auto node : node_list)
		{
			FbxMesh* fbx_mesh = node->GetMesh();
			if (fbx_mesh)
			{
				OutMeshData* out_mesh = new OutMeshData;

				ParseMesh(fbx_mesh, out_mesh);
				out_mesh_list.push_back(out_mesh);

				CreateMaterialFile(out_mesh->mesh_name);
			}
		}
		return true;
	}


	void FbxLoader::PreProcess(FbxNode* fbx_node)
	{
		FbxNodeAttribute* attribute = fbx_node->GetNodeAttribute();
		if (attribute)
		{
			switch (attribute->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
			{
				node_list.push_back(fbx_node);
			} break;
			case FbxNodeAttribute::eSkeleton:
			{
				node_list.push_back(fbx_node);
			} break;
			case FbxNodeAttribute::eNull:
			{
				node_list.push_back(fbx_node);
			} break;
			}
		}

		int child_count = fbx_node->GetChildCount();
		for (int child_index = 0; child_index < child_count; child_index++)
		{
			FbxNode* child_node = fbx_node->GetChild(child_index);
			PreProcess(child_node);
		}
	}

	void FbxLoader::ParseMesh(FbxMesh* fbx_mesh, OutMeshData* out_mesh)
	{
		FbxNode* fbx_node = fbx_mesh->GetNode();
		out_mesh->mesh_name = fbx_node->GetName();
		RefineMeshName(out_mesh->mesh_name);

		// 스키닝 정보 확인
		out_mesh->is_skinned = ParseMeshSkinning(fbx_mesh, out_mesh);

		// 초기 행렬 저장
		FbxAMatrix geom;
		FbxVector4 trans = fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 rot = fbx_node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 scale = fbx_node->GetGeometricScaling(FbxNode::eSourcePivot);

		// 임포트 설정값
		scale *= import_options.import_scale;
		rot = import_options.import_rotation;
		geom.SetS(scale);
		geom.SetR(rot);
		geom.SetT(trans);

		FbxAMatrix local_matrix = geom;
		local_matrix = local_matrix.Inverse();
		local_matrix = local_matrix.Transpose();

		// Layer 개념
		FbxLayerElementUV* vertex_uv_layer = nullptr;
		FbxLayerElementNormal* vertex_normal_layer = nullptr;

		FbxLayer* fbx_layer = fbx_mesh->GetLayer(0);
		if (fbx_layer->GetUVs() != nullptr)
		{
			vertex_uv_layer = fbx_layer->GetUVs();
		}
		if (fbx_layer->GetNormals() != nullptr)
		{
			vertex_normal_layer = fbx_layer->GetNormals();
		}

		// 머터리얼 정보
		FbxSurfaceMaterial* surface_material = fbx_node->GetMaterial(0);
		if (surface_material)
		{
			out_mesh->material_name = surface_material->GetName();
		}

		// 정점
		UINT cp_count = fbx_mesh->GetControlPointsCount();
		UINT poly_count = fbx_mesh->GetPolygonCount();

		vector<Vertex> vertices_by_control_point; 
		vertices_by_control_point.resize(cp_count);

		vector<Vertex> vertices_by_polygon_vertex;
		
		vector<SkinnedVertex> skinned_vertices_by_control_point; 
		skinned_vertices_by_control_point.resize(cp_count);

		vector<SkinnedVertex> skinned_vertices_by_polygon_vertex;

		vector<Vertex> control_points; control_points.resize(cp_count);
		for (UINT cp = 0; cp < cp_count; ++cp)
		{
			control_points[cp].p.x = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(cp)).mData[0]);
			control_points[cp].p.y = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(cp)).mData[2]);
			control_points[cp].p.z = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(cp)).mData[1]);
		}

		UINT vertex_counter = 0;
		UINT uv_index = 0;
		UINT cn_index = 0;

		Vertex vertex;
		SkinnedVertex skinned_vertex;
		for (int p = 0; p < poly_count; ++p)
		{
			UINT poly_size = fbx_mesh->GetPolygonSize(p);
			UINT face_count = poly_size - 2;
			for (int f = 0; f < face_count; ++f)
			{
				for (int v = 0; v < 3; ++v)
				{
					UINT vertex_index;
					switch (v)
					{
					case 0:
						vertex_index = (UINT)fbx_mesh->GetPolygonVertex(p, 0);
						uv_index = fbx_mesh->GetTextureUVIndex(p, 0);
						cn_index = 0;
						break;
					case 1:
						vertex_index = (UINT)fbx_mesh->GetPolygonVertex(p, f + 1);
						uv_index = fbx_mesh->GetTextureUVIndex(p, f + 1);
						cn_index = f + 1;
						break;
					case 2: vertex_index = (UINT)fbx_mesh->GetPolygonVertex(p, f + 2);
						uv_index = fbx_mesh->GetTextureUVIndex(p, f + 2);
						cn_index = f + 2;
						break;
					}

					out_mesh->indices.push_back(vertex_index);

					vertex.p.x = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[0]);
					vertex.p.y = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[1]);
					vertex.p.z = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[2]);

					skinned_vertex.p.x = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[0]);
					skinned_vertex.p.y = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[1]);
					skinned_vertex.p.z = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[2]);

					if (vertex_uv_layer)
					{
						FbxVector2 t = ReadUV(fbx_mesh, vertex_uv_layer, vertex_index, uv_index);
						
						vertices_by_control_point[vertex_index].t.x = t.mData[0];
						vertices_by_control_point[vertex_index].t.y = 1.0f - t.mData[1];
					}

					if (vertex_normal_layer)
					{
						FbxVector4 n = ReadNormal(fbx_mesh, vertex_normal_layer, vertex_index, vertex_counter);
						n = local_matrix.MultT(n);
						vertices_by_control_point[vertex_index].n.x = n.mData[0];
						vertices_by_control_point[vertex_index].n.y = n.mData[1];
						vertices_by_control_point[vertex_index].n.z = n.mData[2];
					}

					vertices_by_control_point[vertex_index].p = control_points[vertex_index].p;

					if (out_mesh->is_skinned)
					{
						IndexWeight* index_weight = &out_mesh->index_weight[vertex_index];

						skinned_vertices_by_control_point[vertex_index].i.x = index_weight->index[0];
						skinned_vertices_by_control_point[vertex_index].i.y = index_weight->index[1];
						skinned_vertices_by_control_point[vertex_index].i.z = index_weight->index[2];
						skinned_vertices_by_control_point[vertex_index].i.w = index_weight->index[3];

						skinned_vertices_by_control_point[vertex_index].w.x = index_weight->weight[0];
						skinned_vertices_by_control_point[vertex_index].w.y = index_weight->weight[1];
						skinned_vertices_by_control_point[vertex_index].w.z = index_weight->weight[2];
						skinned_vertices_by_control_point[vertex_index].w.w = index_weight->weight[3];

						skinned_vertices_by_control_point[vertex_index] = vertices_by_control_point[vertex_index];

						skinned_vertex.i.x = index_weight->index[0];
						skinned_vertex.i.y = index_weight->index[1];
						skinned_vertex.i.z = index_weight->index[2];
						skinned_vertex.i.w = index_weight->index[3];

						skinned_vertex.w.x = index_weight->weight[0];
						skinned_vertex.w.y = index_weight->weight[1];
						skinned_vertex.w.z = index_weight->weight[2];
						skinned_vertex.w.w = index_weight->weight[3];

						skinned_vertex = vertices_by_control_point[vertex_index];

					}
					vertex = vertices_by_control_point[vertex_index];
					out_mesh->vertices_by_polygon_vertex.push_back(vertex);
					out_mesh->skinned_vertices_by_polygon_vertex.push_back(skinned_vertex);
					vertex_counter++;
				}
			}
		}

		out_mesh->vertices_by_control_point = vertices_by_control_point;
		out_mesh->skinned_vertices_by_control_point = skinned_vertices_by_control_point;
	}

	FbxVector2 FbxLoader::ReadUV(FbxMesh* fbx_mesh, FbxLayerElementUV* vertex_uv_layer, int vertex_index, int uv_index)
	{
		FbxVector2 t;
		FbxLayerElement::EMappingMode mode = vertex_uv_layer->GetMappingMode();
		switch (mode)
		{
		case FbxLayerElementUV::eByControlPoint:
		{
			switch (vertex_uv_layer->GetReferenceMode())
			{
			case FbxLayerElementUV::eDirect:
			{
				t = vertex_uv_layer->GetDirectArray().GetAt(vertex_index);
			}break;
			case FbxLayerElementUV::eIndexToDirect:
			{
				int index = vertex_uv_layer->GetIndexArray().GetAt(vertex_index);
				t = vertex_uv_layer->GetDirectArray().GetAt(index);
			}break;
			}break;
		} break;
		case FbxLayerElementUV::eByPolygonVertex:
		{
			switch (vertex_uv_layer->GetReferenceMode())
			{
			case FbxLayerElementUV::eDirect:
			case FbxLayerElementUV::eIndexToDirect:
			{
				t = vertex_uv_layer->GetDirectArray().GetAt(uv_index);
				break;
			}
			break;
			}
		}break;
		}
		return t;
	}

	FbxColor FbxLoader::ReadColor(FbxMesh* fbx_mesh, FbxLayerElementVertexColor* vertex_color_layer, int vertex_index, int color_index)
	{
		FbxColor color(1, 1, 1, 1);
		FbxLayerElement::EMappingMode mode = vertex_color_layer->GetMappingMode();
		switch (mode)
		{
		case FbxLayerElementUV::eByControlPoint:
		{
			switch (vertex_color_layer->GetReferenceMode())
			{
			case FbxLayerElementUV::eDirect:
			{
				color = vertex_color_layer->GetDirectArray().GetAt(vertex_index);
			}break;
			case FbxLayerElementUV::eIndexToDirect:
			{
				int index = vertex_color_layer->GetIndexArray().GetAt(vertex_index);
				color = vertex_color_layer->GetDirectArray().GetAt(index);
			}break;
			}break;
		} break;
		case FbxLayerElementUV::eByPolygonVertex:
		{
			switch (vertex_color_layer->GetReferenceMode())
			{
			case FbxLayerElementUV::eDirect:
			{
				color = vertex_color_layer->GetDirectArray().GetAt(color_index);
			}break;
			case FbxLayerElementUV::eIndexToDirect:
			{
				int index = vertex_color_layer->GetIndexArray().GetAt(color_index);
				color = vertex_color_layer->GetDirectArray().GetAt(index);
			}break;
			}break;
		}break;
		}
		return color;
	}

	FbxVector4 FbxLoader::ReadNormal(FbxMesh* fbx_mesh, FbxLayerElementNormal* vertex_normal_layer, int vertex_index, int normal_index)
	{
		FbxVector4 normal(1, 1, 1, 1);
		FbxLayerElement::EMappingMode mode = vertex_normal_layer->GetMappingMode();
		switch (mode)
		{
		case FbxLayerElementUV::eByControlPoint:
		{
			switch (vertex_normal_layer->GetReferenceMode())
			{
			case FbxLayerElementUV::eDirect:
			{
				normal = vertex_normal_layer->GetDirectArray().GetAt(vertex_index);
			}break;
			case FbxLayerElementUV::eIndexToDirect:
			{
				int index = vertex_normal_layer->GetIndexArray().GetAt(vertex_index);
				normal = vertex_normal_layer->GetDirectArray().GetAt(index);
			}break;
			}break;
		} break;
		case FbxLayerElementUV::eByPolygonVertex:
		{
			switch (vertex_normal_layer->GetReferenceMode())
			{
			case FbxLayerElementUV::eDirect:
			{
				normal = vertex_normal_layer->GetDirectArray().GetAt(normal_index);
			}break;
			case FbxLayerElementUV::eIndexToDirect:
			{
				int index = vertex_normal_layer->GetIndexArray().GetAt(normal_index);
				normal = vertex_normal_layer->GetDirectArray().GetAt(index);
			}break;
			}break;
		}break;
		}
		return normal;
	}

	AnimFrame FbxLoader::InitAnimation(int stack_index, FbxTime::EMode time_mode, string &anim_name)
	{
		FbxLongLong s = 0;
		FbxLongLong n = 0;
		FbxTime::SetGlobalTimeMode(time_mode);

		FbxAnimStack* anim_stack = fbx_scene->GetSrcObject<FbxAnimStack>(stack_index);

		AnimFrame frame;

		if (anim_stack)
		{
			fbx_scene->SetCurrentAnimationStack(anim_stack);

			FbxString anim_name_fbx = anim_stack->GetName();
			FbxTakeInfo* take = fbx_scene->GetTakeInfo(anim_name_fbx);

			anim_name = anim_name_fbx;
			vector<string> strs = split(anim_name, '|');
			anim_name = strs[strs.size() - 1];

			FbxTimeSpan localTimeSpan = take->mLocalTimeSpan;
			FbxTime start = localTimeSpan.GetStart();
			FbxTime end = localTimeSpan.GetStop();
			FbxTime Duration = localTimeSpan.GetDirection();
			s = start.GetFrameCount(time_mode);
			n = end.GetFrameCount(time_mode);
		}

		frame.start = s;
		frame.end = n;

		return frame;
	}

	void FbxLoader::LoadAnimation(FbxTime::EMode time_mode, const string& filename)
	{
		// animation
		int anim_count = fbx_scene->GetSrcObjectCount<FbxAnimStack>();

		for (int i = 0; i < anim_count; ++i)
		{
			string anim_name = "";
			AnimFrame anim_frame = InitAnimation(i, time_mode, anim_name);
			anim_name = filename + '_' + anim_name + ".anim";

			OutAnimData out_anim;
			out_anim_map.insert({ anim_name, out_anim });
			out_anim_map[anim_name].start_frame = anim_frame.start;
			out_anim_map[anim_name].end_frame = anim_frame.end;

			for (auto bone : node_id_map)
			{
				vector<XMMATRIX> keyframes;
				FbxTime time;
				for (FbxLongLong t = anim_frame.start; t < anim_frame.end; t++)
				{
					time.SetFrame(t, time_mode);

					AnimTrack track;
					track.frame = t;
					FbxAMatrix fbxMatrix = bone.first->EvaluateGlobalTransform(time);

					//fbxMatrix *= import_transform;

					track.anim_mat = FbxToDxConvert(fbxMatrix);

					XMMatrixDecompose(&track.scaling_vec, &track.rotation_vec, &track.translation_vec, track.anim_mat);
					keyframes.push_back(track.anim_mat);
				}

				out_anim_map[anim_name].animation_matrices.insert(make_pair(bone.second, keyframes));
			}
		}
	}

	XMMATRIX FbxLoader::FbxToDxConvert(FbxAMatrix& fbx_matrix)
	{
		XMMATRIX dx_matrix;
		dx_matrix.r[0] = { (float)fbx_matrix.Get(0, 0), (float)fbx_matrix.Get(0, 2), (float)fbx_matrix.Get(0, 1), 0.0f };
		dx_matrix.r[1] = { (float)fbx_matrix.Get(2, 0), (float)fbx_matrix.Get(2, 2), (float)fbx_matrix.Get(2, 1), 0.0f };
		dx_matrix.r[2] = { (float)fbx_matrix.Get(1, 0), (float)fbx_matrix.Get(1, 2), (float)fbx_matrix.Get(1, 1), 0.0f };
		dx_matrix.r[3] = { (float)fbx_matrix.Get(3, 0), (float)fbx_matrix.Get(3, 2), (float)fbx_matrix.Get(3, 1), 1.0f };

		return dx_matrix;
	}

	void FbxLoader::CreateMaterialFile(string mesh_name)
	{
		string directory = "../../Contents/Material/";

		Material new_material;
		new_material.SaveEmpty(directory + mesh_name + ".mat");
	}

	void FbxLoader::RefineMeshName(string& mesh_name)
	{
		for (auto& word : mesh_name)
		{
			if (word == '.' || word == '|' || word == ' ' ||
				word == '\\' || word == '/' || word == '?' ||
				word == '%' || word == '*' || word == ':' ||
				word == '"' || word == '<' || word == '>')

				word = '_';
		}
	}

	bool FbxLoader::ParseMeshSkinning(FbxMesh* fbx_mesh, OutMeshData* out_mesh)
	{
		int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
		if (deformer_count == 0) return false;

		int vertex_count = fbx_mesh->GetControlPointsCount();
		out_mesh->index_weight.resize(vertex_count);

		FbxDeformer* deformer = fbx_mesh->GetDeformer(0, FbxDeformer::eSkin);
		FbxSkin* fbx_skin = (FbxSkin*)deformer;

		int cluster_count = fbx_skin->GetClusterCount();
		for (int cluster_index = 0; cluster_index < cluster_count; cluster_index++)
		{
			FbxCluster* fbx_cluster = fbx_skin->GetCluster(cluster_index);
			FbxNode* fbx_node = fbx_cluster->GetLink();

			int bone_ID = node_id_map.find(fbx_node)->second;

			// 뼈대공간으로 변환하는 행렬이 필요하다.
			FbxAMatrix cluster_transform;
			FbxAMatrix cluster_link_transform;

			fbx_cluster->GetTransformLinkMatrix(cluster_transform);
			fbx_cluster->GetTransformMatrix(cluster_link_transform);

			FbxAMatrix bind_pose = cluster_link_transform.Inverse() * cluster_transform;
			
			//bind_pose *= import_transform;

			XMMATRIX bind_pose_matrix = FbxToDxConvert(bind_pose);

			// 설정 값에 따라 회전, 스케일 적용
			XMVECTOR scale, rotation, translation;
			XMMatrixDecompose(&scale, &rotation, &translation, bind_pose_matrix);

			scale *= import_options.import_scale;
			XMVECTOR imported_rotation = XMVectorSet(XMConvertToRadians(import_options.import_rotation[0]),
				XMConvertToRadians(import_options.import_rotation[1]),
				XMConvertToRadians(import_options.import_rotation[2]),
				1.0f);

			bind_pose_matrix = XMMatrixScalingFromVector(scale) *
				XMMatrixRotationQuaternion(rotation) * XMMatrixRotationQuaternion(imported_rotation) *
				XMMatrixTranslationFromVector(translation);

			XMVECTOR det;
			bind_pose_matrix = XMMatrixInverse(&det, bind_pose_matrix);
			out_mesh->bind_poses.insert(std::make_pair(bone_ID, bind_pose_matrix));
			out_mesh->name_bone_map.insert(std::make_pair(skeleton_name_map[bone_ID], skeleton_bone_map[bone_ID]));

			// 임의의 1개 정점에 영향을 미치는 뼈대의 개수
			int index_weight_count = fbx_cluster->GetControlPointIndicesCount();
			int* indices = fbx_cluster->GetControlPointIndices();
			double* weights = (double*)fbx_cluster->GetControlPointWeights();
			for (int vertex = 0; vertex < index_weight_count; vertex++)
			{
				int vertex_index = indices[vertex];
				float weight = weights[vertex];
				out_mesh->index_weight[vertex_index].insert(bone_ID, weight);
			}
		}
		return true;
	}
}
#endif