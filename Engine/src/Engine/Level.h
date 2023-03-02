#pragma once
#include "Components.h"
#include "DllMacro.h"
#include "DataTypes.h"
#include "ResourceMgr.h"
#include "InstancedObject.h"

#define LerpByTan(start, end, tan) (start - (start * tan) + (end * tan))

namespace KGCA41B
{
	class DLL_API SkySphere
	{
	public:
		SkySphere();
		~SkySphere();

	public:
		bool CreateSphere();
		void FrameRender(const C_Camera* camera);

		XMFLOAT4 skycolor_afternoon;
		XMFLOAT4 skycolor_noon;
		XMFLOAT4 skycolor_night;
		FLOAT color_strength;

		CbSkySphere cb_sky;

	private:
		void FrameBackgroundSky(const C_Camera* camera);
		void RenderBackgroundSky();

		void FrameSunStarSky(const C_Camera* camera);
		void RenderSunStarSky();

		void FrameCloudSky(const C_Camera* camera);
		void RenderCloudSky();

	private:
		shared_ptr<StaticMesh> sphere_mesh;
		shared_ptr<StaticMesh> cloud_dome;
		shared_ptr<VertexShader> vs;
		CbTransform cb_transform;
	};

	class DLL_API Level
	{
	public:
		Level() = default;
		~Level();

	public:
		// Import
		bool ImportFromFile(string filepath);

	public:
		// Create
		bool CreateLevel(UINT _max_lod, UINT _cell_scale, UINT _uv_scale, XMINT2 _row_col_blocks);
		void SetCamera(C_Camera* _camera);
		void Update();
		void Render(bool culling);

	public:
		// Getter
		float GetHeightAt(float x, float y);
		XMVECTOR GetNormalAt(float x, float y);
		XMINT2 GetWorldSize();
		XMINT2 GetBlocks();
		UINT MaxLod();
		vector<Vertex> GetLevelVertex() { return level_mesh_.vertices; }
		vector<UINT> GetLevelIndex() { return level_mesh_.indices; }
		reactphysics3d::CollisionBody* GetCollisionBody() { return height_field_body_; }

	public:
		string vs_id_;
		string ps_id_;
		vector<string> texture_id;
		C_Camera* camera = nullptr;

	public:
		vector<InstancedObject> inst_objects;
		SkySphere sky_sphere;

	protected:
		void GenVertexNormal();
		void GetHeightList();
		XMFLOAT2 GetMinMaxHeight();
		XMFLOAT3 GetNormal(UINT i0, UINT i1, UINT i2);
		bool CreateBuffers();
		bool CreateHeightField(float min_height, float max_height);

		void RenderObjects();
		void RenderSkySphere();

	protected:
		SingleMesh<Vertex> level_mesh_;
		vector<float> height_list_;
		ComPtr<ID3D11SamplerState> mip_map_sample;

		UINT num_row_vertex_;
		UINT num_col_vertex_;
		float cell_distance_;

		UINT max_lod_;
		UINT cell_scale_;
		XMINT2 row_col_blocks_;
		float uv_scale_;


	protected:
		reactphysics3d::HeightFieldShape* height_field_shape_ = nullptr;
		reactphysics3d::Collider* height_field_collider_ = nullptr;
		reactphysics3d::CollisionBody* height_field_body_ = nullptr;
	};
}