#include "stdafx.h"
#include "ResourceMgr.h"
#include "FmodMgr.h"
#include "FbxMgr.h"
#include <io.h>

using namespace KGCA41B;

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
    LoadDir(directory_ + "/STM/", &ResourceMgr::ImportSTM);
    LoadDir(directory_ + "/ANIM/", &ResourceMgr::ImportANIM);
    LoadDir(directory_ + "/Shader/", &ResourceMgr::ImportShaders);
    LoadDir(directory_ + "/Sound/", &ResourceMgr::ImportSound);
    LoadDir(directory_ + "/Texture/", &ResourceMgr::ImportTexture);
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

map<string, string> KGCA41B::ResourceMgr::GetTotalResID()
{
    map<string, string> res_id_map;
    
    for (auto res : resdic_static_mesh)
    {
        res_id_map.insert(make_pair("[STM]" + res.first, "STM"));
    }
    for (auto res : resdic_skeletal_mesh)
    {
        res_id_map.insert(make_pair("[SKM]" + res.first, "SKM"));
    }
    for (auto res : resdic_animation)
    {
        res_id_map.insert(make_pair("[ANM]" + res.first, "ANM"));
    }
    for (auto res : resdic_vs)
    {
        res_id_map.insert(make_pair("[VS]" + res.first, "VS"));
    }
    for (auto res : resdic_ps)
    {
        res_id_map.insert(make_pair("[PS]" + res.first, "PS"));
    }
    for (auto res : resdic_texture)
    {
        res_id_map.insert(make_pair("[TEX]" + res.first, "TEX"));
    }
    for (auto res : resdic_sound)
    {
        res_id_map.insert(make_pair("[SND]" + res.first, "SND"));
    }

    return res_id_map;
}

set<string> KGCA41B::ResourceMgr::GetTotalTexID()
{
    set<string> tex_id_set;
    for (auto pair : resdic_texture)
    {
        tex_id_set.insert(pair.first);
    }
    return tex_id_set;
}

set<string> KGCA41B::ResourceMgr::GetTotalVSID()
{
    set<string> vs_id_set;
    for (auto pair : resdic_vs)
    {
        vs_id_set.insert(pair.first);
    }
    return vs_id_set;
}

set<string> KGCA41B::ResourceMgr::GetTotalPSID()
{
    set<string> ps_id_set;
    for (auto pair : resdic_ps)
    {
        ps_id_set.insert(pair.first);
    }
    return ps_id_set;
}

set<string> KGCA41B::ResourceMgr::GetTotalGSID()
{
    set<string> gs_id_set;
    for (auto pair : resdic_gs)
    {
        gs_id_set.insert(pair.first);
    }
    return gs_id_set;
}

set<string> KGCA41B::ResourceMgr::GetTotalSKMID()
{
    set<string> skm;
    for (auto pair : resdic_skeletal_mesh)
    {
        skm.insert(pair.first);
    }
    return skm;
}

set<string> KGCA41B::ResourceMgr::GetTotalSTMID()
{
    set<string> stm;
    for (auto pair : resdic_static_mesh)
    {
        stm.insert(pair.first);
    }
    return stm;
}

set<string> KGCA41B::ResourceMgr::GetTotalANIMID()
{
    set<string> anim;
    for (auto pair : resdic_animation)
    {
        anim.insert(pair.first);
    }
    return anim;
}

void KGCA41B::ResourceMgr::PushStaticMesh(string id, const StaticMesh& static_mesh)
{
    resdic_static_mesh.insert({ id, static_mesh });
}

void KGCA41B::ResourceMgr::PushSkeletalMesh(string id, const SkeletalMesh& skeletal_mesh)
{
    resdic_skeletal_mesh.insert({ id, skeletal_mesh });
}

void KGCA41B::ResourceMgr::PushAnimation(string id, const vector<OutAnimData>& animation)
{
    resdic_animation.insert({ id, animation });
}

bool KGCA41B::ResourceMgr::ImportShaders(string filename)
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

bool KGCA41B::ResourceMgr::ImportSKM(string filename)
{
    SkeletalMesh skeletal_mesh = FBX->LoadSkeletalMesh(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_skeletal_mesh.insert(make_pair(id, skeletal_mesh));

    return false;
}

bool KGCA41B::ResourceMgr::ImportSTM(string filename)
{
    StaticMesh static_mesh = FBX->LoadStaticMesh(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_static_mesh.insert(make_pair(id, static_mesh));

    return false;
}

bool KGCA41B::ResourceMgr::ImportANIM(string filename)
{
    vector<OutAnimData> animation = FBX->LoadAnimation(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_animation.insert(make_pair(id, animation));

    return false;
}

bool ResourceMgr::ImportTexture(string filename)
{
    Texture new_tex;
    new_tex.LoadTextureWIC(to_mw(filename));

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_texture.insert(make_pair(id, new_tex));

    return false;
}
