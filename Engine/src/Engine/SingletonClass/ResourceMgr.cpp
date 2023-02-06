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
    LoadDir(directory_ + "/Shader/VsDefault/", &ResourceMgr::ImportVsDefault);
    LoadDir(directory_ + "/Shader/VsSkinned/", &ResourceMgr::ImportVsSkinned);
    LoadDir(directory_ + "/Shader/PsDefault/", &ResourceMgr::ImportPsDefault);
    LoadDir(directory_ + "/Sound/", &ResourceMgr::ImportSound);
}

void ResourceMgr::LoadDir(string path, Load_Func load_func)
{   
    string tempAdd = path + "*.*";
    intptr_t handle;
    struct _finddata_t fd;
    handle = _findfirst(tempAdd.c_str(), &fd);

    // 못찾으면 리턴
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

//bool ResourceMgr::ImportFbx(string filename)
//{
//    FbxLoader fbx_loader;
//    if (!fbx_loader.LoadFromFbxFile(filename))
//    {
//        fbx_loader.Destroy();
//        return false;
//    }
//
//    vector<SingleMesh<Vertex>> res_static_mesh;
//    vector<SingleMesh<SkinnedVertex>> res_skeletal_mesh;
//    map<UINT, XMMATRIX> res_skeleton;
//    for (auto out_mesh : fbx_loader.out_mesh_list)
//    {
//        if (out_mesh->is_skinned)
//        {
//            SingleMesh<SkinnedVertex> single_mesh;
//            single_mesh.vertices = out_mesh->skinned_vertices;
//            res_skeletal_mesh.push_back(single_mesh);
//            res_skeleton.merge(out_mesh->bind_poses);
//        }
//        else
//        {
//            SingleMesh<Vertex> single_mesh;
//            single_mesh.vertices = out_mesh->vertices;
//            res_static_mesh.push_back(single_mesh);
//        }
//    }
//
//    fbx_loader.LoadAnimation(FbxTime::eFrames60);
//    vector<OutAnimData> res_anim_list;
//    for (auto out_anim : fbx_loader.out_anim_list)
//    {
//        res_anim_list.push_back(*out_anim);
//    }
//
//    for (auto& single_mesh : res_static_mesh)
//    {
//        CreateVertexBuffers(single_mesh);
//    }
//    for (auto& single_mesh : res_skeletal_mesh)
//    {
//        CreateVertexBuffers(single_mesh);
//    }
//
//
//
//    if (res_static_mesh.size() > 0)
//        resdic_static_mesh.insert(make_pair(current_id, res_static_mesh));
//
//    if (res_skeletal_mesh.size() > 0)
//        resdic_skeletal_mesh.insert(make_pair(current_id, res_skeletal_mesh));
//
//    if (res_skeleton.size() > 0)
//        resdic_skeleton.insert(make_pair(current_id, res_skeleton));
//
//    if (res_anim_list.size() > 0)
//        resdic_animation.insert(make_pair(current_id, res_anim_list));
//
//    fbx_loader.Destroy();
//    return true;
//}

bool ResourceMgr::CreateVertexBuffers(SingleMesh<Vertex>& mesh)
{
    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA subdata;

    ZeroMemory(&desc, sizeof(desc));
    ZeroMemory(&subdata, sizeof(subdata));

    desc.ByteWidth = sizeof(Vertex) * mesh.vertices.size();
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    subdata.pSysMem = mesh.vertices.data();

    HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, mesh.vertex_buffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}

bool ResourceMgr::CreateVertexBuffers(SingleMesh<SkinnedVertex>& mesh)
{
    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA subdata;

    ZeroMemory(&desc, sizeof(desc));
    ZeroMemory(&subdata, sizeof(subdata));

    desc.ByteWidth = sizeof(SkinnedVertex) * mesh.vertices.size();
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    subdata.pSysMem = mesh.vertices.data();

    HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, mesh.vertex_buffer.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}

//bool ResourceMgr::ImportVsDefault(string filename)
//{
//    VsDefault vs_default;
//    if (!vs_default.LoadCompiled(str_to_w(filename)))
//        return false;
//
//    resdic_vs_default.insert(make_pair(current_id, vs_default));
//    return true;
//}
//
//bool ResourceMgr::ImportVsSkinned(string filename)
//{
//    VsSkinned vs_skinned;
//    if (!vs_skinned.LoadCompiled(str_to_w(filename)))
//        return false;
//
//    resdic_vs_skinned.insert(make_pair(current_id, vs_skinned));
//    return true;
//}
//
//bool ResourceMgr::ImportPsDefault(string filename)
//{
//    PsDefault ps_default;
//    if (!ps_default.LoadCompiled(str_to_w(filename)))
//        return false;
//
//    resdic_ps_default.insert(make_pair(current_id, ps_default));
//    return true;
//}
//
//bool ResourceMgr::ImportSound(string filename)
//{
//    FMOD::Sound* newSound;
//
//    FMOD_RESULT hr = FMOD_MGR->fmod_system()->createSound(filename.c_str(), (FMOD_MODE)(FMOD_3D), nullptr, &newSound);
//    if (hr != FMOD_OK)
//    {
//        return false;
//    }
//    resdic_sound.insert(make_pair(current_id, newSound));
//    return true;
//}


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
            res_skeletal_mesh.push_back(single_mesh);
            res_skeleton.merge(out_mesh->bind_poses);
        }
        else
        {
            SingleMesh<Vertex> single_mesh;
            single_mesh.vertices = out_mesh->vertices;
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
        CreateVertexBuffers(single_mesh);
    }
    for (auto& single_mesh : res_skeletal_mesh)
    {
        CreateVertexBuffers(single_mesh);
    }



    if (res_static_mesh.size() > 0)
        resdic_static_mesh.insert(make_pair(filename, res_static_mesh));

    if (res_skeletal_mesh.size() > 0)
        resdic_skeletal_mesh.insert(make_pair(filename, res_skeletal_mesh));

    if (res_skeleton.size() > 0)
        resdic_skeleton.insert(make_pair(filename, res_skeleton));

    if (res_anim_list.size() > 0)
        resdic_animation.insert(make_pair(filename, res_anim_list));

    fbx_loader.Destroy();
    return true;
}

bool ResourceMgr::ImportVsDefault(string filename)
{
    VsDefault vs_default;
    if (!vs_default.LoadCompiled(str_to_w(filename)))
        return false;

    resdic_vs_default.insert(make_pair(filename, vs_default));
    return true;
}

bool ResourceMgr::ImportVsSkinned(string filename)
{
    VsSkinned vs_skinned;
    if (!vs_skinned.LoadCompiled(str_to_w(filename)))
        return false;

    resdic_vs_skinned.insert(make_pair(filename, vs_skinned));
    return true;
}

bool ResourceMgr::ImportPsDefault(string filename)
{
    PsDefault ps_default;
    if (!ps_default.LoadCompiled(str_to_w(filename)))
        return false;

    resdic_ps_default.insert(make_pair(filename, ps_default));
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
    resdic_sound.insert(make_pair(filename, newSound));
    return true;
}