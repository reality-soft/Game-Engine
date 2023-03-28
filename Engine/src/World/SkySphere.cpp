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

	sun_world = XMMatrixIdentity();

	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subresource, sizeof(subresource));

	desc.ByteWidth = sizeof(CbTransform::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subresource.pSysMem = &cb_transform.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subresource, cb_transform.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

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

void reality::SkySphere::FrameRender(const C_Camera* camera)
{
	DX11APP->GetDeviceContext()->VSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, 0, 0);

	DX11APP->GetDeviceContext()->IASetInputLayout(vs.get()->InputLayout());
	DX11APP->GetDeviceContext()->VSSetShader(vs.get()->Get(), 0, 0);

	//DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->CullClockwise());
	DX11APP->GetDeviceContext()->OMSetBlendState(DX11APP->GetCommonStates()->Additive(), 0, -1);
	{
		FrameBackgroundSky(camera);
		RenderBackgroundSky();
	}
	//DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->CullNone());
	DX11APP->GetDeviceContext()->OMSetBlendState(DX11APP->GetCommonStates()->Opaque(), 0, -1);
}

void reality::SkySphere::FrameBackgroundSky(const C_Camera* camera)
{
	XMMATRIX following_camera_matrix =
		XMMatrixScaling(camera->far_z * 0.9, camera->far_z * 0.9, camera->far_z * 0.9) *
		XMMatrixTranslationFromVector(camera->camera_pos);

	cb_transform.data.world_matrix = XMMatrixTranspose(following_camera_matrix);

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_transform.buffer.Get(), 0, 0, &cb_transform.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_transform.buffer.GetAddressOf());

	skycolor_afternoon = { 0.859375f, 0.1953125f, 0.06640625f, 1.0f };
	//cb_sky.data.strength.x = 0.5f;
	//cb_sky.data.strength.y = 0.5f;
	//cb_sky.data.strength.z = 0.01f;


	//if (cb_sky.data.time.x >= 0)
	//{
	//	cb_sky.data.time.x -= TM_DELTATIME;
	//	cb_sky.data.sky_color = LerpColor(skycolor_afternoon, skycolor_noon, (240 - cb_sky.data.time.x) / 240);
	//	cb_sky.data.strength.w = cb_sky.data.strength.x;

	//}
	//else if (cb_sky.data.time.y >= 0)
	//{
	//	cb_sky.data.time.y -= TM_DELTATIME;
	//	cb_sky.data.sky_color = LerpColor(skycolor_noon, skycolor_afternoon, (480 - cb_sky.data.time.y) / 480);
	//	cb_sky.data.strength.w = cb_sky.data.strength.y;
	//}
	//else if (cb_sky.data.time.z >= 0)
	//{
	//	cb_sky.data.time.z -= TM_DELTATIME;
	//	cb_sky.data.sky_color = LerpColor(skycolor_afternoon, skycolor_night, (240 - cb_sky.data.time.z) / 240);
	//	cb_sky.data.strength.w = cb_sky.data.strength.y + (cb_sky.data.strength.z - cb_sky.data.strength.y) * (240 - cb_sky.data.time.z) / 240;
	//}
	//else if (cb_sky.data.time.w >= 0)
	//{
	//	cb_sky.data.time.w -= TM_DELTATIME;
	//	cb_sky.data.sky_color = LerpColor(skycolor_night, skycolor_afternoon, (480 - cb_sky.data.time.w) / 480);
	//	cb_sky.data.strength.w = cb_sky.data.strength.z + (cb_sky.data.strength.y - cb_sky.data.strength.z) * (480 - cb_sky.data.time.w) / 480;
	//}
	//else
	//{
	//	cb_sky.data.time = { 240, 480, 240, 480 };
	//}
	cb_sky.data.sky_color = skycolor_afternoon;
	cb_sky.data.strength.w = 0.612344132f;
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_sky.buffer.Get(), 0, 0, &cb_sky.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_sky.buffer.GetAddressOf());
}

void reality::SkySphere::RenderBackgroundSky()
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