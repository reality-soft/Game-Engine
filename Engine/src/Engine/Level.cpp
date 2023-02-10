#include "Level.h"
#include "DX11App.h"
#include "ResourceMgr.h"

using namespace KGCA41B;

KGCA41B::Level::Level()
	: num_row_vertex_(0)
	, num_col_vertex_(0)
	, cell_distance_(0)
	, uv_scale_(0)
	, max_height_(0)
	, device_(nullptr)
	, device_context_(nullptr)
{
	level_transform_.data.world_matrix = XMMatrixIdentity();
	level_light_.data.light_direction = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	level_light_.data.light_bright = 1.0f;

	device_ = DX11APP->GetDevice();
	device_context_ = DX11APP->GetDeviceContext();
}

bool Level::CreateLevel(UINT num_row, UINT num_col, float cell_distance, float uv_scale)
{
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

	if (CreateBuffers() == false)
		return false;

    return true;
}

void KGCA41B::Level::Update()
{
	// Set Cb : Transform
	level_transform_.data.world_matrix = XMMatrixTranspose(level_transform_.data.world_matrix);

	device_context_->UpdateSubresource(level_transform_.buffer.Get(), 0, nullptr, &level_transform_.data, 0, 0);
	device_context_->VSSetConstantBuffers(0, 1, level_transform_.buffer.GetAddressOf());

	// Set Cb : Light
	device_context_->UpdateSubresource(level_light_.buffer.Get(), 0, nullptr, &level_light_.data, 0, 0);
	device_context_->PSSetConstantBuffers(0, 1, level_light_.buffer.GetAddressOf());
}

void Level::Render()
{
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(vs_id_);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(ps_id_);

	// Set Shader : PS
	device_context_->PSSetShader(ps->Get(), 0, 0);

	ID3D11SamplerState* sampler = DX11APP->GetCommonStates()->LinearWrap();
	device_context_->PSSetSamplers(0, 1, &sampler);

	UINT slot = 0;
	for (auto id : texture_id)
	{
		Texture* texture = RESOURCE->UseResource<Texture>(id);
		if (texture != nullptr)
		{
			device_context_->PSSetShaderResources(slot++, 1, texture->srv.GetAddressOf());
		}
	}

	// Set Shader : VS
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	device_context_->IASetVertexBuffers(0, 1, level_mesh_.vertex_buffer.GetAddressOf(), &stride, &offset);
	device_context_->IASetIndexBuffer(level_mesh_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	device_context_->IASetInputLayout(vs->InputLayoyt());
	device_context_->VSSetShader(vs->Get(), 0, 0);

	device_context_->DrawIndexed(level_mesh_.indices.size(), 0, 0);
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

		face_normals[face++] = GetNormal(i0, i1, i2);
	}
	for (UINT f = 0; f < face_normals.size(); ++f)
	{
		for (UINT i = 0; i < 3; ++i)
		{
			UINT index = level_mesh_.indices[f * 3 + i];
			level_mesh_.vertices[index].n = face_normals[f];
		}
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

	// ConstantBuffer : Light

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbLight::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &level_light_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, level_light_.buffer.GetAddressOf());

	return true;
}