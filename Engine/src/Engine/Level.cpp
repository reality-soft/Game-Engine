#include "stdafx.h"
#include "Level.h"
#include "DX11App.h"
#include "PhysicsMgr.h"
#include "FileTransfer.h"
#include "TimeMgr.h"

using namespace reality;

reality::SkySphere::SkySphere()
{
}

reality::SkySphere::~SkySphere()
{
}

bool reality::SkySphere::CreateSphere()
{
	sphere_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>("SkySphere.stmesh"));
	cloud_dome = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>("CloudDome.stmesh"));

	vs = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>("SkySphereVS.cso"));

	if (sphere_mesh.get() == nullptr)
		return false;

	if (cloud_dome.get() == nullptr);

	if (vs.get() == nullptr)
		return false;

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

		FrameSunSky(camera);
		RenderSunSky();

		FrameStarSky(camera);
		RenderStarSky();

		FrameCloudSky(camera);
		RenderCloudSky();
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

	skycolor_afternoon = { 1.0f,  0.2f,   0.01f, 1.0f };
	skycolor_noon = { 0.2f,  0.2f,   0.5f,  1.0f };
	skycolor_night = { 0.01f, 0.005f, 0.03f, 1.0f };
	cb_sky.data.strength.x = 0.5f;
	cb_sky.data.strength.y = 0.5f;
	cb_sky.data.strength.z = 0.01f;


	if (cb_sky.data.time.x >= 0)
	{
		cb_sky.data.time.x -= TM_DELTATIME * 10;
		cb_sky.data.sky_color = LerpColor(skycolor_afternoon, skycolor_noon, (360 - cb_sky.data.time.x) / 360);
		cb_sky.data.strength.w = cb_sky.data.strength.x;

	}
	else if (cb_sky.data.time.y >= 0)
	{
		cb_sky.data.time.y -= TM_DELTATIME * 10;
		cb_sky.data.sky_color = LerpColor(skycolor_noon, skycolor_afternoon, (360 - cb_sky.data.time.y) / 360);
		cb_sky.data.strength.w = cb_sky.data.strength.y;
	}
	else if (cb_sky.data.time.z >= 0)
	{
		cb_sky.data.time.z -= TM_DELTATIME * 10;
		cb_sky.data.sky_color = LerpColor(skycolor_afternoon, skycolor_night, (360 - cb_sky.data.time.z) / 360);
		cb_sky.data.strength.w = cb_sky.data.strength.y + (cb_sky.data.strength.z - cb_sky.data.strength.y) * (360 - cb_sky.data.time.z) / 360;
	}
	else if (cb_sky.data.time.w >= 0)
	{
		cb_sky.data.time.w -= TM_DELTATIME * 10;
		cb_sky.data.sky_color = LerpColor(skycolor_night, skycolor_afternoon, (360 - cb_sky.data.time.w) / 360);
		cb_sky.data.strength.w = cb_sky.data.strength.z + (cb_sky.data.strength.y - cb_sky.data.strength.z) * (360 - cb_sky.data.time.w) / 360;
	}
	else
	{
		cb_sky.data.time = { 360, 360, 360, 360 };
	}

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_sky.buffer.Get(), 0, 0, &cb_sky.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_sky.buffer.GetAddressOf());
}

void reality::SkySphere::RenderBackgroundSky()
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (auto mesh : sphere_mesh.get()->meshes)
	{
		reality::Material* material = RESOURCE->UseResource<reality::Material>("BackgroundSky.mat");
		if (material)
			material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(mesh.indices.size(), 0, 0);
	}
}

void reality::SkySphere::FrameSunSky(const C_Camera* camera)
{
	XMMATRIX following_camera_matrix =
		XMMatrixScaling(camera->far_z * 0.8, camera->far_z * 0.8, camera->far_z * 0.8) *
		XMMatrixTranslationFromVector(camera->camera_pos) *
		XMMatrixRotationY(XMConvertToRadians(90)) *
		XMMatrixRotationZ(XMConvertToRadians(TM_GAMETIME));

	cb_transform.data.world_matrix = XMMatrixTranspose(following_camera_matrix);

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_transform.buffer.Get(), 0, 0, &cb_transform.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_transform.buffer.GetAddressOf());

	cb_sky.data.sky_color = { 1, 1, 1, 0 };
	cb_sky.data.strength.w = -1;
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_sky.buffer.Get(), 0, 0, &cb_sky.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_sky.buffer.GetAddressOf());
}

void reality::SkySphere::RenderSunSky()
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (auto mesh : cloud_dome.get()->meshes)
	{
		reality::Material* material = RESOURCE->UseResource<reality::Material>("SunSky.mat");
		if (material)
			material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(mesh.indices.size(), 0, 0);
	}
}

void reality::SkySphere::FrameStarSky(const C_Camera* camera)
{
	XMMATRIX following_camera_matrix =
		XMMatrixScaling(camera->far_z * 0.8, camera->far_z * 0.8, camera->far_z * 0.8) *
		XMMatrixTranslationFromVector(camera->camera_pos) *
		XMMatrixRotationY(XMConvertToRadians(-90)) *
		XMMatrixRotationZ(XMConvertToRadians(TM_GAMETIME));

	cb_transform.data.world_matrix = XMMatrixTranspose(following_camera_matrix);

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_transform.buffer.Get(), 0, 0, &cb_transform.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_transform.buffer.GetAddressOf());

	cb_sky.data.sky_color = { 1, 1, 1, 0 };
	cb_sky.data.strength.w = -1;
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_sky.buffer.Get(), 0, 0, &cb_sky.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_sky.buffer.GetAddressOf());
}

void reality::SkySphere::RenderStarSky()
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (auto mesh : cloud_dome.get()->meshes)
	{
		reality::Material* material = RESOURCE->UseResource<reality::Material>("StarSky.mat");
		if (material)
			material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(mesh.indices.size(), 0, 0);
	}
}

void reality::SkySphere::FrameCloudSky(const C_Camera* camera)
{
	XMMATRIX following_camera_matrix =
		XMMatrixScaling(camera->far_z * 0.5, camera->far_z * 0.5, camera->far_z * 0.5) *
		XMMatrixTranslationFromVector(camera->camera_pos) * 
		XMMatrixRotationX(XMConvertToRadians(-90)) * 
		XMMatrixRotationY(XMConvertToRadians(TM_GAMETIME));

	cb_transform.data.world_matrix = XMMatrixTranspose(following_camera_matrix);

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_transform.buffer.Get(), 0, 0, &cb_transform.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_transform.buffer.GetAddressOf());

	cb_sky.data.sky_color = { 1, 1, 1, 0 };
	cb_sky.data.strength.w = -1;
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_sky.buffer.Get(), 0, 0, &cb_sky.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_sky.buffer.GetAddressOf());

	ID3D11SamplerState* sampler = DX11APP->GetCommonStates()->LinearWrap();
	DX11APP->GetDeviceContext()->PSSetSamplers(0, 1, &sampler);
}

void reality::SkySphere::RenderCloudSky()
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (auto mesh : cloud_dome.get()->meshes)
	{
		reality::Material* material = RESOURCE->UseResource<reality::Material>("CloudSky.mat");
		if (material)
			material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(mesh.indices.size(), 0, 0);
	}
}


reality::Level::~Level()
{
	level_mesh_.vertices.clear();
	level_mesh_.vertex_buffer.Get()->Release();
	level_mesh_.vertex_buffer.ReleaseAndGetAddressOf();

	level_mesh_.indices.clear();
	level_mesh_.index_buffer.Get()->Release();
	level_mesh_.index_buffer.ReleaseAndGetAddressOf();

	height_list_.clear();

	height_field_body_->removeCollider(height_field_collider_);
	PHYSICS->GetPhysicsWorld()->destroyCollisionBody(height_field_body_);

	height_field_shape_ = nullptr;
	height_field_collider_ = nullptr;
	height_field_body_ = nullptr;

	for (auto inst_obj : inst_objects)
	{
		inst_obj.Release();
	}
}

bool reality::Level::ImportFromFile(string filepath)
{
	FileTransfer file_transfer(filepath, READ);

	//Single Datas;
	file_transfer.ReadBinary<UINT>(num_row_vertex_);
	file_transfer.ReadBinary<UINT>(num_col_vertex_);
	file_transfer.ReadBinary<float>(cell_distance_);
	file_transfer.ReadBinary<float>(uv_scale_);
	file_transfer.ReadBinary<UINT>(max_lod_);
	file_transfer.ReadBinary<UINT>(cell_scale_);
	file_transfer.ReadBinary<XMINT2>(row_col_blocks_);

	// Arrays
	file_transfer.ReadBinary<LevelVertex>(level_mesh_.vertices);
	file_transfer.ReadBinary<UINT>(level_mesh_.indices);
	file_transfer.ReadBinary<string>(texture_id);

	UINT inst_list_size = 0;
	file_transfer.ReadBinary<UINT>(inst_list_size);

	inst_objects.resize(inst_list_size);

	for (auto& inst : inst_objects)
	{
		file_transfer.ReadBinary<string>(inst.mesh_id_);
		file_transfer.ReadBinary<string>(inst.vs_id_);
	}

	file_transfer.Close();

	vs_id_ = "LevelVS.cso";
	ps_id_ = "LevelPS.cso";

	XMFLOAT2 minmax_height = GetMinMaxHeight();

	if (CreateBuffers() == false)
		return false;

	if (CreateHeightField(minmax_height.x, minmax_height.y) == false)
		return false;

	for (auto& inst : inst_objects)
	{
		inst.Init(inst.mesh_id_, inst.vs_id_);
	}

	sky_sphere.CreateSphere();

	return true;
}

bool reality::Level::CreateLevel(UINT _max_lod, UINT _cell_scale, UINT _uv_scale, XMINT2 _row_col_blocks)
{
	max_lod_ = _max_lod;
	cell_scale_ = _cell_scale;
	row_col_blocks_ = _row_col_blocks;
	uv_scale_ = _uv_scale;

	num_row_vertex_ = pow(2, max_lod_) * row_col_blocks_.x + 1;
	num_col_vertex_ = pow(2, max_lod_) * row_col_blocks_.y + 1;
	cell_distance_ = cell_scale_ / pow(2, max_lod_);

	UINT num_row_cell = num_row_vertex_ - 1;
	UINT num_col_cell = num_col_vertex_ - 1;
	float half_row = num_row_vertex_ / 2;
	float half_col = num_col_vertex_ / 2;

	level_mesh_.vertices.resize(num_row_vertex_ * num_col_vertex_);

	for (int r = 0; r < num_row_vertex_; ++r)
	{
		for (int c = 0; c < num_col_vertex_; ++c)
		{
			UINT index = r * num_row_vertex_ + c;

			level_mesh_.vertices[index].p.x = (c - half_row) * cell_distance_;
			level_mesh_.vertices[index].p.y = 0.0f;
			level_mesh_.vertices[index].p.z = (half_col - r) * cell_distance_;

			level_mesh_.vertices[index].c = { 0, 0, 0, 0 };

			level_mesh_.vertices[index].t.x = (float)c / (float)(num_row_cell)*uv_scale_;
			level_mesh_.vertices[index].t.y = (float)r / (float)(num_col_cell)*uv_scale_;

			level_mesh_.vertices[index].t_layer.x = (float)c / (float)(num_row_cell);
			level_mesh_.vertices[index].t_layer.y = (float)r / (float)(num_col_cell);
		}
	}

	level_mesh_.indices.resize(num_row_cell * num_col_cell * 6);
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
	XMFLOAT2 minmax_height = GetMinMaxHeight();

	if (CreateBuffers() == false)
		return false;

	if (CreateHeightField(minmax_height.x, minmax_height.y) == false)
		return false;

	sky_sphere.CreateSphere();

	alpha_layer.CreateAlphaTexture(1024, 1024 / num_col_cell);

	texture_id.resize(5);

	return true;
}

void reality::Level::SetCamera(C_Camera* _camera)
{
	camera = _camera;
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

void reality::Level::RenderObjects()
{
	for (auto inst : inst_objects)
	{
		inst.Frame();
		inst.Render();
	}
}

void reality::Level::RenderSkySphere()
{
	sky_sphere.FrameRender(camera);
}

void Level::Update()
{
	RenderSkySphere();
	RenderObjects();

	SetTextures();
}

void Level::Render(bool culling)
{
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(vs_id_);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(ps_id_);


	{ // Textures Stage

		DX11APP->GetDeviceContext()->PSSetShaderResources(0, ARRAYSIZE(texture_layers), texture_layers);
		DX11APP->GetDeviceContext()->PSSetShaderResources(5, 1, alpha_layer.alpha_srv.GetAddressOf());
	}

	{ // Samplers Stage
		ID3D11SamplerState* sample = DX11APP->GetCommonStates()->LinearWrap();
		DX11APP->GetDeviceContext()->PSSetSamplers(0, 1, &sample);
	}

	{ // Input Assembly Stage
		UINT stride = sizeof(LevelVertex);
		UINT offset = 0;
		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, level_mesh_.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayout());
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

bool reality::Level::CreateLevelMesh(string ltmesh_file)
{
	level_light_mesh = shared_ptr<LightMesh>(RESOURCE->UseResource<LightMesh>(ltmesh_file));
	if (level_light_mesh.get() == nullptr)
		return false;

	for (auto& mesh : level_light_mesh.get()->meshes)
	{
		total_vertex_buffers.push_back(mesh.vertex_buffer.Get());
	}

	sky_sphere.CreateSphere();

	return true;
}

void reality::Level::RenderLevelMesh()
{
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(vs_id_);
	DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);

	for (auto mesh : level_light_mesh.get()->meshes)
	{
		reality::Material* material = RESOURCE->UseResource<reality::Material>(mesh.mesh_name + ".mat");
		if (material)
			material->Set();

		UINT stride = sizeof(LightVertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
	}
}

XMINT2 reality::Level::GetWorldSize()
{
	return XMINT2(num_row_vertex_ - 1, num_col_vertex_ - 1);
}

XMINT2 reality::Level::GetBlocks()
{
	return row_col_blocks_;
}

UINT reality::Level::MaxLod()
{
	return max_lod_;
}


void reality::Level::SetTextures()
{
	int index = 0;
	for (auto texid : texture_id)
	{
		if (index >= ARRAYSIZE(texture_layers))
			break;

		Texture* texture = RESOURCE->UseResource<Texture>(texid);
		if (texture)
		{
			texture_layers[index++] = texture->srv.Get();
		}
	}
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

XMVECTOR reality::Level::GetNormalAt(float x, float z)
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

	XMFLOAT3 normal;

	normal = GetNormal(
		(int)vertex_x * num_row_vertex_ + (int)vertex_z, 
		(int)vertex_x * num_row_vertex_ + (int)vertex_z + 1, 
		(int)vertex_x + 1 * num_row_vertex_ + (int)vertex_z);

	return XMLoadFloat3(&normal);
}

void Level::GetHeightList()
{
	for (auto vertex : level_mesh_.vertices)
	{
		height_list_.push_back(vertex.p.y);
	}
}

XMFLOAT2 reality::Level::GetMinMaxHeight()
{
	float min = 0;
	float max = 0;

	for (auto vertex : level_mesh_.vertices)
	{
		min = std::min(min, vertex.p.y);
		max = std::max(min, vertex.p.y);
	}

	return XMFLOAT2(min, max);
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

	desc.ByteWidth = sizeof(LevelVertex) * level_mesh_.vertices.size();
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

	return true;
}
