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

            single_mesh.mesh_name = out_mesh->mesh_name;
        }
        else
        {
            SingleMesh<Vertex> single_mesh;
            single_mesh.vertices = out_mesh->vertices;
            single_mesh.indices = out_mesh->indices;
            res_static_mesh.meshes.push_back(single_mesh);

            single_mesh.mesh_name = out_mesh->mesh_name;
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
    strs = split(id, '\\');
    id = strs[strs.size() - 1];
    strs = split(id, '.');
    string name = strs[0];

    if (res_static_mesh.meshes.size() > 0) 
    {
        RESOURCE->PushStaticMesh(name + ".stmesh", res_static_mesh);
        SaveStaticMesh(res_static_mesh, name);
    }

    if (res_skeletal_mesh.meshes.size() > 0) 
    {

        RESOURCE->PushSkeletalMesh(name + ".skmesh", res_skeletal_mesh);
        SaveSkeletalMesh(res_skeletal_mesh, name);
    }

    if (res_anim_list.size() > 0) 
    {
        RESOURCE->PushAnimation(name + ".anim", res_anim_list);
        SaveAnimation(res_anim_list, name);
    }

    fbx_loader.Destroy();
    return true;
}

void KGCA41B::FbxMgr::SaveStaticMesh(const StaticMesh& static_mesh, string filename)
{

}

void KGCA41B::FbxMgr::SaveSkeletalMesh(const SkeletalMesh& skeletal_mesh, string filename)
{
    string file_name = RESOURCE->directory() + "SKM/" + filename + ".skmesh";
    FileTransfer file_exporter(file_name, WRITE);

    int num_of_meshes = skeletal_mesh.meshes.size();

    file_exporter.WriteBinaryWithoutSize<int>(&num_of_meshes, 1);

    for (int cur_mesh_index = 0;cur_mesh_index < num_of_meshes;cur_mesh_index++) {
        int num_of_vertices = skeletal_mesh.meshes[cur_mesh_index].vertices.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_vertices, 1);
        file_exporter.WriteBinaryWithoutSize<SkinnedVertex>(const_cast<SkinnedVertex*>(skeletal_mesh.meshes[cur_mesh_index].vertices.data()), num_of_vertices);

        int num_of_indices = skeletal_mesh.meshes[cur_mesh_index].indices.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_indices, 1);
        file_exporter.WriteBinaryWithoutSize<UINT>(const_cast<UINT*>(skeletal_mesh.meshes[cur_mesh_index].indices.data()), num_of_indices);

        //string shader_id = skeletal_mesh.meshes[cur_mesh_index].material.shader_id;
        //string texture_id = skeletal_mesh.meshes[cur_mesh_index].material.texture_id;
        //int shader_id_size = shader_id.size() + 1;
        //int texture_id_size = texture_id.size() + 1;
        //file_exporter.WriteBinaryWithoutSize<int>(&shader_id_size, 1);
        //file_exporter.WriteBinaryWithoutSize<char>(const_cast<char*>(shader_id.c_str()), shader_id_size);
        //file_exporter.WriteBinaryWithoutSize<int>(&texture_id_size, 1);
        //file_exporter.WriteBinaryWithoutSize<char>(const_cast<char*>(texture_id.c_str()), texture_id_size);
    }

    vector<UINT> keys;
    vector<XMMATRIX> bind_pose_matrices;

    for (const auto& cur_pair : skeletal_mesh.skeleton.bind_pose_matrices) {
        keys.push_back(cur_pair.first);
        bind_pose_matrices.push_back(cur_pair.second);
    }

    int num_of_keys = keys.size();
    file_exporter.WriteBinaryWithoutSize<int>(&num_of_keys, 1);
    file_exporter.WriteBinaryWithoutSize<UINT>(keys.data(), num_of_keys);

    int num_of_matrices = bind_pose_matrices.size();
    file_exporter.WriteBinaryWithoutSize<int>(&num_of_matrices, 1);
    file_exporter.WriteBinaryWithoutSize<XMMATRIX>(bind_pose_matrices.data(), num_of_matrices); 
}

void KGCA41B::FbxMgr::SaveAnimation(const vector<OutAnimData>& animation, string filename)
{
    string file_name = RESOURCE->directory() + "ANIM/" + filename + ".anim";
    FileTransfer file_exporter(file_name, WRITE);

    int num_of_anim = animation.size();

    file_exporter.WriteBinaryWithoutSize<int>(&num_of_anim, 1);

    for (int cur_anim_index = 0;cur_anim_index < num_of_anim;cur_anim_index++) {
        vector<UINT> keys;
        vector<vector<XMMATRIX>> animation_matrices;

        for (const auto& cur_pair : animation[cur_anim_index].animations) {
            keys.push_back(cur_pair.first);
            animation_matrices.push_back(cur_pair.second);
        }

        int num_of_keys = keys.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_keys, 1);
        file_exporter.WriteBinaryWithoutSize<UINT>(keys.data(), num_of_keys);

        int num_of_animation = animation_matrices.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_animation, 1);

        for (int cur_animation_index = 0;cur_animation_index < num_of_animation;cur_animation_index++) {
            int num_of_matrices = animation_matrices[cur_animation_index].size();
            file_exporter.WriteBinaryWithoutSize<int>(&num_of_matrices, 1);
            file_exporter.WriteBinaryWithoutSize<XMMATRIX>(animation_matrices[cur_animation_index].data(), num_of_matrices);
        }

        UINT start_frame = animation[cur_anim_index].start_frame;
        file_exporter.WriteBinaryWithoutSize<UINT>(&start_frame, 1);
        UINT end_frame = animation[cur_anim_index].end_frame;
        file_exporter.WriteBinaryWithoutSize<UINT>(&end_frame, 1);
    }
}

KGCA41B::StaticMesh KGCA41B::FbxMgr::LoadStaticMesh(string filename)
{
    return StaticMesh();
}

KGCA41B::SkeletalMesh KGCA41B::FbxMgr::LoadSkeletalMesh(string filename)
{
    FileTransfer file_exporter(filename, READ);

    SkeletalMesh skeletal_mesh;

    int num_of_meshes = file_exporter.ReadBinaryWithoutSize<int>(1)[0];

    skeletal_mesh.meshes.resize(num_of_meshes);

    for (int cur_mesh_index = 0;cur_mesh_index < num_of_meshes;cur_mesh_index++) {
        int num_of_vertices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        skeletal_mesh.meshes[cur_mesh_index].vertices = file_exporter.ReadBinaryWithoutSize<SkinnedVertex>(num_of_vertices);

        int num_of_indices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        skeletal_mesh.meshes[cur_mesh_index].indices = file_exporter.ReadBinaryWithoutSize<UINT>(num_of_indices);
        
        CreateBuffers(skeletal_mesh.meshes[cur_mesh_index]);

        //int shader_id_size = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        //skeletal_mesh.meshes[cur_mesh_index].material.shader_id = file_exporter.ReadBinaryWithoutSize<char>(shader_id_size).data();

        //int texture_id_size = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        //skeletal_mesh.meshes[cur_mesh_index].material.texture_id = file_exporter.ReadBinaryWithoutSize<char>(texture_id_size).data();
    }

    vector<UINT> keys;
    vector<XMMATRIX> bind_pose_matrices;

    int num_of_keys = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
    keys = file_exporter.ReadBinaryWithoutSize<UINT>(num_of_keys);

    int num_of_matrices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
    bind_pose_matrices = file_exporter.ReadBinaryWithoutSize<XMMATRIX>(num_of_keys);

    for (int i = 0;i < num_of_keys;i++) {
        skeletal_mesh.skeleton.bind_pose_matrices.insert({ keys[i], bind_pose_matrices[i] });
    }

    return skeletal_mesh;
}

vector<KGCA41B::OutAnimData> KGCA41B::FbxMgr::LoadAnimation(string filename)
{
    vector<OutAnimData> animation_data;

    FileTransfer file_exporter(filename, READ);

    int num_of_anim = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
    animation_data.resize(num_of_anim);

    for (int cur_anim_index = 0;cur_anim_index < num_of_anim;cur_anim_index++) {
        vector<UINT> keys;
        vector<vector<XMMATRIX>> animation_matrices;

        int num_of_keys = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        keys.resize(num_of_keys);
        keys = file_exporter.ReadBinaryWithoutSize<UINT>(num_of_keys);

        int num_of_animation = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        animation_matrices.resize(num_of_animation);

        for (int cur_animation_index = 0;cur_animation_index < num_of_animation;cur_animation_index++) {
            int num_of_matrices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
            animation_matrices[cur_animation_index].resize(num_of_matrices);
            animation_matrices[cur_animation_index] = file_exporter.ReadBinaryWithoutSize<XMMATRIX>(num_of_matrices);
        }

        for (int i = 0;i < num_of_keys;i++) {
            animation_data[cur_anim_index].animations.insert({ keys[i], animation_matrices[i] });
        }

        animation_data[cur_anim_index].start_frame = file_exporter.ReadBinaryWithoutSize<UINT>(1)[0];
        animation_data[cur_anim_index].end_frame = file_exporter.ReadBinaryWithoutSize<UINT>(1)[0];
    }

    return animation_data;
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
