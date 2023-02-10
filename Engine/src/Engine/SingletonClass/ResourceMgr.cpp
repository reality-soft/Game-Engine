#include "ResourceMgr.h"
#include "FmodMgr.h"
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
    LoadDir(directory_ + "/FBX/", &ResourceMgr::ImportFbx);
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
    for (auto res : resdic_skeleton)
    {
        res_id_map.insert(make_pair("[SKT]" + res.first, "SKT"));
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

bool ResourceMgr::CreateBuffers(SingleMesh<Vertex>& mesh)
{
    HRESULT hr;

    // VertexBuffer

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA subdata;

    ZeroMemory(&desc, sizeof(desc));
    ZeroMemory(&subdata, sizeof(subdata));

    desc.ByteWidth = sizeof(Vertex) * mesh.vertices.size();
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    subdata.pSysMem = mesh.vertices.data();

    hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, mesh.vertex_buffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    // IndexBuffer

    ZeroMemory(&desc, sizeof(desc));
    ZeroMemory(&subdata, sizeof(subdata));

    desc.ByteWidth = sizeof(UINT) * mesh.indices.size();
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subdata.pSysMem = mesh.indices.data();

    hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, mesh.index_buffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}

bool ResourceMgr::CreateBuffers(SingleMesh<SkinnedVertex>& mesh)
{
    HRESULT hr;

    // VertexBuffer

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA subdata;

    ZeroMemory(&desc, sizeof(desc));
    ZeroMemory(&subdata, sizeof(subdata));

    desc.ByteWidth = sizeof(SkinnedVertex) * mesh.vertices.size();
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    subdata.pSysMem = mesh.vertices.data();

    hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, mesh.vertex_buffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    // IndexBuffer

    ZeroMemory(&desc, sizeof(desc));
    ZeroMemory(&subdata, sizeof(subdata));

    desc.ByteWidth = sizeof(UINT) * mesh.indices.size();
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subdata.pSysMem = mesh.indices.data();

    hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, mesh.index_buffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}

bool ResourceMgr::ImportFbx(string filename)
{
    FbxLoader fbx_loader;
    if (!fbx_loader.LoadFromFbxFile(filename))
    {
        fbx_loader.Destroy();
        return false;
    }

    vector<SingleMesh<Vertex>> res_static_mesh;
    vector<SingleMesh<SkinnedVertex>> res_skeletal_mesh;
    map<UINT, XMMATRIX> res_skeleton;
    for (auto out_mesh : fbx_loader.out_mesh_list)
    {
        if (out_mesh->is_skinned)
        { 
            SingleMesh<SkinnedVertex> single_mesh;
            single_mesh.vertices = out_mesh->skinned_vertices;
            single_mesh.indices = out_mesh->indices;
            res_skeletal_mesh.push_back(single_mesh);
            res_skeleton.merge(out_mesh->bind_poses);
        }
        else
        {
            SingleMesh<Vertex> single_mesh;
            single_mesh.vertices = out_mesh->vertices;
            single_mesh.indices = out_mesh->indices;
            res_static_mesh.push_back(single_mesh);
        }
    }

    fbx_loader.LoadAnimation(FbxTime::eFrames60);
    vector<OutAnimData> res_anim_list;
    for (auto out_anim : fbx_loader.out_anim_list)
    {
        res_anim_list.push_back(*out_anim);
    }

    for (auto& single_mesh : res_static_mesh)
    {
        CreateBuffers(single_mesh);
    }
    for (auto& single_mesh : res_skeletal_mesh)
    {
        CreateBuffers(single_mesh);
    }

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    if (res_static_mesh.size() > 0)
        resdic_static_mesh.insert(make_pair(id, res_static_mesh));

    if (res_skeletal_mesh.size() > 0)
        resdic_skeletal_mesh.insert(make_pair(id, res_skeletal_mesh));

    if (res_skeleton.size() > 0)
        resdic_skeleton.insert(make_pair(id, res_skeleton));

    if (res_anim_list.size() > 0)
        resdic_animation.insert(make_pair(id, res_anim_list));

    fbx_loader.Destroy();
    return true;
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

bool ResourceMgr::ImportTexture(string filename)
{
    Texture new_tex;
    new_tex.LoadTextureWIC(to_mw(filename));

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_texture.insert(make_pair(id, new_tex));

    return false;
}
