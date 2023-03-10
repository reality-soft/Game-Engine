#include "stdafx.h"
#include "LightMeshLevel.h"
#include "ResourceMgr.h"

using namespace reality;

reality::LightMeshLevel::LightMeshLevel()
{
}

reality::LightMeshLevel::~LightMeshLevel()
{
}

bool reality::LightMeshLevel::Create(string mesh_id, string vs_id, string gs_id)
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

    return true;
}

void reality::LightMeshLevel::Update()
{
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

}

void reality::LightMeshLevel::Destroy()
{
}
