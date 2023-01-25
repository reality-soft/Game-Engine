#include "FbxLoader.h"

void FbxLoader::Destroy()
{
	for (auto out_mesh : out_mesh_list)
	{
		delete out_mesh;
		out_mesh = nullptr;
	}

	for (auto node : node_list)
	{
		node->Destroy();
		node = nullptr;
	}

	if (fbx_scene)
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

	FbxAxisSystem SceneAxisSystem = fbx_scene->GetGlobalSettings().GetAxisSystem();

	FbxSystemUnit::m.ConvertScene(fbx_scene);

	//FbxAxisSystem::DirectX.ConvertScene(pFbxScene);
	//FbxGeometryConverter converter(pFbxManager);
	//converter.Triangulate(pFbxScene, true);

	root_node = fbx_scene->GetRootNode();
	PreProcess(root_node);

	int ID = 0;
	for (auto node : node_list)
	{
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
		}
	}

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

	// 스키닝 정보 확인
	out_mesh->is_skinned = ParseMeshSkinning(fbx_mesh, out_mesh);

	// 초기 행렬 저장
	FbxAMatrix geom;
	FbxVector4 trans = fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 rot = fbx_node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 scale = fbx_node->GetGeometricScaling(FbxNode::eSourcePivot);

	geom.SetT(trans);
	geom.SetR(rot);
	geom.SetS(scale);

	FbxAMatrix local_matrix = geom;
	local_matrix = local_matrix.Inverse();
	local_matrix = local_matrix.Transpose();
	out_mesh->fbx_local_matrix = local_matrix;

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


	int polygon_count = fbx_mesh->GetPolygonCount();
	int face_count = 0;
	int polygon_index = 0;
	int iSubMtrl = 0;

	FbxVector4* control_point = fbx_mesh->GetControlPoints();
	for (int poly = 0; poly < polygon_count; poly++)
	{
		int poly_size = fbx_mesh->GetPolygonSize(poly);
		face_count = poly_size - 2;

		for (int face = 0; face < face_count; face++)
		{
			// 정점컬러인덱스
			int vertex_color[3] = { 0, face + 2, face + 1 };

			// 정점인덱스
			int vertex_index[3];
			vertex_index[0] = fbx_mesh->GetPolygonVertex(poly, 0);
			vertex_index[1] = fbx_mesh->GetPolygonVertex(poly, face + 2);
			vertex_index[2] = fbx_mesh->GetPolygonVertex(poly, face + 1);

			int uv_index[3];
			uv_index[0] = fbx_mesh->GetTextureUVIndex(poly, 0);
			uv_index[1] = fbx_mesh->GetTextureUVIndex(poly, face + 2);
			uv_index[2] = fbx_mesh->GetTextureUVIndex(poly, face + 1);

			for (int index = 0; index < 3; index++)
			{
				int vertex_ID = vertex_index[index];
				FbxVector4 v = geom.MultT(control_point[vertex_ID]);

				Vertex vertex;
				SkinnedVertex skinned_vertex;

				vertex.p.x = v.mData[0];
				vertex.p.y = v.mData[2];
				vertex.p.z = v.mData[1];
				vertex.c = XMFLOAT4(1, 1, 1, 1);

				if (vertex_color_layer)
				{
					FbxColor c = ReadColor(
						fbx_mesh,
						vertex_color_layer,
						vertex_index[index],
						polygon_index + vertex_color[index]);
					vertex.c.x = c.mRed;
					vertex.c.y = c.mGreen;
					vertex.c.z = c.mBlue;
					vertex.c.w = 1.0f;
				}

				if (vertex_uv_layer)
				{
					FbxVector2 t = ReadUV(
						fbx_mesh,
						vertex_uv_layer,
						vertex_index[index],
						uv_index[index]);
					vertex.t.x = t.mData[0];
					vertex.t.y = 1.0f - t.mData[1];
				}

				if (vertex_normal_layer)
				{
					FbxVector4 n = ReadNormal(
						fbx_mesh,
						vertex_normal_layer,
						vertex_index[index],
						polygon_index + vertex_color[index]);
					n = local_matrix.MultT(n);
					vertex.n.x = n.mData[0];
					vertex.n.y = n.mData[2];
					vertex.n.z = n.mData[1];
				}

				if (out_mesh->is_skinned)
				{
					IndexWeight* index_weight = &out_mesh->index_weight[vertex_ID];
					skinned_vertex.i.x = index_weight->index[0];
					skinned_vertex.i.y = index_weight->index[1];
					skinned_vertex.i.z = index_weight->index[2];
					skinned_vertex.i.w = index_weight->index[3];
					skinned_vertex.w.x = index_weight->weight[0];
					skinned_vertex.w.y = index_weight->weight[1];
					skinned_vertex.w.z = index_weight->weight[2];
					skinned_vertex.w.w = index_weight->weight[3];

					skinned_vertex += vertex;
					out_mesh->skinned_vertices.push_back(skinned_vertex);
				}
				else
				{
					out_mesh->vertices.push_back(vertex);
				}
			}
		}
		polygon_index += poly_size;
	}
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
		}break;
		}break;
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
	XMMATRIX dx_matrix = 
	{
		(float)fbx_matrix.Get(0, 0), (float)fbx_matrix.Get(1, 0), (float)fbx_matrix.Get(2, 0), (float)fbx_matrix.Get(3, 0),
		(float)fbx_matrix.Get(0, 1), (float)fbx_matrix.Get(1, 1), (float)fbx_matrix.Get(2, 1), (float)fbx_matrix.Get(3, 1),
		(float)fbx_matrix.Get(0, 2), (float)fbx_matrix.Get(1, 2), (float)fbx_matrix.Get(2, 2), (float)fbx_matrix.Get(3, 2),
		(float)fbx_matrix.Get(0, 3), (float)fbx_matrix.Get(1, 3), (float)fbx_matrix.Get(2, 3), (float)fbx_matrix.Get(3, 3)
	};
	dx_matrix = XMMatrixTranspose(dx_matrix);
	return dx_matrix;
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

		fbx_cluster->GetTransformMatrix(cluster_transform);
		fbx_cluster->GetTransformLinkMatrix(cluster_link_transform);

		FbxAMatrix bind_pose = cluster_transform.Inverse() * cluster_link_transform;

		XMMATRIX bind_pose_matrix = FbxToDxConvert(bind_pose);
		bind_pose_matrix = XMMatrixInverse(nullptr, bind_pose_matrix);
		out_mesh->bind_poses.insert(std::make_pair(bone_ID, bind_pose_matrix));


		// 임의의 1개 정점에 영향을 미치는 뼈대의 개수
		int index_weight_count = fbx_cluster->GetControlPointIndicesCount();
		int* indices = fbx_cluster->GetControlPointIndices();
		float* weights = (float*)fbx_cluster->GetControlPointWeights();
		for (int vertex = 0; vertex < index_weight_count; vertex++)
		{
			int vertex_index = indices[vertex];
			float weight = weights[vertex];
			out_mesh->index_weight[vertex_index].insert(bone_ID, weight);
		}		
	}
	return true;
}
