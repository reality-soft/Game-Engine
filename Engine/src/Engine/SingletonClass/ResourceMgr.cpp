#include "stdafx.h"
#include "ResourceMgr.h"
#include "FmodMgr.h"
#include "FbxMgr.h"
#include <io.h>
#include "DataMgr.h"

using namespace reality;

bool ResourceMgr::Init(LPCWSTR packagefile)
{
    return true;
}

bool ResourceMgr::Init(string directory)
{
    directory_ = directory;
    LoadAllResource();
    return true;
}

void ResourceMgr::Release()
{
    for (auto& pair : resdic_sound)
    {
        pair.second->release();
    }
}

void ResourceMgr::LoadAllResource()
{
    LoadDir(directory_ + "/SKM/", &ResourceMgr::ImportSKM);
    LoadDir(directory_ + "/LTM/", &ResourceMgr::ImportLTM);
    LoadDir(directory_ + "/STM/", &ResourceMgr::ImportSTM);
    LoadDir(directory_ + "/ANIM/", &ResourceMgr::ImportANIM);
    LoadDir(directory_ + "/Shader/", &ResourceMgr::ImportShaders);
    LoadDir(directory_ + "/Sound/", &ResourceMgr::ImportSound);
    LoadDir(directory_ + "/Texture/", &ResourceMgr::ImportTexture);
    LoadDir(directory_ + "/Sprite/", &ResourceMgr::ImportSprite);
    LoadDir(directory_ + "/Material/", &ResourceMgr::ImportMaterial);
}

void ResourceMgr::LoadDir(string path, Load_Func load_func)
{   
    string tempAdd = path + "*.*";
    intptr_t handle;
    struct _finddata_t fd;
     handle = _findfirst(tempAdd.c_str(), &fd);

    // ��ã��� ����
    if (handle == -1L) return;

    do {
        if ((fd.attrib & _A_SUBDIR) && (fd.name[0] != '.'))
        {
            LoadDir(path + fd.name + "/", load_func);
        }
        else if (fd.name[0] != '.')
        {
            (this->*load_func)(path + fd.name);
        }
    } while (_findnext(handle, &fd) == 0);
}

map<string, string> reality::ResourceMgr::GetTotalResID()
{
    map<string, string> res_id_map;
    
    for (auto res : resdic_static_mesh)
    {
        res_id_map.insert(make_pair(res.first, "STM"));
    }
    for (auto res : resdic_skeletal_mesh)
    {
        res_id_map.insert(make_pair(res.first, "SKM"));
    }
    for (auto res : resdic_light_mesh)
    {
        res_id_map.insert(make_pair(res.first, "SKM"));
    }
    for (auto res : resdic_animation)
    {
        res_id_map.insert(make_pair(res.first, "ANM"));
    }
    for (auto res : resdic_vs)
    {
        res_id_map.insert(make_pair(res.first, "VS"));
    }
    for (auto res : resdic_ps)
    {
        res_id_map.insert(make_pair(res.first, "PS"));
    }
    for (auto res : resdic_texture)
    {
        res_id_map.insert(make_pair(res.first, "TEX"));
    }
    for (auto res : resdic_sound)
    {
        res_id_map.insert(make_pair(res.first, "SND"));
    }

    return res_id_map;
}

set<string> reality::ResourceMgr::GetTotalTexID()
{
    set<string> tex_id_set;
    for (auto pair : resdic_texture)
    {
        tex_id_set.insert(pair.first);
    }
    return tex_id_set;
}

set<string> reality::ResourceMgr::GetTotalVSID()
{
    set<string> vs_id_set;
    for (auto pair : resdic_vs)
    {
        vs_id_set.insert(pair.first);
    }
    return vs_id_set;
}

set<string> reality::ResourceMgr::GetTotalPSID()
{
    set<string> ps_id_set;
    for (auto pair : resdic_ps)
    {
        ps_id_set.insert(pair.first);
    }
    return ps_id_set;
}

set<string> reality::ResourceMgr::GetTotalGSID()
{
    set<string> gs_id_set;
    for (auto pair : resdic_gs)
    {
        gs_id_set.insert(pair.first);
    }
    return gs_id_set;
}

set<string> reality::ResourceMgr::GetTotalSKMID()
{
    set<string> skm;
    for (auto pair : resdic_skeletal_mesh)
    {
        skm.insert(pair.first);
    }
    return skm;
}

set<string> reality::ResourceMgr::GetTotalSTMID()
{
    set<string> stm;
    for (auto pair : resdic_static_mesh)
    {
        stm.insert(pair.first);
    }
    return stm;
}

set<string> reality::ResourceMgr::GetTotalANIMID()
{
    set<string> anim;
    for (auto pair : resdic_animation)
    {
        anim.insert(pair.first);
    }
    return anim;
}

set<string> reality::ResourceMgr::GetTotalSpriteID()
{
    set<string> sprite;
    for (auto pair : resdic_sprite)
    {
        sprite.insert(pair.first);
    }
    return sprite;
}

set<string> reality::ResourceMgr::GetTotalMATID()
{
    set<string> mat;
    for (auto pair : resdic_material)
    {
        mat.insert(pair.first);
    }
    return mat;
}


void reality::ResourceMgr::PushLightMesh(string id, const LightMesh& light_mesh)
{
    resdic_light_mesh.insert({id, light_mesh });
}

void reality::ResourceMgr::PushStaticMesh(string id, const StaticMesh& static_mesh)
{
    resdic_static_mesh.insert({ id, static_mesh });
}

void reality::ResourceMgr::PushSkeletalMesh(string id, const SkeletalMesh& skeletal_mesh)
{
    resdic_skeletal_mesh.insert({ id, skeletal_mesh });
}

void reality::ResourceMgr::PushAnimation(string id, const vector<OutAnimData>& animation)
{
    resdic_animation.insert({ id, animation });
}

bool reality::ResourceMgr::ImportShaders(string filename)
{

    if (filename.find("VS") != string::npos)
    {
        VertexShader new_vs;
        if (new_vs.LoadCompiled(to_mw(filename)) == false)
            return false;

        auto strs = split(filename, '/');
        string id = strs[strs.size() - 1];

        resdic_vs.insert(make_pair(id, new_vs));
        return true;
    }
    if (filename.find("PS") != string::npos)
    {
        PixelShader ps_default;
        if (!ps_default.LoadCompiled(to_mw(filename)))
            return false;

        auto strs = split(filename, '/');
        string id = strs[strs.size() - 1];

        resdic_ps.insert(make_pair(id, ps_default));
        return true;
    }
    if (filename.find("GS") != string::npos)
    {
        GeometryShader gs_default;
        if (!gs_default.LoadCompiled(to_mw(filename)))
            return false;

        auto strs = split(filename, '/');
        string id = strs[strs.size() - 1];

        resdic_gs.insert(make_pair(id, gs_default));
        return true;
    }

    return true;
}

bool ResourceMgr::ImportSound(string filename)
{
    FMOD::Sound* newSound;

    FMOD_RESULT hr = FMOD_MGR->fmod_system()->createSound(filename.c_str(), (FMOD_MODE)(FMOD_3D), nullptr, &newSound);
    if (hr != FMOD_OK)
    {
        return false;
    }

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_sound.insert(make_pair(id, newSound));
    return true;
}

bool reality::ResourceMgr::ImportSKM(string filename)
{
    SkeletalMesh skeletal_mesh = FBX->LoadSkeletalMesh(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_skeletal_mesh.insert(make_pair(id, skeletal_mesh));

    return false;
}

bool reality::ResourceMgr::ImportSTM(string filename)
{
    StaticMesh static_mesh = FBX->LoadStaticMesh(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_static_mesh.insert(make_pair(id, static_mesh));

    return true;
}

bool reality::ResourceMgr::ImportLTM(string filename)
{
    LightMesh static_mesh = FBX->LoadLightMesh(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_light_mesh.insert(make_pair(id, static_mesh));

    return true;
}

bool reality::ResourceMgr::ImportANIM(string filename)
{
    vector<OutAnimData> animation = FBX->LoadAnimation(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_animation.insert(make_pair(id, animation));

    return true;
}

bool ResourceMgr::ImportTexture(string filename)
{
    Texture new_tex;
    bool success_load = false;

    success_load = new_tex.LoadTextureWIC(to_mw(filename));

    if (!success_load)
        success_load = new_tex.LoadTextureDDS(to_mw(filename));

    if (!success_load)
        return false;

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_texture.insert(make_pair(id, new_tex));

    return false;
}

bool ResourceMgr::ImportSprite(string filename)
{
    DATA->LoadSheetFile(filename);

    auto strs1 = split(filename, '/');
    auto name = strs1[max((int)strs1.size() - 1, 0)];
    auto strs2 = split(name, '.');
    name = strs2[0];

    auto sheet = DATA->LoadSheet(name);

    if (!sheet)
        return false;

    auto item = sheet->LoadItem(name);
    if (item == NULL)
        return false;

    string str_type = item->GetValue("type");

    if (str_type == "")
        return false;

    E_EffectType type = (E_EffectType)stoi(str_type);

    switch (type)
    {
    case UV_SPRITE:
    {
        UVSprite uv_sprite;
        uv_sprite.tex_id = item->GetValue("tex_id");

        auto uvListItem = sheet->LoadItem("uvList");
        // 리스트에서 가장 높은 프레임의 값을 가져온다.
        int max = 0;
        for (int i = 1; true; i++)
        {
            if (uvListItem->values[to_string(i)] == "")
            {
                max = i - 1;
                break;
            }

        }
        // 가장 낮은 프레임부터 가장 높은 프레임까지 파싱해서 uv값을 넣어준다.
        uv_sprite.uv_list.clear();
        for (int i = 0; i < max; i++)
        {
            // 0 0 25 25 형식
            auto splited_str = split(uvListItem->values[to_string(i + 1)], ' ');
            uv_sprite.uv_list.push_back({ { stol(splited_str[0]), stol(splited_str[1]) }, { stol(splited_str[2]), stol(splited_str[3]) } });
        }

        // 로딩한 스프라이트를 리스트에 넣는다.
        resdic_sprite.insert({ name, make_shared<UVSprite>(uv_sprite) });
    } break;
    case TEX_SPRITE:
    {
        TextureSprite tex_sprite;

        // TODO : 데이터 형태 수정해야할듯
        auto texListItem = sheet->LoadItem("texList");
        // 리스트에서 가장 높은 프레임의 값을 가져온다.
        int max = 0;
        for (int i = 1; true; i++)
        {
            if (texListItem->values[to_string(i)] == "")
            {
                max = i - 1;
                break;
            }
        }
        // 가장 낮은 프레임부터 가장 높은 프레임까지 파싱해서 tex_id값을 넣어준다.
        tex_sprite.tex_id_list.clear();
        for (int i = 0; i < max; i++)
        {
            tex_sprite.tex_id_list.push_back(texListItem->values[to_string(i + 1)]);
        }
        // 로딩한 스프라이트를 리스트에 넣는다.
        resdic_sprite.insert({ name, make_shared<TextureSprite>(tex_sprite) });
    } break;
    }

    return true;
}

bool reality::ResourceMgr::SaveSprite(string name, shared_ptr<Sprite> new_sprite)
{
    if (resdic_sprite.find(name) != resdic_sprite.end())
        return false;
    else
    {
        resdic_sprite.insert({ name, new_sprite });
        return true;
    }
    
}

bool reality::ResourceMgr::ImportMaterial(string filename)
{
    Material material;
    material.LoadAndCreate(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    RESOURCE->PushResource<Material>(id, material);

    return true;
}
