#include "stdafx.h"
#include "FbxLoader.h"
#include "Material.h"

namespace reality {

	void FbxLoader::Destroy()
	{
		for (auto out_mesh : out_mesh_list)
		{
			delete out_mesh;
			out_mesh = nullptr;
		}
		out_mesh_list.clear();

		for (auto out_anim : out_anim_list)
		{
			delete out_anim;
			out_anim = nullptr;
		}
		out_anim_list.clear();

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
		
		converter.Triangulate(fbx_scene, true);
		root_node = fbx_scene->GetRootNode();
		PreProcess(root_node);

		int ID = 0;
		for (auto node : node_list)
		{
			string name = node->GetName();
			skeleton_id_map.insert({ ID, name });
			node_id_map.insert(make_pair(node, ID++));
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
		FbxLayerElementVertexColor* vertex_color_layer = nullptr;
		FbxLayerElementNormal* vertex_normal_layer = nullptr;

		FbxLayer* fbx_layer = fbx_mesh->GetLayer(0);
		if (fbx_layer->GetUVs() != nullptr)
		{
			vertex_uv_layer = fbx_layer->GetUVs();
		}
		if (fbx_layer->GetVertexColors() != nullptr)
		{
			vertex_color_layer = fbx_layer->GetVertexColors();
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

		vector<Vertex> vertices; vertices.resize(cp_count);
		vector<SkinnedVertex> skinned_vertices; skinned_vertices.resize(cp_count);

		vector<Vertex> control_points; control_points.resize(cp_count);
		for (UINT cp = 0; cp < cp_count; ++cp)
		{
			control_points[cp].p.x = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(cp)).mData[0]);
			control_points[cp].p.y = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(cp)).mData[1]);
			control_points[cp].p.z = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(cp)).mData[2]);
		}

		UINT vertex_counter = 0;
		UINT uv_index = 0;
		UINT cn_index = 0;

		LightVertex light_vertex;
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
					light_vertex.p.x = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[0]);
					light_vertex.p.y = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[1]);
					light_vertex.p.z = static_cast<float>(geom.MultT(fbx_mesh->GetControlPointAt(vertex_index)).mData[2]);

					if (vertex_color_layer)
					{
						FbxColor c = ReadColor(fbx_mesh, vertex_color_layer, vertex_index, vertex_counter);
						vertices[vertex_index].c.x = c.mRed;
						vertices[vertex_index].c.y = c.mGreen;
						vertices[vertex_index].c.z = c.mBlue;
						vertices[vertex_index].c.w = 1.0f;
					}
					else { vertices[vertex_index].c = { 1, 1, 1, 1 }; }

					if (vertex_uv_layer)
					{
						FbxVector2 t = ReadUV(fbx_mesh, vertex_uv_layer, vertex_index, uv_index);
						
						vertices[vertex_index].t.x = t.mData[0];
						vertices[vertex_index].t.y = 1.0f - t.mData[1];

						light_vertex.t.x = t.mData[0];
						light_vertex.t.y = 1.0f - t.mData[1];
					}

					if (vertex_normal_layer)
					{
						FbxVector4 n = ReadNormal(fbx_mesh, vertex_normal_layer, vertex_index, vertex_counter);
						n = local_matrix.MultT(n);
						vertices[vertex_index].n.x = n.mData[0];
						vertices[vertex_index].n.y = n.mData[1];
						vertices[vertex_index].n.z = n.mData[2];
					}

					vertices[vertex_index].p = control_points[vertex_index].p;

					if (out_mesh->is_skinned)
					{
						IndexWeight* index_weight = &out_mesh->index_weight[vertex_index];

						skinned_vertices[vertex_index].i.x = index_weight->index[0];
						skinned_vertices[vertex_index].i.y = index_weight->index[1];
						skinned_vertices[vertex_index].i.z = index_weight->index[2];
						skinned_vertices[vertex_index].i.w = index_weight->index[3];

						skinned_vertices[vertex_index].w.x = index_weight->weight[0];
						skinned_vertices[vertex_index].w.y = index_weight->weight[1];
						skinned_vertices[vertex_index].w.z = index_weight->weight[2];
						skinned_vertices[vertex_index].w.w = index_weight->weight[3];

						skinned_vertices[vertex_index] += vertices[vertex_index];
					}
					vertex_counter++;

					out_mesh->light_vertices.push_back(light_vertex);
				}
			}
		}
		out_mesh->vertices = vertices;
		out_mesh->skinned_vertices = skinned_vertices;
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

	AnimFrame FbxLoader::InitAnimation(int stack_index, FbxTime::EMode time_mode)
	{
		FbxLongLong s = 0;
		FbxLongLong n = 0;
		FbxTime::SetGlobalTimeMode(time_mode);

		FbxAnimStack* anim_stack = fbx_scene->GetSrcObject<FbxAnimStack>(stack_index);

		AnimFrame frame;

		if (anim_stack)
		{
			fbx_scene->SetCurrentAnimationStack(anim_stack);

			FbxString anim_name = anim_stack->GetName();
			FbxTakeInfo* take = fbx_scene->GetTakeInfo(anim_name);

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

	void FbxLoader::LoadAnimation(FbxTime::EMode time_mode)
	{
		// animation
		int anim_count = fbx_scene->GetSrcObjectCount<FbxAnimStack>();

		for (int i = 0; i < anim_count; ++i)
		{
			AnimFrame anim_frame = InitAnimation(i, time_mode);

			OutAnimData* out_anim = new OutAnimData;

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
					track.anim_mat = FbxToDxConvert(fbxMatrix);

					// 설정 값에 따라 회전, 스케일 적용
					XMVECTOR scale, rotation, translation;
					XMMatrixDecompose(&scale, &rotation, &translation, track.anim_mat);

					scale *= import_options.import_scale;
					XMVECTOR imported_rotation = XMVectorSet(XMConvertToRadians(import_options.import_rotation[0]),
						XMConvertToRadians(import_options.import_rotation[1]),
						XMConvertToRadians(import_options.import_rotation[2]),
						1.0f);

					track.anim_mat = XMMatrixScalingFromVector(scale) *
						XMMatrixRotationQuaternion(rotation) * XMMatrixRotationQuaternion(imported_rotation) *
						XMMatrixTranslationFromVector(translation);

					XMMatrixDecompose(&track.scaling_vec, &track.rotation_vec, &track.translation_vec, track.anim_mat);
					keyframes.push_back(track.anim_mat);
				}

				out_anim->animations.insert(make_pair(bone.second, keyframes));
			}

			out_anim->start_frame = anim_frame.start;
			out_anim->end_frame = anim_frame.end;
			out_anim_list.push_back(out_anim);
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
			out_mesh->skelton_id_map.insert(std::make_pair(skeleton_id_map[bone_ID], bone_ID));

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