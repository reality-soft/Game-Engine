#include "stdafx.h"
#include "StaticMeshLevel.h"
#include "Components.h"
#include "FileTransfer.h"
#include "InputEventMgr.h"

using namespace reality;

reality::StaticMeshLevel::StaticMeshLevel()
{
}

reality::StaticMeshLevel::~StaticMeshLevel()
{
}

bool reality::StaticMeshLevel::Create(string mesh_id, string vs_id, string collision_ltmesh)
{
    level_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>(mesh_id));
    if (level_mesh.get() == nullptr)
        return false;

    vertex_shader = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>(vs_id));
    if (vertex_shader.get() == nullptr)
        return false;

    collision_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>(collision_ltmesh));
    if (collision_mesh.get() == nullptr)
        return false;

    // Create Collision
    for (auto& mesh : collision_mesh.get()->meshes)
    {
        UINT num_triangle = mesh.vertices.size() / 3;
        UINT index = 0;
        for (UINT t = 0; t < num_triangle; t++)
        {
            TriangleShape tri_plane = TriangleShape(
                mesh.vertices[index + 0].p,
                mesh.vertices[index + 2].p,
                mesh.vertices[index + 1].p
            );

            tri_plane.index = t;

            level_triangles.push_back(tri_plane);
            index += 3;
        }        
    }

    return true;
}

void reality::StaticMeshLevel::Update()
{
    if (DINPUT->GetKeyState(DIK_C) == KEY_PUSH)
        view_collision = !view_collision;

    DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
}

void reality::StaticMeshLevel::Render()
{
    DX11APP->GetDeviceContext()->OMSetBlendState(DX11APP->GetCommonStates()->Opaque(), 0, -1);

    DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader.get()->InputLayout());
    DX11APP->GetDeviceContext()->VSSetShader(vertex_shader.get()->Get(), nullptr, 0);

    for (auto& mesh : level_mesh.get()->meshes)
    {
        Material* material = RESOURCE->UseResource<Material>(mesh.mesh_name + ".mat");
        if (material)
            material->Set();

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
    }

    if (view_collision)
        RenderCollisionMesh();
}

void reality::StaticMeshLevel::Destroy()
{
}

void reality::StaticMeshLevel::RenderCollisionMesh()
{
    DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->Wireframe());

    for (auto& mesh : collision_mesh.get()->meshes)
    {
        Material* material = RESOURCE->UseResource<Material>("LevelMat_Collision.mat");
        if (material)
            material->Set();

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);        
    }

    DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->CullNone());
}

StaticMesh* reality::StaticMeshLevel::GetLevelMesh()
{
    return level_mesh.get();
}

bool reality::StaticMeshLevel::SetMaterialToMesh(string mesh_name, string material_id)
{
    auto material = RESOURCE->UseResource<Material>(material_id);
    if (material)
        mesh_material_map.insert(make_pair(mesh_name, shared_ptr<Material>(material)));
    else
        return false;

    return true;
}

bool reality::StaticMeshLevel::SetRandomMaterialToMesh(string mesh_name, string keyward)
{
    vector<string> materials_by_keyward;
    auto mat_ids = RESOURCE->GetTotalMATID();
    for (const auto& id : mat_ids)
    {
        if (id.find(keyward) != string::npos)
            materials_by_keyward.push_back(id);
    }

    if (materials_by_keyward.empty())
        return false;

    UINT num_material = materials_by_keyward.size();
    UINT random_index = (UINT)(rand() % num_material);
    SetMaterialToMesh(mesh_name, materials_by_keyward[random_index]);

    return true;
}

void reality::StaticMeshLevel::ImportGuideLines(string mapdat_file, GuideLine::GuideType guide_type)
{
    FileTransfer out_mapdata(mapdat_file, READ);

    UINT num_guide_lines = 0;
    out_mapdata.ReadBinary<UINT>(num_guide_lines);
    for (UINT i = 0; i < num_guide_lines; ++i)
    {
        GuideLine new_guide_line;
        new_guide_line.guide_type_ = guide_type;

        UINT num_nodes = 0;
        out_mapdata.ReadBinary<UINT>(num_nodes);

        for (UINT j = 0; j < num_nodes; ++j)
        {
            UINT node_number;
            XMVECTOR node_pos;
            out_mapdata.ReadBinary<UINT>(node_number);
            out_mapdata.ReadBinary<XMVECTOR>(node_pos);

            new_guide_line.AddNode(node_pos);
        }

        guide_lines.push_back(new_guide_line);
    }
}

vector<GuideLine> reality::StaticMeshLevel::GetGuideLines(GuideLine::GuideType guide_type)
{
    vector<GuideLine> guides;
    for (const auto& gd : guide_lines)
    {
        if (gd.guide_type_ == guide_type)
            guides.push_back(gd);
    }
    return guides;
}
