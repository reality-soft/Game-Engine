#include "stdafx.h"
#include "DataTypes.h"
#include "FbxMgr.h"
#include "FileTransfer.h"
#include "ResourceMgr.h"
#include "Dx11App.h"

bool KGCA41B::FbxMgr::ImportAndSaveFbx(string filename)
{
    FbxLoader fbx_loader;
    if (!fbx_loader.LoadFromFbxFile(filename))
    {
        fbx_loader.Destroy();
        return false;
    }

    StaticMesh res_static_mesh;
    SkeletalMesh res_skeletal_mesh;
    for (auto out_mesh : fbx_loader.out_mesh_list)
    {
        if (out_mesh->is_skinned)
        {
            SingleMesh<SkinnedVertex> single_mesh;
            single_mesh.vertices = out_mesh->skinned_vertices;
            single_mesh.indices = out_mesh->indices;
            res_skeletal_mesh.meshes.push_back(single_mesh);
            res_skeletal_mesh.skeleton.bind_pose_matrices.merge(out_mesh->bind_poses);
        }
        else
        {
            SingleMesh<Vertex> single_mesh;
            single_mesh.vertices = out_mesh->vertices;
            single_mesh.indices = out_mesh->indices;
            res_static_mesh.meshes.push_back(single_mesh);
        }
    }

    fbx_loader.LoadAnimation(FbxTime::eFrames60);
    vector<OutAnimData> res_anim_list;
    for (auto out_anim : fbx_loader.out_anim_list)
    {
        res_anim_list.push_back(*out_anim);
    }

    for (auto& single_mesh : res_static_mesh.meshes)
    {
        CreateBuffers(single_mesh);
    }
    for (auto& single_mesh : res_skeletal_mesh.meshes)
    {
        CreateBuffers(single_mesh);
    }

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];
    strs = split(filename, '\\');
    id = strs[strs.size() - 1];
    strs = split(filename, '.');
    filename = strs[0];

    if (res_static_mesh.meshes.size() > 0) 
    {
        RESOURCE->PushStaticMesh(filename + ".stmesh", res_static_mesh);
        SaveStaticMesh(res_static_mesh, filename);
    }

    if (res_skeletal_mesh.meshes.size() > 0) 
    {
        RESOURCE->PushSkeletalMesh(filename + ".skmesh", res_skeletal_mesh);
        SaveSkeletalMesh(res_skeletal_mesh, filename);
    }

    if (res_anim_list.size() > 0) 
    {
        RESOURCE->PushAnimation(filename + ".anim", res_anim_list);
        SaveAnimation(res_anim_list, filename);
    }

    fbx_loader.Destroy();
    return true;
}

void KGCA41B::FbxMgr::SaveStaticMesh(const StaticMesh& static_mesh, string filename)
{

}

void KGCA41B::FbxMgr::SaveSkeletalMesh(const SkeletalMesh& skeletal_mesh, string filename)
{
    FileTransfer file_exporter(RESOURCE->directory() + "/SKM/" + filename + ".skmesh", WRITE);

    int num_of_meshes = skeletal_mesh.meshes.size();

    file_exporter.WriteBinary<int>(&num_of_meshes, 1);

    for (int cur_mesh_index = 0;cur_mesh_index < num_of_meshes;cur_mesh_index++) {
        int num_of_vertices = skeletal_mesh.meshes[cur_mesh_index].vertices.size();
        file_exporter.WriteBinary<int>(&num_of_vertices, 1);
        file_exporter.WriteBinary<SkinnedVertex>(skeletal_mesh.meshes[cur_mesh_index].vertices.data(), num_of_vertices);

        int num_of_indices = skeletal_mesh.meshes[cur_mesh_index].indices.size();
        file_exporter.WriteBinary<int>(&num_of_indices, 1);
        file_exporter.WriteBinary<UINT>(skeletal_mesh.meshes[cur_mesh_index].indices.data(), num_of_indices);
    }
}

void KGCA41B::FbxMgr::SaveAnimation(const vector<OutAnimData>& animation, string filename)
{
}

void KGCA41B::FbxMgr::LoadStaticMesh(string filename)
{
}

void KGCA41B::FbxMgr::LoadSkeletalMesh(string filename)
{
}

void KGCA41B::FbxMgr::LoadAnimation(string filename)
{
}

bool KGCA41B::FbxMgr::CreateBuffers(SingleMesh<Vertex>& mesh)
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

bool KGCA41B::FbxMgr::CreateBuffers(SingleMesh<SkinnedVertex>& mesh) {
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
