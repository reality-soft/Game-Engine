#include "stdafx.h"
#include "GuideLine.h"
#include "DX11App.h"
#include "ResourceMgr.h"

bool reality::GuideLine::Create(string mesh_id, string vs_id, string ps_id)
{
	StaticMesh* static_mesh = RESOURCE->UseResource<StaticMesh>(mesh_id);
	if (static_mesh == nullptr)
		return false;

	node_mesh = static_mesh->meshes[0];

	vertex_shader = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>(vs_id));
	if (vertex_shader.get() == nullptr)
		return false;

	pixel_shader = shared_ptr<PixelShader>(RESOURCE->UseResource<PixelShader>(ps_id));
	if (pixel_shader.get() == nullptr)
		return false;

	return true;
}

void reality::GuideLine::AddNode(XMVECTOR node_pos)
{
	if (line_nodes.empty() == false)
	{
		line_vectors.push_back(node_pos - line_nodes.back());
	}
	line_nodes.push_back(node_pos);

	UINT index = line_nodes.size();
	for (auto& node : line_nodes)
	{
		Vertex vertex;
		vertex.p = XMFLOAT3(node.m128_f32);
		vertex.c = XMFLOAT4(1, 1, 1, 1);
		line_mesh.vertices.push_back(vertex);

		if (index > 0)
		{
			line_mesh.indices.push_back(index - 1);
			line_mesh.indices.push_back(index);
			line_mesh.indices.push_back(index - 1);
		}

		index++;
	}

	line_mesh.vertex_buffer.ReleaseAndGetAddressOf();
	line_mesh.index_buffer.ReleaseAndGetAddressOf();

	HRESULT hr;

	// VertexBuffer
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(Vertex) * line_mesh.vertices.size();
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	subdata.pSysMem = line_mesh.vertices.data();

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, line_mesh.vertex_buffer.GetAddressOf());

	// IndexBuffer
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(UINT) * line_mesh.indices.size();
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subdata.pSysMem = line_mesh.indices.data();

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, line_mesh.index_buffer.GetAddressOf());
}

void reality::GuideLine::DrawLines(XMFLOAT4 color)
{
	if (line_nodes.empty())
		return;

	for (auto& vertex : node_mesh.vertices)
	{
		vertex.c = color;
	}
	DX11APP->GetDeviceContext()->UpdateSubresource(node_mesh.vertex_buffer.Get(), 0, nullptr, node_mesh.vertices.data(), 0, 0);

	for (auto& vertex : line_mesh.vertices)
	{
		vertex.c = color;
	}
	DX11APP->GetDeviceContext()->UpdateSubresource(line_mesh.vertex_buffer.Get(), 0, nullptr, line_mesh.vertices.data(), 0, 0);

	DX11APP->GetDeviceContext()->VSSetShader(vertex_shader.get()->Get(), 0, 0);
	DX11APP->GetDeviceContext()->PSSetShader(pixel_shader.get()->Get(), 0, 0);
	DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader.get()->InputLayout());

	for (auto node : line_nodes)
	{
		XMMATRIX world_matrix = XMMatrixTranslationFromVector(node);
		node_transform.data.world_matrix = XMMatrixTranspose(world_matrix);
		DX11APP->GetDeviceContext()->UpdateSubresource(node_transform.buffer.Get(), 0, 0, &node_transform.data, 0, 0);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, node_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(node_mesh.vertex_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, node_transform.buffer.GetAddressOf());
		DX11APP->GetDeviceContext()->DrawIndexed(node_mesh.indices.size(), 0, 0);
	}

	node_transform.data.world_matrix = XMMatrixIdentity();
	DX11APP->GetDeviceContext()->UpdateSubresource(node_transform.buffer.Get(), 0, 0, &node_transform.data, 0, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, line_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
	DX11APP->GetDeviceContext()->IASetIndexBuffer(line_mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, node_transform.buffer.GetAddressOf());
	DX11APP->GetDeviceContext()->DrawIndexed(line_mesh.indices.size(), 0, 0);
}