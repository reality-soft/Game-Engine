#include "Weapon.h"
#include "Engine_include.h"
using namespace reality;

void Weapon::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	reality::C_StaticMesh stm;
	stm.local = XMMatrixIdentity();
	stm.world = XMMatrixIdentity();
	stm.static_mesh_id = "";
	stm.vertex_shader_id = "StaticMeshVS.cso";
	registry.emplace_or_replace<reality::C_StaticMesh>(entity_id_, stm);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(C_StaticMesh));

	transform_matrix_ = XMMatrixTranslation(0, 100, 0);
	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);
}

void Weapon::OnUpdate()
{
	Character* owner_character = SCENE_MGR->GetActor<Character>(entity_id_);

	if (owner_character == nullptr) {
		return;
	}
	transform_matrix_ = owner_character->GetTransformMatrix();

	C_Animation* animation_component = reg_scene_->try_get<C_Animation>(owner_character->GetEntityId());

	if (animation_component != nullptr && socket_id_ != -1) {
		OutAnimData* anim_data = RESOURCE->UseResource<OutAnimData>(animation_component->anim_id);
		
		animation_matrix_ = anim_data->animations[socket_id_][animation_component->cur_frame];
	}


	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, socket_offset_ * animation_matrix_ * transform_matrix_);
}

void Weapon::SetOwnerId(entt::entity owner_id)
{
	owner_ = owner_id;
}

void Weapon::SetSocket(int socket_id)
{
	socket_id_ = socket_id;
}
