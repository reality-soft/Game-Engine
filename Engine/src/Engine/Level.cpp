#include "stdafx.h"
#include "Level.h"
#include "DX11App.h"
#include "ResourceMgr.h"
#include "PhysicsMgr.h"
#include "InputMgr.h"
#include "FileTransfer.h"
using namespace KGCA41B;


bool KGCA41B::Level::ImportFromFile(string filepath)
{
	FileTransfer file_transfer(filepath, READ);

	int data1_size = file_transfer.ReadBinary<int>(1)[0];
	vector<Vertex> data1 = file_transfer.ReadBinary<Vertex>(data1_size);
	int data2_size = file_transfer.ReadBinary<int>(1)[0];
	vector<UINT> data2 = file_transfer.ReadBinary<UINT>(data2_size);
	int data3_size = file_transfer.ReadBinary<int>(1)[0];
	vector<string> data3 = file_transfer.ReadBinary<string>(data3_size);

	return true;
}

bool Level::CreateLevel(UINT num_row, UINT num_col, int cell_distance, int uv_scale)
{
	level_transform_.data.world_matrix = XMMatrixIdentity();
	level_light_.data.light_direction = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	level_light_.data.light_bright = 1.5f;

    num_row_vertex_ = num_row;
    num_col_vertex_ = num_col;
    UINT num_row_cell = num_row_vertex_ - 1;
    UINT num_col_cell = num_col_vertex_ - 1;
    int num_half_row = num_row_vertex_ / 2;
    int num_half_col = num_col_vertex_ / 2;
	cell_distance_ = cell_distance;
	uv_scale_ = uv_scale;

    level_mesh_.vertices.resize(num_row_vertex_ * num_col_vertex_);

	for (int r = 0; r < num_row_vertex_; ++r)
	{
		for (int c = 0; c < num_col_vertex_; ++c)
		{
			UINT index = r * num_row_vertex_ + c;

			level_mesh_.vertices[index].p.x = (float)(c - num_half_row) * cell_distance_;
			level_mesh_.vertices[index].p.y = 0.0f;
			level_mesh_.vertices[index].p.z = (float)(num_half_col - r) * cell_distance_;

			level_mesh_.vertices[index].c = { 1, 1, 1, 1 };

			level_mesh_.vertices[index].t.x = (float)c / (float)(num_row_cell) * uv_scale_;
			level_mesh_.vertices[index].t.y = (float)r / (float)(num_col_cell) * uv_scale_;
		}
	}

	level_mesh_.indices.resize(num_row_cell * num_col_cell * 6.0f);
	UINT index = 0;
	for (UINT r = 0; r < num_row_cell; ++r)
	{
		for (UINT c = 0; c < num_col_cell; ++c)
		{
			UINT next_row = r + 1;
			UINT next_col = c + 1;

			level_mesh_.indices[index + 0] = r * num_row_vertex_ + c;
			level_mesh_.indices[index + 1] = r * num_row_vertex_ + c + 1;
			level_mesh_.indices[index + 2] = next_row * num_row_vertex_ + c;
			level_mesh_.indices[index + 3] = next_row * num_row_vertex_ + c;
			level_mesh_.indices[index + 4] = r * num_row_vertex_ + c + 1;
			level_mesh_.indices[index + 5] = next_row * num_row_vertex_ + c + 1;

			index += 6;
		}
	}
	
	GenVertexNormal();

	if (CreateBuffers() == false)
		return false;

    return true;
}

bool Level::CreateHeightField(float min_height, float max_height)
{
	GetHeightList();


	height_field_shape_ = PHYSICS->physics_common_.createHeightFieldShape(
		num_col_vertex_,// * cell_distance_,
		num_row_vertex_,// * cell_distance_,
		min_height,
		max_height,
		height_list_.data(),
		reactphysics3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);

	if (height_field_shape_ == nullptr)
		return false;

	height_field_shape_->setScale(Vector3(cell_distance_, 1, cell_distance_));

	reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
	height_field_body_ = PHYSICS->GetPhysicsWorld()->createCollisionBody(transform);
	height_field_collider_ = height_field_body_->addCollider(height_field_shape_, transform);

	return true;
}

void Level::Update()
{
	// Set VS Cb : Transform
	level_transform_.data.world_matrix = XMMatrixTranspose(level_transform_.data.world_matrix);
	DX11APP->GetDeviceContext()->UpdateSubresource(level_transform_.buffer.Get(), 0, nullptr, &level_transform_.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(0, 1, level_transform_.buffer.GetAddressOf());

	// Set PS Cb : Light
	DX11APP->GetDeviceContext()->UpdateSubresource(level_light_.buffer.Get(), 0, nullptr, &level_light_.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(0, 1, level_light_.buffer.GetAddressOf());
}

void Level::Render(bool culling)
{
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(vs_id_);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(ps_id_);


	{ // Textures Stage
		UINT slot = 0;
		for (auto id : texture_id)
		{
			Texture* texture = RESOURCE->UseResource<Texture>(id);
			if (texture != nullptr)
			{
				DX11APP->GetDeviceContext()->PSSetShaderResources(slot++, 1, texture->srv.GetAddressOf());
			}
		}
	}

	{ // Samplers Stage
		DX11APP->GetDeviceContext()->PSSetSamplers(0, 1, mip_map_sample.GetAddressOf());
	}

	{ // Input Assembly Stage
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, level_mesh_.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayoyt());
	}

	{ // Set Shader Stage
		DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);
		DX11APP->GetDeviceContext()->PSSetShader(ps->Get(), 0, 0);
	}

	// Draw Call
	if (culling == false)
	{
		DX11APP->GetDeviceContext()->IASetIndexBuffer(level_mesh_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(level_mesh_.indices.size(), 0, 0);
	}
}

XMINT2 KGCA41B::Level::GetWorldSize()
{
	return XMINT2(num_row_vertex_ - 1, num_col_vertex_ - 1);
}


void Level::GenVertexNormal()
{
	vector<XMFLOAT3> face_normals;
	face_normals.resize(level_mesh_.indices.size());
	UINT face = 0;

	for (UINT i = 0; i < level_mesh_.indices.size(); i += 3)
	{
		UINT i0 = level_mesh_.indices[i + 0];
		UINT i1 = level_mesh_.indices[i + 1];
		UINT i2 = level_mesh_.indices[i + 2];

		XMFLOAT3 normal = GetNormal(i0, i1, i2);
		level_mesh_.vertices[i0].n = normal;
		level_mesh_.vertices[i1].n = normal;
		level_mesh_.vertices[i2].n = normal;
	}
}

float Level::GetHeightAt(float x, float z)
{
	float cell_x = (float)((num_row_vertex_ - 1) * cell_distance_ / 2.0f + x);
	float cell_z = (float)((num_col_vertex_ - 1) * cell_distance_ / 2.0f - z);

	cell_x /= (float)cell_distance_;
	cell_z /= (float)cell_distance_;

	float vertex_x = floorf(cell_x);
	float vertex_z = floorf(cell_z);

	if (vertex_x < 0.f)  vertex_x = 0.f;
	if (vertex_z < 0.f)  vertex_z = 0.f;
	if ((float)(num_row_vertex_ - 2) < vertex_x)	vertex_x = (float)(num_row_vertex_ - 2);
	if ((float)(num_col_vertex_ - 2) < vertex_z)	vertex_z = (float)(num_col_vertex_ - 2);

	float A = level_mesh_.vertices[(int)vertex_x *     num_row_vertex_ + (int)vertex_z    ].p.y;
	float B = level_mesh_.vertices[(int)vertex_x *     num_row_vertex_ + (int)vertex_z + 1].p.y;
	float C = level_mesh_.vertices[(int)vertex_x + 1 * num_row_vertex_ + (int)vertex_z    ].p.y;
	float D = level_mesh_.vertices[(int)vertex_x + 1 * num_row_vertex_ + (int)vertex_z + 1].p.y;

	float delta_x = cell_x - vertex_x;
	float delta_z = cell_z - vertex_z;
	float height = 0.0f;

	if (delta_z < (1.0f - delta_x))
	{
		float uy = B - A;
		float vy = C - A;	
		
		height = A - LerpByTan(0.0f, uy, delta_x) + LerpByTan(0.0f, vy, delta_z);
	}
	else
	{
		float uy = C - D;
		float vy = B - D;
		height = A - LerpByTan(0.0f, uy, 1.0f - delta_x) + LerpByTan(0.0f, vy, 1.0f - delta_z);
	}
	
	return height;
}

void Level::GetHeightList()
{
	// If you need every '1' unit's interpolated height... (Using GetHeightAt())
	{
		//int num_row = static_cast<int>(num_row_vertex_) * cell_distance_;
		//int num_col = static_cast<int>(num_col_vertex_) * cell_distance_;
		//for (int r = -(num_row / 2); r < num_row / 2; ++r)
		//{
		//	for (int c = -(num_col / 2); c < num_col / 2; ++c)
		//	{
		//		height_list_.push_back(GetHeightAt(r, c));
		//	}
		//}
	}

	for (auto vertex : level_mesh_.vertices)
	{
		height_list_.push_back(vertex.p.y);
	}
}

XMFLOAT3 Level::GetNormal(UINT i0, UINT i1, UINT i2)
{
	XMFLOAT3 normal;
	XMVECTOR e0 = XMLoadFloat3(&level_mesh_.vertices[i1].p) - XMLoadFloat3(&level_mesh_.vertices[i0].p);
	XMVECTOR e1 = XMLoadFloat3(&level_mesh_.vertices[i2].p) - XMLoadFloat3(&level_mesh_.vertices[i0].p);
	XMStoreFloat3(&normal, XMVector3Normalize(XMVector3Cross(e0, e1)));

	return normal;
}

bool Level::CreateBuffers()
{
	HRESULT hr;

	// VertexBuffer

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(Vertex) * level_mesh_.vertices.size();
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	subdata.pSysMem = level_mesh_.vertices.data();

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, level_mesh_.vertex_buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	// IndexBuffer

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(UINT) * level_mesh_.indices.size();
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subdata.pSysMem = level_mesh_.indices.data();

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, level_mesh_.index_buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	// ConstantBuffer : Transform

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbTransform::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &level_transform_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, level_transform_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	// ConstantBuffer : Light

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbLight::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &level_light_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, level_light_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	// sampler
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MinLOD = -3.0f;
	sampler_desc.MaxLOD =  3.0f;

	hr = DX11APP->GetDevice()->CreateSamplerState(&sampler_desc, mip_map_sample.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}
