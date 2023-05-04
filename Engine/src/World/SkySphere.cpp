#include "stdafx.h"
#include "SkySphere.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"

reality::SkySphere::SkySphere()
{
}

reality::SkySphere::~SkySphere()
{
}

bool reality::SkySphere::CreateSphere()
{
	sphere_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>("SkySphere.stmesh"));
	vs = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>("SkySphereVS.cso"));

	if (sphere_mesh.get() == nullptr)
		return false;

	if (vs.get() == nullptr)
		return false;

	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subresource;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subresource, sizeof(subresource));

	desc.ByteWidth = sizeof(CbSkySphere::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subresource.pSysMem = &cb_sky.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subresource, cb_sky.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

void reality::SkySphere::Update(float lerp_value)
{
	DX11APP->GetDeviceContext()->VSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, 0, 0);

	DX11APP->GetDeviceContext()->IASetInputLayout(vs.get()->InputLayout());
	DX11APP->GetDeviceContext()->VSSetShader(vs.get()->Get(), 0, 0);

	cb_sky.data.sky_color = LerpColor(skycolor_night, skycolor_noon, lerp_value);

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_sky.buffer.Get(), 0, 0, &cb_sky.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_sky.buffer.GetAddressOf());
}

void reality::SkySphere::Render()
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (auto& mesh : sphere_mesh.get()->meshes)
	{
		reality::Material* material = RESOURCE->UseResource<reality::Material>("BackgroundSky.mat");
		if (material)
			material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(mesh.indices.size(), 0, 0);
	}
}

void reality::SkySphere::SetNoonColor(XMFLOAT4 _color)
{
	skycolor_noon = _color;
}

void reality::SkySphere::SetNightColor(XMFLOAT4 _color)
{
	skycolor_night = _color;
}

XMFLOAT4 reality::SkySphere::GetSkyColor()
{
	return cb_sky.data.sky_color;
}
