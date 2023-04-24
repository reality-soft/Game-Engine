#include "stdafx.h"
#include "FbxMgr.h"
#include "FileTransfer.h"
#include "ResourceMgr.h"
#include "Dx11App.h"

#ifdef _DEBUG
bool reality::FbxMgr::ImportAndSaveFbx(string filename, FbxImportOption options, FbxVertexOption vertex_option)
{
    FbxLoader fbx_loader;
    fbx_loader.import_options = options;
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
            single_mesh.mesh_name = out_mesh->mesh_name;

            switch (vertex_option) {
            case FbxVertexOption::BY_CONTROL_POINT:
                single_mesh.vertices = out_mesh->skinned_vertices_by_control_point;
                single_mesh.indices = out_mesh->indices;
                break;
            case FbxVertexOption::BY_POLYGON_VERTEX:
                single_mesh.vertices = out_mesh->skinned_vertices_by_polygon_vertex;
                break;
            }

            res_skeletal_mesh.meshes.push_back(single_mesh);
            res_skeletal_mesh.skeleton.bind_pose_matrices.merge(out_mesh->bind_poses);

            for (auto name_bone : out_mesh->name_bone_map) {
                res_skeletal_mesh.skeleton.id_bone_map.insert({ name_bone.second.bone_id, name_bone.second });
                res_skeletal_mesh.skeleton.bone_name_id_map.insert({ name_bone.first, name_bone.second.bone_id });
            }
        }
        else
        {
            SingleMesh<Vertex> single_mesh;
            single_mesh.mesh_name = out_mesh->mesh_name;

            switch (vertex_option) {
            case FbxVertexOption::BY_CONTROL_POINT:
                single_mesh.vertices = out_mesh->vertices_by_control_point;
                single_mesh.indices = out_mesh->indices;
                break;
            case FbxVertexOption::BY_POLYGON_VERTEX:
                single_mesh.vertices = out_mesh->vertices_by_polygon_vertex;
                break;
            }

            res_static_mesh.meshes.push_back(single_mesh);
        }
    }

    if (options.recalculate_normal)
    {
        if (res_static_mesh.meshes.size() > 0)
        {
            ReCalculateNormal(res_static_mesh, vertex_option);
        }
        else if (res_skeletal_mesh.meshes.size() > 0)
        {
            ReCalculateNormal(res_skeletal_mesh, vertex_option);
        }  
    }


    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];
    strs = split(id, '\\');
    id = strs[strs.size() - 1];
    strs = split(id, '.');
    string name = strs[0];

    fbx_loader.LoadAnimation(FbxTime::eFrames60, name);
    for (auto& single_mesh : res_static_mesh.meshes)
    {
        CreateBuffers(single_mesh);
    }
    for (auto& single_mesh : res_skeletal_mesh.meshes)
    {
        CreateBuffers(single_mesh);
    }

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

    if (fbx_loader.out_anim_map.size() > 0)
    {
        RESOURCE->PushAnimation(fbx_loader.out_anim_map);
        SaveAnimation(fbx_loader.out_anim_map);
    }

    fbx_loader.Destroy();
    return true;
}
#endif

void reality::FbxMgr::SaveStaticMesh(const StaticMesh& static_mesh, string filename)
{
    string file_name = RESOURCE->directory() + "STM/" + filename + ".stmesh";
    FileTransfer file_exporter(file_name, WRITE);

    int num_of_meshes = static_mesh.meshes.size();

    file_exporter.WriteBinaryWithoutSize<int>(&num_of_meshes, 1);

    for (int cur_mesh_index = 0;cur_mesh_index < num_of_meshes;cur_mesh_index++) {
        string mesh_name = static_mesh.meshes[cur_mesh_index].mesh_name;
        int mesh_name_size = mesh_name.size() + 1;
        file_exporter.WriteBinaryWithoutSize<int>(&mesh_name_size, 1);
        file_exporter.WriteBinaryWithoutSize<char>(const_cast<char*>(mesh_name.c_str()), mesh_name_size);

        int num_of_vertices = static_mesh.meshes[cur_mesh_index].vertices.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_vertices, 1);
        file_exporter.WriteBinaryWithoutSize<Vertex>(const_cast<Vertex*>(static_mesh.meshes[cur_mesh_index].vertices.data()), num_of_vertices);

        int num_of_indices = static_mesh.meshes[cur_mesh_index].indices.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_indices, 1);
        file_exporter.WriteBinaryWithoutSize<UINT>(const_cast<UINT*>(static_mesh.meshes[cur_mesh_index].indices.data()), num_of_indices);
    }
}

void reality::FbxMgr::SaveSkeletalMesh(const SkeletalMesh& skeletal_mesh, string filename)
{
    string file_name = RESOURCE->directory() + "SKM/" + filename + ".skmesh";
    FileTransfer file_exporter(file_name, WRITE);

    int num_of_meshes = skeletal_mesh.meshes.size();

    file_exporter.WriteBinaryWithoutSize<int>(&num_of_meshes, 1);

    for (int cur_mesh_index = 0;cur_mesh_index < num_of_meshes;cur_mesh_index++) {
        string mesh_name = skeletal_mesh.meshes[cur_mesh_index].mesh_name;
        int mesh_name_size = mesh_name.size() + 1;
        file_exporter.WriteBinaryWithoutSize<int>(&mesh_name_size, 1);
        file_exporter.WriteBinaryWithoutSize<char>(const_cast<char*>(mesh_name.c_str()), mesh_name_size);

        int num_of_vertices = skeletal_mesh.meshes[cur_mesh_index].vertices.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_vertices, 1);
        file_exporter.WriteBinaryWithoutSize<SkinnedVertex>(const_cast<SkinnedVertex*>(skeletal_mesh.meshes[cur_mesh_index].vertices.data()), num_of_vertices);

        int num_of_indices = skeletal_mesh.meshes[cur_mesh_index].indices.size();
        file_exporter.WriteBinaryWithoutSize<int>(&num_of_indices, 1);
        file_exporter.WriteBinaryWithoutSize<UINT>(const_cast<UINT*>(skeletal_mesh.meshes[cur_mesh_index].indices.data()), num_of_indices);
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

    vector<UINT> bone_ids;
    vector<Bone> bones;

    for (const auto& cur_pair : skeletal_mesh.skeleton.id_bone_map) {
        bone_ids.push_back(cur_pair.first);
        bones.push_back(cur_pair.second);
    }

    int num_of_bone_ids = bone_ids.size();
    file_exporter.WriteBinaryWithoutSize<int>(&num_of_bone_ids, 1);
    file_exporter.WriteBinaryWithoutSize<UINT>(bone_ids.data(), num_of_bone_ids);

    int num_of_bones = bones.size();
    file_exporter.WriteBinaryWithoutSize<int>(&num_of_bones, 1);
    for (int i = 0;i < num_of_bones;i++) {
        file_exporter.WriteBinaryWithoutSize<UINT>(&(bones[i].bone_id), 1);
        file_exporter.WriteBinaryWithoutSize<UINT>(&(bones[i].parent_bone_id), 1);
        int num_child_ids = bones[i].child_bone_ids.size();
        file_exporter.WriteBinaryWithoutSize<int>(&(num_child_ids), 1);
        file_exporter.WriteBinaryWithoutSize<UINT>(bones[i].child_bone_ids.data(), num_child_ids);
    }

    vector<string> skeleton_names;
    bone_ids.clear();

    for (const auto& cur_pair : skeletal_mesh.skeleton.bone_name_id_map) {
        skeleton_names.push_back(cur_pair.first);
        bone_ids.push_back(cur_pair.second);
    }

    int num_of_names = skeleton_names.size();
    file_exporter.WriteBinaryWithoutSize<int>(&num_of_names, 1);
    for (auto skeleton_name : skeleton_names) {
        int skeleton_name_size = skeleton_name.size() + 1;
        file_exporter.WriteBinaryWithoutSize<int>(&skeleton_name_size, 1);
        file_exporter.WriteBinaryWithoutSize<char>(const_cast<char*>(skeleton_name.c_str()), skeleton_name_size);
    }

    num_of_bone_ids = bone_ids.size();
    file_exporter.WriteBinaryWithoutSize<int>(&num_of_bone_ids, 1);
    file_exporter.WriteBinaryWithoutSize<UINT>(bone_ids.data(), num_of_bone_ids);
}

void reality::FbxMgr::SaveAnimation(const map<string, OutAnimData>& animation)
{
    for (const auto& cur_anim : animation)
    {
        string file_name = RESOURCE->directory() + "ANIM/" + cur_anim.first;
        FileTransfer file_exporter(file_name, WRITE);

        vector<UINT> keys;
        vector<vector<XMMATRIX>> animation_matrices;

        for (const auto& cur_pair : cur_anim.second.animation_matrices) {
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

        UINT start_frame = cur_anim.second.start_frame;
        file_exporter.WriteBinaryWithoutSize<UINT>(&start_frame, 1);
        UINT end_frame = cur_anim.second.end_frame;
        file_exporter.WriteBinaryWithoutSize<UINT>(&end_frame, 1);
    }
}

reality::StaticMesh reality::FbxMgr::LoadStaticMesh(string filename)
{
    FileTransfer file_exporter(filename, READ);

    StaticMesh static_mesh;

    int num_of_meshes = file_exporter.ReadBinaryWithoutSize<int>(1)[0];

    static_mesh.meshes.resize(num_of_meshes);

    for (int cur_mesh_index = 0;cur_mesh_index < num_of_meshes;cur_mesh_index++) {
        int mesh_name_size = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        static_mesh.meshes[cur_mesh_index].mesh_name = file_exporter.ReadBinaryWithoutSize<char>(mesh_name_size).data();

        int num_of_vertices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        static_mesh.meshes[cur_mesh_index].vertices = file_exporter.ReadBinaryWithoutSize<Vertex>(num_of_vertices);

        int num_of_indices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        static_mesh.meshes[cur_mesh_index].indices = file_exporter.ReadBinaryWithoutSize<UINT>(num_of_indices);

        CreateBuffers(static_mesh.meshes[cur_mesh_index]);
    }

    return static_mesh;
}

reality::SkeletalMesh reality::FbxMgr::LoadSkeletalMesh(string filename)
{
    FileTransfer file_exporter(filename, READ);

    SkeletalMesh skeletal_mesh;

    int num_of_meshes = file_exporter.ReadBinaryWithoutSize<int>(1)[0];

    skeletal_mesh.meshes.resize(num_of_meshes);

    for (int cur_mesh_index = 0;cur_mesh_index < num_of_meshes;cur_mesh_index++) {
        int mesh_name_size = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        skeletal_mesh.meshes[cur_mesh_index].mesh_name = file_exporter.ReadBinaryWithoutSize<char>(mesh_name_size).data();
        
        int num_of_vertices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        skeletal_mesh.meshes[cur_mesh_index].vertices = file_exporter.ReadBinaryWithoutSize<SkinnedVertex>(num_of_vertices);

        int num_of_indices = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        skeletal_mesh.meshes[cur_mesh_index].indices = file_exporter.ReadBinaryWithoutSize<UINT>(num_of_indices);
        
        CreateBuffers(skeletal_mesh.meshes[cur_mesh_index]);
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

    vector<UINT> bone_ids;
    vector<Bone> bones;

    int num_bone_ids = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
    bone_ids = file_exporter.ReadBinaryWithoutSize<UINT>(num_bone_ids);

    int num_of_bones = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
    for (int i = 0;i < num_of_bones;i++) {
        Bone bone;
        bone.bone_id = file_exporter.ReadBinaryWithoutSize<UINT>(1)[0];
        bone.parent_bone_id = file_exporter.ReadBinaryWithoutSize<UINT>(1)[0];
        int num_child_ids = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        bone.child_bone_ids = file_exporter.ReadBinaryWithoutSize<UINT>(num_child_ids);
        bones.push_back(bone);
    }

    for (int i = 0;i < num_bone_ids;i++) {
        skeletal_mesh.skeleton.id_bone_map.insert({ bone_ids[i], bones[i] });
    }

    vector<string> bone_names;
    bone_ids.clear();

    int num_of_names = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
    for (int i = 0;i < num_of_names;i++) {
        int skeleton_name_size = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
        bone_names.push_back(file_exporter.ReadBinaryWithoutSize<char>(skeleton_name_size).data());
    }

    num_bone_ids = file_exporter.ReadBinaryWithoutSize<int>(1)[0];
    bone_ids = file_exporter.ReadBinaryWithoutSize<UINT>(num_bone_ids);

    for (int i = 0;i < num_bone_ids;i++) {
        skeletal_mesh.skeleton.bone_name_id_map.insert({ bone_names[i], bone_ids[i] });
    }

    return skeletal_mesh;
}

reality::OutAnimData reality::FbxMgr::LoadAnimation(string filename)
{
    OutAnimData animation_data;

    FileTransfer file_exporter(filename, READ);

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
        animation_data.animation_matrices.insert({ keys[i], animation_matrices[i] });
    }

    animation_data.start_frame = file_exporter.ReadBinaryWithoutSize<UINT>(1)[0];
    animation_data.end_frame = file_exporter.ReadBinaryWithoutSize<UINT>(1)[0];

    return animation_data;
}

bool reality::FbxMgr::CreateBuffers(SingleMesh<Vertex>& mesh)
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

bool reality::FbxMgr::CreateBuffers(SingleMesh<SkinnedVertex>& mesh) {
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
    if (mesh.indices.size() != 0) {
        ZeroMemory(&desc, sizeof(desc));
        ZeroMemory(&subdata, sizeof(subdata));

        desc.ByteWidth = sizeof(UINT) * mesh.indices.size();
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subdata.pSysMem = mesh.indices.data();

        hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, mesh.index_buffer.GetAddressOf());
        if (FAILED(hr))
            return false;
    }

    return true;
}

#ifdef _DEBUG
void reality::FbxMgr::ReCalculateNormal(StaticMesh& static_mesh, FbxVertexOption vertex_option)
{
    for (auto& mesh : static_mesh.meshes)
    {
        UINT num_triangle;
        UINT index = 0;
        switch (vertex_option) {
        case FbxVertexOption::BY_CONTROL_POINT:
            num_triangle = mesh.indices.size() / 3;
            break;
        case FbxVertexOption::BY_POLYGON_VERTEX:
            num_triangle = mesh.vertices.size() / 3;
            break;
        }

        for (UINT t = 0; t < num_triangle; t++)
        {
            XMFLOAT3 v0;
            XMFLOAT3 v1;
            XMFLOAT3 v2;

            switch (vertex_option) {
            case FbxVertexOption::BY_CONTROL_POINT:
                v0 = mesh.vertices[mesh.indices[index + 0]].p;
                v1 = mesh.vertices[mesh.indices[index + 1]].p;
                v2 = mesh.vertices[mesh.indices[index + 2]].p;
                break;

            case FbxVertexOption::BY_POLYGON_VERTEX:
                v0 = mesh.vertices[index + 0].p;
                v1 = mesh.vertices[index + 1].p;
                v2 = mesh.vertices[index + 2].p;
                break;
            }

            XMVECTOR edge1 = XMLoadFloat3(&v2) - XMLoadFloat3(&v0);
            XMVECTOR edge2 = XMLoadFloat3(&v1) - XMLoadFloat3(&v0);

            XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

            switch (vertex_option) {
            case FbxVertexOption::BY_CONTROL_POINT:
                mesh.vertices[mesh.indices[index + 0]].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[mesh.indices[index + 1]].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[mesh.indices[index + 2]].n = XMFLOAT3(normal.m128_f32);
                break;

            case FbxVertexOption::BY_POLYGON_VERTEX:
                mesh.vertices[index + 0].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[index + 1].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[index + 2].n = XMFLOAT3(normal.m128_f32);
                break;
            }


            index += 3;
        }
    }
}

void reality::FbxMgr::ReCalculateNormal(SkeletalMesh& skeletal_mesh, FbxVertexOption vertex_option)
{
    for (auto& mesh : skeletal_mesh.meshes)
    {
        UINT num_triangle;
        UINT index = 0;
        switch (vertex_option) {
        case FbxVertexOption::BY_CONTROL_POINT:
            num_triangle = mesh.indices.size();
            break;
        case FbxVertexOption::BY_POLYGON_VERTEX:
            num_triangle = mesh.vertices.size() / 3;
            break;
        }

        for (UINT t = 0; t < num_triangle; t++)
        {
            XMFLOAT3 v0;
            XMFLOAT3 v1;
            XMFLOAT3 v2;

            switch (vertex_option) {
            case FbxVertexOption::BY_CONTROL_POINT:
                v0 = mesh.vertices[mesh.indices[index + 0]].p;
                v1 = mesh.vertices[mesh.indices[index + 1]].p;
                v2 = mesh.vertices[mesh.indices[index + 2]].p;
                break;

            case FbxVertexOption::BY_POLYGON_VERTEX:
                v0 = mesh.vertices[index + 0].p;
                v1 = mesh.vertices[index + 1].p;
                v2 = mesh.vertices[index + 2].p;
                break;
            }

            XMVECTOR edge1 = XMLoadFloat3(&v2) - XMLoadFloat3(&v0);
            XMVECTOR edge2 = XMLoadFloat3(&v1) - XMLoadFloat3(&v0);

            XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

            switch (vertex_option) {
            case FbxVertexOption::BY_CONTROL_POINT:
                mesh.vertices[mesh.indices[index + 0]].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[mesh.indices[index + 1]].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[mesh.indices[index + 2]].n = XMFLOAT3(normal.m128_f32);
                break;

            case FbxVertexOption::BY_POLYGON_VERTEX:
                mesh.vertices[index + 0].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[index + 1].n = XMFLOAT3(normal.m128_f32);
                mesh.vertices[index + 2].n = XMFLOAT3(normal.m128_f32);
                break;
            }
            index += 3;
        }
    }
}
#endif