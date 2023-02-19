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
	private:
		string directory_;
	public:
		string directory() { return directory_; }
		void set_directory(string dir) { directory_ = dir; }
	public:
		bool Init(LPCWSTR packagefile); // 통합 패키지 파일을 불러오고 없으면 빈 값
		bool Init(string directory); 
		void Release();

		using Load_Func = bool(ResourceMgr::*)(string);
		void LoadAllResource();
		void LoadDir(string path, Load_Func load_func);

		template<typename T>
		bool PushResource(string id, string filename);
		template<typename T>
		T* UseResource(string id);

		map<string, string> GetTotalResID();

		set<string> GetTotalTexID();
		set<string> GetTotalVSID();
		set<string> GetTotalPSID();
		set<string> GetTotalGSID();
		set<string> GetTotalSKMID();
		set<string> GetTotalSKID();
		set<string> GetTotalSTMID();
		set<string> GetTotalANIMID();

	private:
		string current_id;

		map<string, vector<SingleMesh<Vertex>>> resdic_static_mesh;
		map<string, vector<SingleMesh<SkinnedVertex>>> resdic_skeletal_mesh;
		map<string, map<UINT, XMMATRIX>> resdic_skeleton;
		map<string, vector<OutAnimData>> resdic_animation;

		map<string, VertexShader> resdic_vs;
		map<string, PixelShader>  resdic_ps;
		map<string, GeometryShader> resdic_gs;
		map<string, Texture> resdic_texture;

		map<string, FMOD::Sound*>	resdic_sound;

	private:
		bool ImportFbx(string filename);
		bool ImportShaders(string filename);
		bool ImportSound(string filename);	
		bool ImportTexture(string filename);

		bool CreateBuffers(SingleMesh<Vertex>& mesh);
		bool CreateBuffers(SingleMesh<SkinnedVertex>& mesh);

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
		else if (typeid(T) == typeid(VertexShader))
		{
			auto iter = resdic_vs.find(id);
			if (iter != resdic_vs.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(PixelShader))
		{
			auto iter = resdic_ps.find(id);
			if (iter != resdic_ps.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(GeometryShader))
		{
			auto iter = resdic_gs.find(id);
			if (iter != resdic_gs.end())
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
		else if (typeid(T) == typeid(FMOD::Sound))
		{
			auto iter = resdic_sound.find(id);
			if (iter != resdic_sound.end())
			{
				return (T*)iter->second;
			}
		}
		return nullptr;
	}
}