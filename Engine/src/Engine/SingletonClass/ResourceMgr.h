#pragma once
#include "DllMacro.h"
#include "Components.h"
#include "FbxLoader.h"
#include "Texture.h"

namespace KGCA41B
{
	class DLL_API ResourceMgr
	{
		SINGLETON(ResourceMgr)
#define RESOURCE ResourceMgr::GetInst()
	public:
		string directory;

	public:
		bool Init(string packagefile); // 통합 패키지 파일을 불러오고 없으면 빈 값
		void Release();

		template<typename T>
		bool PushResource(string id, string filename);
		template<typename T>
		T* UseResource(string id);

	private:
		string current_id;

		map<string, vector<SingleMesh<Vertex>>> resdic_static_mesh;
		map<string, vector<SingleMesh<SkinnedVertex>>> resdic_skeletal_mesh;
		map<string, map<UINT, XMMATRIX>> resdic_skeleton;
		map<string, vector<OutAnimData>> resdic_animation;

		map<string, VsDefault> resdic_vs_default;
		map<string, VsSkinned> resdic_vs_skinned;
		map<string, PsDefault> resdic_ps_default;
		map<string, Texture> resdic_texture;

		bool ImportFbx(string filename);

		bool CreateVertexBuffers(SingleMesh<Vertex>& mesh);
		bool CreateVertexBuffers(SingleMesh<SkinnedVertex>& mesh);

		bool ImportVsDefault(string filename);
		bool ImportVsSkinned(string filename);
		bool ImportPsDefault(string filename);
		bool ImportTexture(string filename);
	};




	template<typename T>
	inline bool ResourceMgr::PushResource(string id, string filename)
	{
		current_id = id;
		bool result = false;

		if (typeid(T) == typeid(FbxLoader))
		{
			result = ImportFbx(directory + filename);
		}
		else if (typeid(T) == typeid(VsDefault))
		{
			result = ImportVsDefault(directory + filename);
		}
		else if (typeid(T) == typeid(VsSkinned))
		{
			result = ImportVsSkinned(directory + filename);
		}
		else if (typeid(T) == typeid(PsDefault)) 
		{
			result = ImportPsDefault(directory + filename);
		}
		else if (typeid(T) == typeid(Texture))
		{
			result = ImportTexture(directory + filename);
		}

		return result;
	}

	template<typename T>
	inline T* ResourceMgr::UseResource(string id)
	{
		if (typeid(T) == typeid(vector<SingleMesh<Vertex>>))
		{
			auto iter = resdic_static_mesh.find(id);
			if (iter != resdic_static_mesh.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(vector<SingleMesh<SkinnedVertex>>))
		{
			auto iter = resdic_skeletal_mesh.find(id);
			if (iter != resdic_skeletal_mesh.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(map<UINT, XMMATRIX>))
		{
			auto iter = resdic_skeleton.find(id);
			if (iter != resdic_skeleton.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(vector<OutAnimData>))
		{
			auto iter = resdic_animation.find(id);
			if (iter != resdic_animation.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(VsDefault))
		{
			auto iter = resdic_vs_default.find(id);
			if (iter != resdic_vs_default.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(VsSkinned))
		{
			auto iter = resdic_vs_skinned.find(id);
			if (iter != resdic_vs_skinned.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(PsDefault))
		{
			auto iter = resdic_ps_default.find(id);
			if (iter != resdic_ps_default.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(Texture))
		{
			auto iter = resdic_texture.find(id);
			if (iter != resdic_texture.end())
			{
				return (T*)(&iter->second);
			}
		}
		return nullptr;
	}
}