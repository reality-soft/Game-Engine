#include "stdafx.h"
#include "DXStates.h"
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

bool reality::StaticMeshLevel::Create(string mesh_id, string vs_id)
{
    level_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>(mesh_id));
    if (level_mesh.get() == nullptr)
        return false;

    vertex_shader = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>(vs_id));
    if (vertex_shader.get() == nullptr)
        return false;

    // Create Collision
    
    for (const auto& mesh : level_mesh.get()->meshes)
    {
        if (mesh.mesh_name == "Collision")
            ConvertToTrianlgeShapes(level_triangles, mesh);  
    }

    clipping_rs = DXStates::rs_solid_cull_front();

    return true;
}

void reality::StaticMeshLevel::Update()
{
    DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
}

void reality::StaticMeshLevel::Render()
{
    DX11APP->GetDeviceContext()->OMSetBlendState(DX11APP->GetCommonStates()->Opaque(), 0, -1);
    DX11APP->GetDeviceContext()->RSSetState(clipping_rs);

    DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader.get()->InputLayout());
    DX11APP->GetDeviceContext()->VSSetShader(vertex_shader.get()->Get(), nullptr, 0);

    for (auto& mesh : level_mesh.get()->meshes)
    {
        if (mesh.mesh_name == "Collision")
        {
            continue;
        }

        Material* material = RESOURCE->UseResource<Material>(mesh.mesh_name + ".mat");
        if (material)
            material->Set();

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
        DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
    }

    DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->CullNone());
}

void reality::StaticMeshLevel::Destroy()
{
}

StaticMesh* reality::StaticMeshLevel::GetLevelMesh()
{
    return level_mesh.get();
}

VertexShader* reality::StaticMeshLevel::GetVertexShader()
{
    return vertex_shader.get();
}

void reality::StaticMeshLevel::RenderCollisionMesh()
{
    DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
    DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

    DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader.get()->InputLayout());
    DX11APP->GetDeviceContext()->VSSetShader(vertex_shader.get()->Get(), nullptr, 0);

    for (auto& mesh : level_mesh.get()->meshes)
    {
        if (mesh.mesh_name == "Collision")
        {
            Material* material = RESOURCE->UseResource<Material>(mesh.mesh_name + ".mat");
            if (material)
                material->Set();

            UINT stride = sizeof(Vertex);
            UINT offset = 0;

            DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
            DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
        }
    }
}