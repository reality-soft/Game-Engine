# 23-02-23
###  라이팅 시스템
- 글로벌적인 라이트에 대한 요소는 오브젝트마다 라이브 방향을 상수버퍼에 넘겨줄 필요가 없으므로 라이팅 시스템에서 한번만 넘겨줍니다. 이것으로 전체적인 태양광 조명 구현이 가능합니다. 물론, 모든 픽셀 쉐이더엔 0번 슬롯의 상수버퍼를 다음과 같이 정의해야 합니다.
```c++
cbuffer cb_light : register(b0)
{
	float4 direction;
	float4 color;
}
```

### 범용적인 레이캐스팅
- 기존에는 레벨에만 따로 레벨 피킹을 구현하였으나 이제는 피직스 매니저에 요청하고 받는 식으로 수정하였습니다. WorldPicking 과 ObjectPicking 두가지를 구현 했습니다.
```c++
WorldRayCallback KGCA41B::PhysicsMgr::WorldPicking(const MouseRay& mouse_ray)
{
	WorldRayCallback raycast_callback;
	Ray ray(mouse_ray.start_point, mouse_ray.end_point);
	physics_world_->raycast(ray, &raycast_callback);

	return raycast_callback;
}

bool KGCA41B::PhysicsMgr::ObjectPicking(const MouseRay& mouse_ray, CollisionBody* target)
{
	Ray ray(mouse_ray.start_point, mouse_ray.end_point);
	RaycastInfo ray_info;
	target->raycast(ray, ray_info);
	if (ray_info.body == target)
		return true;

	return false;
}
```
- 월드 피킹은 콜백 캐스트를 리턴받습니다. 이 클래스엔 히트포인트, 히트가 걸린 바디가 들어있습니다.
- 오브젝트 피킹은 부울값을 리턴받는데, 인자로 넘겨준 바디포인터에 한해서 입니다.
