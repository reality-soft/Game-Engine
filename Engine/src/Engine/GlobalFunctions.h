#pragma once
static std::wstring to_mw(const std::string& _src)
{
	USES_CONVERSION;
	return std::wstring(A2W(_src.c_str()));
}

static std::string to_wm(const std::wstring& _src)
{
	USES_CONVERSION;
	return std::string(W2A(_src.c_str()));
}

static std::vector<std::string> split(std::string input, char delimiter) {
	std::vector<std::string> answer;
	std::stringstream ss(input);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}

static void XMtoRP(DirectX::XMVECTOR& xmv, reactphysics3d::Vector3& rpv)
{
	rpv.x = xmv.m128_f32[0];
	rpv.y = xmv.m128_f32[1];
	rpv.z = xmv.m128_f32[2];
}

static void XMtoRP(DirectX::XMVECTOR& xmv, reactphysics3d::Vector2& rpv)
{
	rpv.x = xmv.m128_f32[0];
	rpv.y = xmv.m128_f32[1];
}

static void RPtoXM(reactphysics3d::Vector3& rpv, DirectX::XMVECTOR& xmv)
{
	xmv.m128_f32[0] = rpv.x;
	xmv.m128_f32[1] = rpv.y;
	xmv.m128_f32[2] = rpv.z;
}

static void RPtoXM(reactphysics3d::Vector2& rpv, DirectX::XMVECTOR& xmv)
{
	xmv.m128_f32[0] = rpv.x;
	xmv.m128_f32[1] = rpv.y;
}

static float RandomFloat()
{
	srand(time(NULL));

	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

static float RandomFloatInRange(float min, float max)
{
	srand(time(NULL));

	float scaled = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	return min + (scaled * (max - min));
}

static DirectX::XMVECTOR RandomPointInBox(const DirectX::XMVECTOR& center, const DirectX::XMVECTOR& half_size)
{
	srand(time(NULL));

	DirectX::XMFLOAT3 center_float, half_size_float;
	DirectX::XMStoreFloat3(&center_float, center);
	DirectX::XMStoreFloat3(&half_size_float, half_size);

	float x = center_float.x + (2.0f * half_size_float.x * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - half_size_float.x);
	float y = center_float.y + (2.0f * half_size_float.y * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - half_size_float.y);
	float z = center_float.z + (2.0f * half_size_float.z * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) - half_size_float.z);

	return DirectX::XMVectorSet(x, y, z, 1.0f);
}

static DirectX::XMVECTOR RandomVectorInCone(DirectX::XMVECTOR direction, float cone_angle)
{
	srand((time(NULL)));

	float radians = DirectX::XMConvertToRadians(cone_angle);
	// Generate a random point on the unit circle in the xz-plane

	float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * DirectX::XM_2PI;
	float x = std::cos(angle);
	float z = std::sin(angle);

	// Compute the axis and angle of rotation from the x-axis to the direction vector
	DirectX::XMVECTOR xAxis = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR axis = DirectX::XMVector3Cross(xAxis, direction);
	float angleFromX = std::acos(DirectX::XMVectorGetX(DirectX::XMVector3Dot(xAxis, direction)));

	// Rotate the random point around the direction vector by a random angle within the cone angle
	float coneAngle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * radians;
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(axis, coneAngle);
	DirectX::XMVECTOR point = DirectX::XMVectorSet(x, 0.0f, z, 1.0f);
	point = DirectX::XMVector3Transform(point, rotation);

	// Transform the random point to world space and normalize it
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(point, DirectX::XMMatrixRotationY(angleFromX)));

	return normal;
}

static float Distance(DirectX::XMVECTOR p1, DirectX::XMVECTOR p2)
{
	// Subtract the 'from' point from the 'to' point to get the vector
	DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(p1, p2);	
	return DirectX::XMVector3Length(vector).m128_f32[0];
}

static DirectX::XMFLOAT4 LerpColor(DirectX::XMFLOAT4 start_color, DirectX::XMFLOAT4 end_color, float key)
{
	DirectX::XMFLOAT4 lerped_color = start_color;

	lerped_color.x += (end_color.x - start_color.x) * key;
	lerped_color.y += (end_color.y - start_color.y) * key;
	lerped_color.z += (end_color.z - start_color.z) * key;
	lerped_color.w = 1.0f;

	return lerped_color;
}