#pragma once
#include "FbxLoader.h"
#include "DataTableMgr.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Effect.h"

#define LOCK_MUTEX_RESOURCE(dir, method)\
resource_mutex.lock();\
LoadDir((dir), (method));\
resource_mutex.unlock();\

namespace reality
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
		bool Init(LPCWSTR packagefile);
		bool Init(string directory); 
		void Release();

		using Load_Func = bool(ResourceMgr::*)(string);
		void LoadAllResource();
		void LoadDir(string path, Load_Func load_func);

		template<typename T>
		bool PushResource(string id, string filename);
		template<typename T>
		void PushResource(string id, const T& data);

		template<typename T>
		T* UseResource(string id);

	public:
		map<string, string> GetTotalResID();
		set<string> GetTotalVSID();
		set<string> GetTotalPSID();
		set<string> GetTotalGSID();
		set<string> GetTotalSKMID();
		set<string> GetTotalSTMID();
		set<string> GetTotalANIMID();
		set<string> GetTotalTexID();
		set<string> GetTotalMATID();
		set<string> GetTotalSpriteID();
		set<string> GetTotalEffectID();
		set<string> GetTotalLightID();

	public:
		void PushStaticMesh(string id, const StaticMesh& static_mesh);
		void PushSkeletalMesh(string id, const SkeletalMesh& skeletal_mesh);
		void PushAnimation(const map<string, OutAnimData>& animation);

	private:
		mutex resource_mutex;
	public:
		mutex& GetResourceMutex() { return resource_mutex; }

	private:
		string current_id;

		unordered_map<string, StaticMesh>			resdic_static_mesh;
		unordered_map<string, SkeletalMesh>			resdic_skeletal_mesh;
		unordered_map<string, OutAnimData>			resdic_animation;

		unordered_map<string, VertexShader>			resdic_vs;
		unordered_map<string, PixelShader>			resdic_ps;
		unordered_map<string, GeometryShader>		resdic_gs;
		unordered_map<string, ComputeShader>		resdic_cs;
		unordered_map<string, Texture>				resdic_texture;
		unordered_map<string, Material>				resdic_material;

		// Effect
		unordered_map<string, FMOD::Sound*>			resdic_sound;

		unordered_map<string, shared_ptr<Sprite>>	resdic_sprite;
		unordered_map<string, Effect>				resdic_effect;
		unordered_map<string, shared_ptr<BaseLight>> resdic_light;

	private:
		bool ImportSound(string filename);
		bool ImportMaterial(string filename);
		bool ImportSKM(string filename);
		bool ImportSTM(string filename);
		bool ImportANIM(string filename);
		bool ImportLight(string filename);
	public:
		bool ImportShaders(string filename);
		bool ImportTexture(string filename);
		bool ImportSprite(string filename);
		bool SaveSprite(string name, shared_ptr<Sprite> new_sprite);
		bool ImportEffect(string filename);
		bool SaveEffect(string name, Effect new_effect);

		void	ParseEmitter(DataItem* emitter_data, Emitter& emitter);
		void	ComputeColorTimeline(map<int, XMFLOAT4>& timeline, XMFLOAT4* arr);
		void	ComputeSizeTimeline(map<int, XMFLOAT3>& timeline, XMFLOAT3* arr);
		void	ComputeRotationTimeline(map<int, XMFLOAT3>& timeline, XMFLOAT3* arr);
		void	ComputeVelocityTimeline(map<int, XMFLOAT3>& timeline, XMFLOAT3* arr);

	};

	template<typename T>
	inline void ResourceMgr::PushResource(string id, const T& data)
	{
		if (typeid(T) == typeid(Material))
		{
			resdic_material.insert(make_pair(id, data));
		}
	}

	template<typename T>
	inline T* ResourceMgr::UseResource(string id)
	{
		if (typeid(T) == typeid(StaticMesh))
		{
			auto iter = resdic_static_mesh.find(id);
			if (iter != resdic_static_mesh.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(SkeletalMesh))
		{
			auto iter = resdic_skeletal_mesh.find(id);
			if (iter != resdic_skeletal_mesh.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(OutAnimData))
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
		else if (typeid(T) == typeid(ComputeShader))
		{
			auto iter = resdic_cs.find(id);
			if (iter != resdic_cs.end())
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
		else if (typeid(T) == typeid(Material))
		{
			auto iter = resdic_material.find(id);
			if (iter != resdic_material.end())
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
		else if (typeid(T) == typeid(Sprite))
		{
			auto iter = resdic_sprite.find(id);
			if (iter != resdic_sprite.end())
			{
				return (T*)iter->second.get();
			}
		}
		else if (typeid(T) == typeid(Effect))
		{
			auto iter = resdic_effect.find(id);
			if (iter != resdic_effect.end())
			{
				return (T*)(&iter->second);
			}
		}
		else if (typeid(T) == typeid(BaseLight))
		{
			auto iter = resdic_light.find(id);
			if (iter != resdic_light.end())
			{
				return (T*)iter->second.get();
			}
		}
		return nullptr;
	}
}