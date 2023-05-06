#include "stdafx.h"
#include "Material.h"
#include "FileTransfer.h"
#include "ResourceMgr.h"
#include <io.h>

reality::Material::Material()
{
	for (int i = 0; i < 7; ++i)
		textures[i] = nullptr;

	pixel_shader = nullptr;
	sampler = nullptr;
}

void reality::Material::SaveEmpty(string filename)
{
	if (_access(filename.c_str(), 0) != -1)
	{
		return;
	}

	TextFileTransfer file_trans(filename, WRITE);

	file_trans.WriteText("diffuse", "null");
	file_trans.WriteText("normalmap", "null");
	file_trans.WriteText("metalic", "null");
	file_trans.WriteText("roughness", "null");
	file_trans.WriteText("specular", "null");
	file_trans.WriteText("ambient", "null");
	file_trans.WriteText("opacity", "null");
	file_trans.WriteText("shader", "null");

	file_trans.Close();
}

void reality::Material::Create()
{
	Texture* dif = RESOURCE->UseResource<Texture>(diffuse);
	Texture* nor = RESOURCE->UseResource<Texture>(normalmap);
	Texture* met = RESOURCE->UseResource<Texture>(metalic);
	Texture* rou = RESOURCE->UseResource<Texture>(roughness);
	Texture* spe = RESOURCE->UseResource<Texture>(specular);
	Texture* amb = RESOURCE->UseResource<Texture>(ambient);
	Texture* opa = RESOURCE->UseResource<Texture>(opacity);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(shader);

	if (dif)
		textures[0] = dif->srv.Get();

	if (nor)
		textures[1] = nor->srv.Get();

	if (met)
		textures[2] = met->srv.Get();

	if (rou)
		textures[3] = rou->srv.Get();

	if (spe)
		textures[4] = spe->srv.Get();

	if (amb)
		textures[5] = amb->srv.Get();

	if (opa)
		textures[6] = opa->srv.Get();

	if (ps)
		pixel_shader = ps->Get();

	sampler = DX11APP->GetCommonStates()->LinearClamp();
}

void reality::Material::Save(string filename)
{
	TextFileTransfer file_trans(filename, WRITE);

	file_trans.WriteText("diffuse", diffuse);
	file_trans.WriteText("normalmap", normalmap);
	file_trans.WriteText("metalic", metalic);
	file_trans.WriteText("roughness", roughness);
	file_trans.WriteText("specular", specular);
	file_trans.WriteText("ambient", ambient);
	file_trans.WriteText("opacity", opacity);
	file_trans.WriteText("shader", shader);

	file_trans.Close();
}

void reality::Material::Set()
{
	if (pixel_shader)
		DX11APP->GetDeviceContext()->PSSetShader(pixel_shader, 0, 0);


	DX11APP->GetDeviceContext()->PSSetSamplers(0, 1, &sampler);
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 7, textures);
}

void reality::Material::LoadAndCreate(string filename)
{
	TextFileTransfer file_trans(filename, READ);

	diffuse = file_trans.ReadText("diffuse");
	normalmap = file_trans.ReadText("normalmap");
	metalic = file_trans.ReadText("metalic");
	roughness = file_trans.ReadText("roughness");
	specular = file_trans.ReadText("specular");
	ambient = file_trans.ReadText("ambient");
	opacity = file_trans.ReadText("opacity");
	shader = file_trans.ReadText("shader");

	file_trans.Close();

	Create();
}