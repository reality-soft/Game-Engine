#include "stdafx.h"
#include "LightMeshLevel.h"
#include "Components.h"
#include "FileTransfer.h"
#include "InputEventMgr.h"

using namespace reality;

reality::LightMeshLevel::LightMeshLevel()
{
}

reality::LightMeshLevel::~LightMeshLevel()
{
}

bool reality::LightMeshLevel::Create(string mesh_id, string vs_id, string gs_id, string collision_ltmesh)
{
    level_mesh = shared_ptr<LightMesh>(RESOURCE->UseResource<LightMesh>(mesh_id));
    if (level_mesh.get() == nullptr)
        return false;

    vertex_shader = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>(vs_id));
    if (vertex_shader.get() == nullptr)
        return false;

    geometry_shader = shared_ptr<GeometryShader>(RESOURCE->UseResource<GeometryShader>(gs_id));
    if (geometry_shader.get() == nullptr)
        return false;

    collision_mesh = shared_ptr<LightMesh>(RESOURCE->UseResource<LightMesh>(collision_ltmesh));
    if (collision_mesh.get() == nullptr)
        return false;

    // Create Collision
    for (auto& mesh : collision_mesh.get()->meshes)
    {
        if (mesh.mesh_name != "Level_BackGround")
        {
            UINT num_triangle = mesh.vertices.size() / 3;
            UINT index = 0;
            for (UINT t = 0; t < num_triangle; t++)
            {
                TriangleShape tri_plane = TriangleShape(
                    mesh.vertices[index + 0].p,
                    mesh.vertices[index + 1].p,
                    mesh.vertices[index + 2].p
                );

                tri_plane.index = t;

                level_triangles.push_back(tri_plane);
                index += 3;
            }
        }
    }

    return true;
}

void reality::LightMeshLevel::Update()
{
    if (DINPUT->GetKeyState(DIK_C) == KEY_PUSH)
        view_collision = !view_collision;

    DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
}

void reality::LightMeshLevel::Render()
{
    DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader.get()->InputLayout());
    DX11APP->GetDeviceContext()->VSSetShader(vertex_shader.get()->Get(), nullptr, 0);
    DX11APP->GetDeviceContext()->GSSetShader(geometry_shader.get()->GetDefaultGS(), nullptr, 0);

    for (auto& mesh : level_mesh.get()->meshes)
    {
        Material* material = RESOURCE->UseResource<Material>(mesh.mesh_name + ".mat");
        if (material)
            material->Set();

        UINT stride = sizeof(LightVertex);
        UINT offset = 0;

        DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
    }

    if (view_collision)
        RenderCollisionMesh();
}

void reality::LightMeshLevel::Destroy()
{
}

void reality::LightMeshLevel::RenderCollisionMesh()
{
    DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->Wireframe());

    for (auto& mesh : collision_mesh.get()->meshes)
    {
        Material* material = RESOURCE->UseResource<Material>(mesh.mesh_name + ".mat");
        if (material)
            material->Set();

        UINT stride = sizeof(LightVertex);
        UINT offset = 0;

        DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);        
    }

    DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->CullNone());
}

void reality::LightMeshLevel::ImportGuideLines(string mapdat_file, GuideLine::GuideType guide_type)
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

vector<GuideLine> reality::LightMeshLevel::GetGuideLines(GuideLine::GuideType guide_type)
{
    vector<GuideLine> guides;
    for (const auto& gd : guide_lines)
    {
        if (gd.guide_type_ == guide_type)
            guides.push_back(gd);
    }
    return guides;
}
