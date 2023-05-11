#pragma once
#include "System.h"
#include "ConstantBuffer.h"

struct SequenceInfo
{
	XMVECTOR sequence_start;
	XMVECTOR sequence_end;
	XMVECTOR target_start;
	XMVECTOR target_end;
	float play_time;
	float acceler;
	float decceler;
	int reverse = 0;
};

namespace reality
{
	class DLL_API CameraSystem : public System
	{
	public:
		CameraSystem();
		~CameraSystem();

		void TargetTag(entt::registry& reg, string tag);
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);


		RayShape CreateMouseRay();
		RayShape CreateFrontRay();
		C_Camera* GetCamera();
		XMMATRIX GetViewProj();
		void SetSpeed(float speed) { this->speed = speed; };
		void SetFov(float dgree) { camera->fov = XMConvertToRadians(dgree); };
		void SetFarZ(float z) { camera->far_z = z; };
		void SetNearZ(float z) { camera->near_z = z; };
		XMFLOAT2 ndc;

		void CreateFrustum();
		BoundingFrustum frustum;
		XMMATRIX rotation_matrix;
		XMMATRIX world_matrix;
		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;
		float mouse_sensivity = 0.5f;

		bool enable_control = true;
		bool is_sequence_playing = false;
		bool PlaySequence(SequenceInfo seq_info, float reverse_after);
		bool ZoomToTarget(XMVECTOR target_pos, XMVECTOR zoom_pos, float play_time, float return_after);

	private:
		void DebugCameraMovement();
		void PlayerCameraMovement();

		void CameraAction();
		void CreateMatrix();

	private:
		C_Camera* camera = nullptr;
		D3D11_VIEWPORT* viewport;

		CbCameraInfo	cb_camera_info;
		CbCameraEffect	cb_effect;

		SequenceInfo current_seq_info_;
		XMVECTOR current_target_pos_;
		XMVECTOR current_sequence_pos_;

		float speed = 30;
	};
}
