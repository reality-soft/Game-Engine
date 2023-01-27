#include "TestGame.h"

using namespace KGCA41B;

void TestGame::OnInit()
{
	FbxLoader fbx_loader;

	string filepath = "G:/Repos/Game-Engine/RumbaDancing.fbx";
	if (fbx_loader.LoadFromFbxFile(filepath))
	{
		int mesh_count = fbx_loader.out_mesh_list.size();
		int vertex_count = 0;
		int bind_pose_count = 0;
		for (auto mesh : fbx_loader.out_mesh_list)
		{
			vertex_count += mesh->skinned_vertices.size();
			bind_pose_count += mesh->bind_poses.size();
		}

		OutputDebugString(L"========================================================\n");
		OutputDebugString(L"FBX 메쉬 로드 성공\n");
		OutputDebugString((L"메쉬 갯수 : " + to_wstring(mesh_count) + L"\n").c_str());
		OutputDebugString((L"총 정점 갯수 : " + to_wstring(vertex_count) + L"\n").c_str());
		OutputDebugString((L"바인드 포즈 갯수 : " + to_wstring(bind_pose_count) + L"\n").c_str());

		fbx_loader.LoadAnimation(FbxTime::eFrames60);

		int anim_count = fbx_loader.out_anim_list.size();
		int frame_count = fbx_loader.out_anim_list[0]->end_frame;

		OutputDebugString(L"FBX 애니메이션 로드 성공\n");
		OutputDebugString((L"애니메이션 갯수 : " + to_wstring(anim_count) + L"\n").c_str());
		OutputDebugString((L"첫 번째 애니메이션 프레임 수 : " + to_wstring(frame_count) + L"\n").c_str());

	}
	fbx_loader.Destroy();
}

void TestGame::OnUpdate()
{
}

void TestGame::OnRender()
{
}

void TestGame::OnRelease()
{
}
