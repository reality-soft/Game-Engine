#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "Texture.h"
#include "Collision.h"
#include "Material.h"
#include "UIBase.h"
#include "Sound.h"
#include "Vertex.h"
#include "Effect.h"
#include "DX11App.h"
#include "ResourceMgr.h"
#include "AnimSlot.h"
#include "Socket.h"

namespace reality
{
#define TYPE_ID(type) entt::type_hash<type>().value()

	struct Component
	{
		string tag;
		virtual void OnConstruct() {};
		virtual void OnDestroy() {};
		virtual void OnUpdate() {};
	};

	struct DLL_API C_Transform : public Component
	{
		XMMATRIX local = XMMatrixIdentity();
		XMMATRIX world = XMMatrixIdentity();

		virtual void OnConstruct() override;
	};
  
	struct DLL_API C_Socket : public C_Transform
	{
		unordered_map<string, Socket> sockets;

		void AddSocket(string socket_name, UINT bone_id, XMMATRIX owner_local, XMMATRIX local_offset) {
			sockets.insert({ socket_name, Socket(bone_id, owner_local, local_offset) });
		}
	};

	struct DLL_API C_StaticMesh : public C_Transform
	{
		string static_mesh_id;
		string vertex_shader_id = "StaticMeshVS.cso";
		string socket_name = "";

		virtual void OnConstruct() override {};
	};

	struct DLL_API C_SkeletalMesh : public C_Transform
	{

		string skeletal_mesh_id;
		string vertex_shader_id = "SkinningVS.cso";

		virtual void OnConstruct() override {};
	};

	struct DLL_API C_CapsuleCollision : public C_Transform
	{
		reality::CapsuleShape capsule;
		UINT enclosed_node_index = 0;

		void SetCapsuleData(XMFLOAT3 offset, float height, float radius);
		virtual void OnUpdate() override;
	};

	struct DLL_API C_SphereCollision : public C_Transform
	{
		reality::SphereShape sphere;
		UINT enclosed_node_index = 0;

		bool is_collide;
		XMFLOAT3 tri_normal;

		void SetSphereData(XMFLOAT3 offset, float radius);
		virtual void OnUpdate() override;
	};

	struct DLL_API C_TriggerVolume : public C_Transform
	{
		SphereShape sphere_volume;
		virtual void OnUpdate() override;
	};

	struct DLL_API C_Camera : public C_Transform
	{
		XMVECTOR camera_pos = { 0, 0, 0, 0 };
		XMVECTOR target_pos;
		float target_height = 0.0f;
		XMVECTOR local_pos;
		XMFLOAT2 pitch_yaw = { 0, 0 };
		float roll = 0.0f;
		XMVECTOR look, right, up;
		float rotation = 0.0f, target_rotation = 0.0f;
		float near_z, far_z, fov, aspect;

		bool is_shaking = false;
		float shaking_timer = 0.0f;
		float shake_time = 0.0f;
		float shake_magnitude = 0.0f;
		float shake_frequency = 0.0f;

		virtual void OnConstruct() override {};
		virtual void OnUpdate() override;
		void SetLocalFrom(C_CapsuleCollision& capsule_collision, float arm_length);
	};

	struct DLL_API C_Animation : public Component
	{
		vector<pair<string, AnimSlot>> anim_slots;
		unordered_map<string, int> name_to_anim_slot_index;

		virtual void OnConstruct() override {};
		virtual void OnUpdate() override {};

		AnimSlot GetAnimSlotByName(string anim_slot_name);
		XMMATRIX GetCurAnimMatirixOfBone(int bone_id);

		template<typename AnimationObject, typename... Args>
		inline void SetBaseAnimObject(Args&&...args)
		{
			AnimSlot base_anim_slot;
			base_anim_slot.anim_object_ = make_shared<AnimationObject>(args...);
			anim_slots.push_back({ "Base", base_anim_slot });
			anim_slots[0].second.anim_object_->OnInit();
			name_to_anim_slot_index.insert({ "Base", 0 });
		}

		template<typename AnimSlotType, typename... Args>
		void AddNewAnimSlot(string anim_slot_name, string skeletal_mesh_id, string bone_id, int range, Args&&... args) 
		{
			AnimSlot anim_slot;

			SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skeletal_mesh_id);

			skeletal_mesh->skeleton.GetSubBonesOf(bone_id, range, anim_slot.included_skeletons_, anim_slot.bone_id_to_weight_);
			anim_slot.range_ = range * 2;
			anim_slot.anim_object_ = make_shared<AnimSlotType>(args...);
			anim_slot.anim_object_->OnInit();

			anim_slots.push_back({ anim_slot_name, anim_slot });
			name_to_anim_slot_index.insert({ anim_slot_name, anim_slots.size() - 1 });
		};
	};

	struct DLL_API C_SoundListener : public C_Transform
	{

	};

	struct DLL_API C_SoundGenerator : public C_Transform
	{
		queue<SoundQueue> sound_queue_list;
	};

	struct DLL_API TransformTreeNode
	{
		entt::id_type id_type;
		weak_ptr<TransformTreeNode> parent;
		vector<shared_ptr<TransformTreeNode>> children;

		TransformTreeNode() {};
		TransformTreeNode(entt::id_type type) : id_type(type) {};

		void OnUpdate(entt::registry& registry, entt::entity entity, XMMATRIX world = XMMatrixIdentity());
		void Rotate(entt::registry& registry, entt::entity entity, XMVECTOR rotation_center, XMMATRIX rotation_matrix = XMMatrixIdentity());
		void Translate(entt::registry& registry, entt::entity entity, XMMATRIX world = XMMatrixIdentity());
	};

	struct DLL_API TransformTree
	{
		shared_ptr<TransformTreeNode> root_node;

		bool AddNodeToNode(entt::id_type parent_id_type, entt::id_type child_id_type);
		shared_ptr<TransformTreeNode> FindNode(shared_ptr<TransformTreeNode> parent_node, entt::id_type id_type);
	};

	struct DLL_API C_Movement : public Component
	{
		TransformTree* actor_transform_tree;
		
		XMVECTOR direction = { 0.0f, 0.0f, 0.0f, 0.0f };
		float speed = 0.0f;
		float max_speed = 0.0f;
		float acceleration = 0.0f;
		float rotation = 0.0f;
		float angular_velocity = 0.0f;

		float jump_pulse = 0.0f;
		float gravity_pulse = 0.0f;

		C_Movement(TransformTree* actor_transform_tree) :
			actor_transform_tree(actor_transform_tree) {};

	};

	struct DLL_API C_BoxShape : public C_Transform
	{
		string vs_id;
		string material_id;

		vector<Vertex>			vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;

		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;

		C_BoxShape();
	};

	struct DLL_API C_Effect : public C_Transform
	{
		string	effect_id;
		Effect	effect;
		float	effect_lifetime = -1.0f;
		float	effect_timer;
	};
  
	struct DLL_API C_UI : public Component
	{
		map<string, shared_ptr<UIBase>> ui_list;
	};

	// Light
	struct DLL_API C_PointLight : public C_Transform
	{
		float		timer;
		float		lifetime;
		string		point_light_id;
		PointLight	point_light;
	};

	struct DLL_API C_SpotLight : public C_Transform
	{
		float		timer;
		float		lifetime;
		string		spot_light_id;
		SpotLight	spot_light;
	};
}
