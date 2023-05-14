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

static int RandomIntInRange(int min_, int max_)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(min(min_, max_), max(min_, max_));

	return dis(gen);
}

static float RandomFloatInRange(float min_, float max_)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min(min_, max_), max(min_, max_));

	return dis(gen);
}

static void RandomMarginFloat(float& f, float margin, bool up, bool down)
{
	if (up)
	{
		f += RandomFloatInRange(0, margin);
	}
	else if (down)
	{
		f -= RandomFloatInRange(0, margin);
	}
	else
	{
		f += RandomFloatInRange(-margin / 2, margin / 2);
	}
}

static bool Probability(int percentage)
{
	int r = RandomIntInRange(0, 99);
	return r < percentage;
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
	return DirectX::XMVectorMultiply(DirectX::XMVectorDivide(OB, DirectX::XMVector3LengthSq(OB)), DirectX::XMVector3Dot(OA, OB));
}

static DirectX::XMVECTOR VectorProjectPlane(DirectX::XMVECTOR vector, DirectX::XMVECTOR plane_normal)
{
	DirectX::XMVECTOR proj = DirectX::XMVectorMultiply(DirectX::XMVector3Dot(vector, plane_normal), plane_normal);
	return DirectX::XMVectorSubtract(vector, proj);
}

static bool IsParallelVector(const DirectX::XMVECTOR& vector1, const DirectX::XMVECTOR& vector2)
{
	DirectX::XMVECTOR normalized1 = DirectX::XMVector3Normalize(vector1);
	DirectX::XMVECTOR normalized2 = DirectX::XMVector3Normalize(vector2);
		
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(normalized1, normalized2));

	return DirectX::XMScalarNearEqual(dot, 1.f, 0.001f) || DirectX::XMScalarNearEqual(dot, -1.f, 0.001f);
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

static DirectX::XMMATRIX TransformS(DirectX::XMFLOAT3 sacling)
{
	return DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&sacling));
}
static DirectX::XMMATRIX TransformR(DirectX::XMFLOAT3 roation)
{
	DirectX::XMMATRIX rotation = DirectX::XMMatrixIdentity();
	rotation *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(roation.x));
	rotation *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(roation.y));
	rotation *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(roation.z));

	return rotation;
}
static DirectX::XMMATRIX TransformT(DirectX::XMFLOAT3 position)
{
	return DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position));
}

static float FadeInAlpha(float start, float end, float cur_time)
{
	float alpha = 0.0f;

	float fi_s, fi_e;
	fi_s = start;
	fi_e = end;

	if (fi_s <= cur_time && cur_time <= fi_e) // fade_in
	{
		alpha = (fi_s - cur_time) / (fi_s - fi_e);
	}
	else if (fi_e <= cur_time)
	{
		alpha = 1.0f;
	}

	return alpha;
}

static float FadeOutAlpha(float start, float end, float cur_time)
{
	float alpha = 1.0f;

	float fo_s, fo_e;
	fo_s = start;
	fo_e = end;

	if (fo_s <= cur_time && cur_time <= fo_e) // fade_in
	{
		alpha = 1.0f - (fo_s - cur_time) / (fo_s - fo_e);
	}
	else if (fo_e <= cur_time)
	{
		alpha = 0.0f;
	}

	return alpha;
}


static float FadeAlpha(float start, float end, float fade_in, float fade_out, float cur_time)
{
	float alpha = 0.0f;

	float fi_s, fi_e, fo_s, fo_e;
	fi_s = start;
	fi_e = fi_s + fade_in;
	fo_s = end - fade_out;
	fo_e = end;

	if (fi_s <= cur_time && cur_time <= fi_e) // fade_in
	{
		alpha = cur_time / fade_in;
	}
	else if (fo_s <= cur_time && cur_time <= fo_e) // fade_out
	{
		alpha = 1.0f - (cur_time - fo_s) / fade_out;
	}
	else if (fi_e <= cur_time && cur_time <= fo_s)
	{
		alpha = 1.0f;
	}

	return alpha;
}