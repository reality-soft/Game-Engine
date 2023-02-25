#include "stdafx.h"
#include "Material.h"
#include "FileTransfer.h"
#include "ResourceMgr.h"

void KGCA41B::Material::CreateAndSave(string filename)
{
	Texture* dif = RESOURCE->UseResource<Texture>(diffuse);
	Texture* nor = RESOURCE->UseResource<Texture>(normalmap);
	Texture* met = RESOURCE->UseResource<Texture>(metalic);
	Texture* rou = RESOURCE->UseResource<Texture>(roughness);
	Texture* spe = RESOURCE->UseResource<Texture>(sepcular);
	Texture* amb = RESOURCE->UseResource<Texture>(ambient);
	Texture* opa = RESOURCE->UseResource<Texture>(opacity);
	
	if (dif != nullptr)
		textures[0] = dif->srv.Get();

	if (nor != nullptr)
		textures[1] = nor->srv.Get();

	if (met != nullptr)
		textures[2] = met->srv.Get();

	if (rou != nullptr)
		textures[3] = rou->srv.Get();

	if (spe != nullptr)
		textures[4] = spe->srv.Get();

	if (amb != nullptr)
		textures[5] = amb->srv.Get();

	if (opa != nullptr)
		textures[6] = opa->srv.Get();

	TextFileTransfer file_trans(filename, WRITE);

	file_trans.WriteText("diffuse", diffuse);
	file_trans.WriteText("normalmap", normalmap);
	file_trans.WriteText("metalic", metalic);
	file_trans.WriteText("roughness", roughness);
	file_trans.WriteText("sepcular", sepcular);
	file_trans.WriteText("ambient", ambient);
	file_trans.WriteText("opacity", opacity);
	file_trans.WriteText("shader", shader);

	file_trans.Close();

	auto strs = split(filename, '/');
	string id = strs[strs.size() - 1];

	RESOURCE->PushResource<Material>(id, *this);
}

void KGCA41B::Material::LoadAndCreate(string filename)
{
	TextFileTransfer file_trans(filename, READ);

	diffuse = file_trans.ReadText("diffuse");
	normalmap = file_trans.ReadText("normalmap");
	metalic = file_trans.ReadText("metalic");
	roughness = file_trans.ReadText("roughness");
	sepcular = file_trans.ReadText("sepcular");
	ambient = file_trans.ReadText("ambient");
	opacity = file_trans.ReadText("opacity");
	shader = file_trans.ReadText("shader");

	file_trans.Close();

	Texture* dif = RESOURCE->UseResource<Texture>(diffuse);
	Texture* nor = RESOURCE->UseResource<Texture>(normalmap);
	Texture* met = RESOURCE->UseResource<Texture>(metalic);
	Texture* rou = RESOURCE->UseResource<Texture>(roughness);
	Texture* spe = RESOURCE->UseResource<Texture>(sepcular);
	Texture* amb = RESOURCE->UseResource<Texture>(ambient);
	Texture* opa = RESOURCE->UseResource<Texture>(opacity);

	if (dif != nullptr)
		textures[0] = dif->srv.Get();

	if (nor != nullptr)
		textures[1] = nor->srv.Get();

	if (met != nullptr)
		textures[2] = met->srv.Get();

	if (rou != nullptr)
		textures[3] = rou->srv.Get();

	if (spe != nullptr)
		textures[4] = spe->srv.Get();

	if (amb != nullptr)
		textures[5] = amb->srv.Get();

	if (opa != nullptr)
		textures[6] = opa->srv.Get();
}