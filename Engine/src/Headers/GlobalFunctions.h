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

static std::string VectorToString(DirectX::XMVECTOR& arg)
{
	std::string str;
	str =  "\nX : " + std::to_string(DirectX::XMVectorGetX(arg));
	str += "\nY : " + std::to_string(DirectX::XMVectorGetY(arg));
	str += "\nZ : " + std::to_string(DirectX::XMVectorGetZ(arg));

	return str;
}

static std::string VectorToString(DirectX::XMFLOAT3& arg)
{
	std::string str;
	str =  "\nX : " + std::to_string(arg.x);
	str += "\nY : " + std::to_string(arg.y);
	str += "\nZ : " + std::to_string(arg.z);
	
	return str;
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

static float Vector3Length(DirectX::XMVECTOR vector)
{
	return DirectX::XMVectorGetX(DirectX::XMVector3Length(vector));
}

static DirectX::XMVECTOR Vector3Project(DirectX::XMVECTOR OB, DirectX::XMVECTOR OA)
{
	return DirectX::XMVectorMultiply(OB, DirectX::XMVectorDivide(DirectX::XMVectorMultiply(OA, OB), DirectX::XMVector3LengthSq(OB)));
}

static bool IsParallelVector(const DirectX::XMVECTOR& vector1, const DirectX::XMVECTOR& vector2)
{
	// 정규화된 벡터를 사용하여 평행 여부를 확인합니다.
	DirectX::XMVECTOR normalized1 = DirectX::XMVector3Normalize(vector1);
	DirectX::XMVECTOR normalized2 = DirectX::XMVector3Normalize(vector2);

	// 두 벡터의 내적(dot product)을 계산합니다.
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(normalized1, normalized2));

	// 내적이 1 또는 -1에 근접하면 두 벡터는 평행합니다.
	return DirectX::XMScalarNearEqual(dot, 1.f, 0.001f) || DirectX::XMScalarNearEqual(dot, -1.f, 0.001f);
}

#define randf(x) (x*rand()/(float)RAND_MAX)
#define randf2(x,off) (off+x*rand()/(float)RAND_MAX)
#define randstep(fMin,fMax) (fMin+((float)fMax-(float)fMin)*rand()/(float)RAND_MAX)
#define clamp(x,MinX,MaxX) if (x>MaxX) x=MaxX; else if (x<MinX) x=MinX;

static DirectX::XMFLOAT4 LerpColor(DirectX::XMFLOAT4 start_color, DirectX::XMFLOAT4 end_color, float key)
{
	DirectX::XMFLOAT4 lerped_color = start_color;

	lerped_color.x += (end_color.x - start_color.x) * key;
	lerped_color.y += (end_color.y - start_color.y) * key;
	lerped_color.z += (end_color.z - start_color.z) * key;
	lerped_color.w = 1.0f;

	return lerped_color;
}

static DirectX::XMMATRIX TransformS(DirectX::XMFLOAT3& sacling)
{
	return DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&sacling));
}
static DirectX::XMMATRIX TransformR(DirectX::XMFLOAT3& roation)
{
	DirectX::XMMATRIX rotation = DirectX::XMMatrixIdentity();
	rotation *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(roation.x));
	rotation *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(roation.y));
	rotation *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(roation.z));

	return rotation;
}
static DirectX::XMMATRIX TransformT(DirectX::XMFLOAT3& position)
{
	return DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position));
}