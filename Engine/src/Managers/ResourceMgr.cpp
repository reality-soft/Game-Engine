#include "stdafx.h"
#include "ResourceMgr.h"
#include "FmodMgr.h"
#include "FbxMgr.h"
#include <io.h>

using namespace reality;

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
    LOCK_MUTEX_RESOURCE(directory_ + "/SKM/", &ResourceMgr::ImportSKM);
    LOCK_MUTEX_RESOURCE(directory_ + "/STM/", &ResourceMgr::ImportSTM);
    LOCK_MUTEX_RESOURCE(directory_ + "/ANIM/", &ResourceMgr::ImportANIM);
    LOCK_MUTEX_RESOURCE(directory_ + "/Shader/", &ResourceMgr::ImportShaders);
    LOCK_MUTEX_RESOURCE(directory_ + "/Sound/", &ResourceMgr::ImportSound);
    LOCK_MUTEX_RESOURCE(directory_ + "/Texture/", &ResourceMgr::ImportTexture);
    LOCK_MUTEX_RESOURCE(directory_ + "/Material/", &ResourceMgr::ImportMaterial);
    LOCK_MUTEX_RESOURCE(directory_ + "/Sprite/", &ResourceMgr::ImportSprite);
    LOCK_MUTEX_RESOURCE(directory_ + "/Effect/", &ResourceMgr::ImportEffect);
    LOCK_MUTEX_RESOURCE(directory_ + "/Light/", &ResourceMgr::ImportLight);

    //resource_mutex.lock();
    //LoadDir(directory_ + "/SKM/", &ResourceMgr::ImportSKM);
    //LoadDir(directory_ + "/STM/", &ResourceMgr::ImportSTM);
    //LoadDir(directory_ + "/ANIM/", &ResourceMgr::ImportANIM);
    ////LoadDir(directory_ + "/Shader/", &ResourceMgr::ImportShaders);
    //LoadDir(directory_ + "/Sound/", &ResourceMgr::ImportSound);
    //LoadDir(directory_ + "/Texture/", &ResourceMgr::ImportTexture);
    //LoadDir(directory_ + "/Material/", &ResourceMgr::ImportMaterial);
    //LoadDir(directory_ + "/Sprite/", &ResourceMgr::ImportSprite);
    //LoadDir(directory_ + "/Effect/", &ResourceMgr::ImportEffect);
    //LoadDir(directory_ + "/Light/", &ResourceMgr::ImportLight);
    //resource_mutex.unlock();
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

map<string, string> ResourceMgr::GetTotalResID()
{
    map<string, string> res_id_map;
    
    for (auto res : resdic_static_mesh)
    {
        res_id_map.insert(make_pair(res.first, "STM"));
    }
    for (auto res : resdic_skeletal_mesh)
    {
        res_id_map.insert(make_pair(res.first, "SKM"));
    }
    for (auto res : resdic_animation)
    {
        res_id_map.insert(make_pair(res.first, "ANM"));
    }
    for (auto res : resdic_vs)
    {
        res_id_map.insert(make_pair(res.first, "VS"));
    }
    for (auto res : resdic_ps)
    {
        res_id_map.insert(make_pair(res.first, "PS"));
    }
    for (auto res : resdic_texture)
    {
        res_id_map.insert(make_pair(res.first, "TEX"));
    }
    for (auto res : resdic_sound)
    {
        res_id_map.insert(make_pair(res.first, "SND"));
    }

    return res_id_map;
}

set<string> ResourceMgr::GetTotalTexID()
{
    set<string> tex_id_set;
    for (auto pair : resdic_texture)
    {
        tex_id_set.insert(pair.first);
    }
    return tex_id_set;
}

set<string> ResourceMgr::GetTotalVSID()
{
    set<string> vs_id_set;
    for (auto pair : resdic_vs)
    {
        vs_id_set.insert(pair.first);
    }
    return vs_id_set;
}

set<string> ResourceMgr::GetTotalPSID()
{
    set<string> ps_id_set;
    for (auto pair : resdic_ps)
    {
        ps_id_set.insert(pair.first);
    }
    return ps_id_set;
}

set<string> ResourceMgr::GetTotalGSID()
{
    set<string> gs_id_set;
    for (auto pair : resdic_gs)
    {
        gs_id_set.insert(pair.first);
    }
    return gs_id_set;
}

set<string> ResourceMgr::GetTotalSKMID()
{
    set<string> skm;
    for (auto pair : resdic_skeletal_mesh)
    {
        skm.insert(pair.first);
    }
    return skm;
}

set<string> ResourceMgr::GetTotalSTMID()
{
    set<string> stm;
    for (auto pair : resdic_static_mesh)
    {
        stm.insert(pair.first);
    }
    return stm;
}

set<string> ResourceMgr::GetTotalANIMID()
{
    set<string> anim;
    for (auto pair : resdic_animation)
    {
        anim.insert(pair.first);
    }
    return anim;
}

set<string> ResourceMgr::GetTotalSpriteID()
{
    set<string> sprite;
    for (auto pair : resdic_sprite)
    {
        sprite.insert(pair.first);
    }
    return sprite;
}

set<string> ResourceMgr::GetTotalEffectID()
{
    set<string> effect;
    for (auto pair : resdic_effect)
    {
        effect.insert(pair.first);
    }
    return effect;
}

set<string> ResourceMgr::GetTotalLightID()
{
    set<string> light;
    for (auto pair : resdic_light)
    {
        light.insert(pair.first);
    }
    return light;
}

set<string> ResourceMgr::GetTotalMATID()
{
    set<string> mat;
    for (auto pair : resdic_material)
    {
        mat.insert(pair.first);
    }
    return mat;
}

void reality::ResourceMgr::PushStaticMesh(string id, const StaticMesh& static_mesh)
{
    resdic_static_mesh.insert({ id, static_mesh });
}

void ResourceMgr::PushSkeletalMesh(string id, const SkeletalMesh& skeletal_mesh)
{
    resdic_skeletal_mesh.insert({ id, skeletal_mesh });
}

void ResourceMgr::PushAnimation(const map<string, OutAnimData>& animation)
{
    resdic_animation.insert(animation.begin(), animation.end());
}

bool ResourceMgr::ImportShaders(string filename)
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

    if (filename.find("CS") != string::npos)
    {
        ComputeShader new_cs;
        if (!new_cs.LoadCompiled(to_mw(filename)))
            return false;

        auto strs = split(filename, '/');
        string id = strs[strs.size() - 1];

        resdic_cs.insert(make_pair(id, new_cs));
        return true;
    }

    return true;
}

bool ResourceMgr::ImportSound(string filename)
{
    FMOD::Sound* newSound;

    FMOD_RESULT hr = FMOD_MGR->fmod_system()->createSound(filename.c_str(), FMOD_3D, nullptr, &newSound);
    if (hr != FMOD_OK)
    {
        return false;
    }

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_sound.insert(make_pair(id, newSound));
    return true;
}

bool ResourceMgr::ImportSKM(string filename)
{
    SkeletalMesh skeletal_mesh = FBX->LoadSkeletalMesh(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_skeletal_mesh.insert(make_pair(id, skeletal_mesh));

    return false;
}

bool ResourceMgr::ImportSTM(string filename)
{
    StaticMesh static_mesh = FBX->LoadStaticMesh(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_static_mesh.insert(make_pair(id, static_mesh));

    return true;
}

bool ResourceMgr::ImportANIM(string filename)
{
    OutAnimData animation = FBX->LoadAnimation(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_animation.insert(make_pair(id, animation));

    return true;
}

bool reality::ResourceMgr::ImportLight(string filename)
{
    DATA->LoadSheetFile(filename);

    auto strs1 = split(filename, '/');
    auto name = strs1[max((int)strs1.size() - 1, 0)];
    auto strs2 = split(name, '.');
    name = strs2[0];

    auto sheet = DATA->LoadSheet(name);

    if (name == "PointLight")
    {
        for (auto& pair : sheet->resdic_item)
        {
            auto& item = pair.second;

            PointLight new_pointlight;

            // Light Color
            auto strs = split(item->GetValue("light_color"), ' ');
            if(strs.size() == 4)
                new_pointlight.light_color = {stof(strs[0]), stof(strs[1]), stof(strs[2]), stof(strs[3]) };

            // Range
            new_pointlight.range = stof(item->GetValue("range"));
            
            // Attenuation
            strs = split(item->GetValue("attenuation"), ' ');
            if (strs.size() == 3)
                new_pointlight.attenuation = { stof(strs[0]), stof(strs[1]), stof(strs[2]) };

            // Specular
            new_pointlight.specular = stof(item->GetValue("specular"));

            resdic_light.insert({ pair.first, make_shared<PointLight>(new_pointlight) });
        }
    }
    else if (name == "SpotLight")
    {
        for (auto& pair : sheet->resdic_item)
        {
            auto& item = pair.second;

            SpotLight new_spotlight;

            // Light Color
            auto strs = split(item->GetValue("light_color"), ' ');
            if (strs.size() == 4)
                new_spotlight.light_color = { stof(strs[0]), stof(strs[1]), stof(strs[2]), stof(strs[3]) };

            // Range
            new_spotlight.range = stof(item->GetValue("range"));

            // Attenuation
            strs = split(item->GetValue("attenuation"), ' ');
            if (strs.size() == 3)
                new_spotlight.attenuation = { stof(strs[0]), stof(strs[1]), stof(strs[2]) };

            // Specular
            new_spotlight.specular = stof(item->GetValue("specular"));

            // Spot
            new_spotlight.spot = stof(item->GetValue("spot"));

            resdic_light.insert({ pair.first, make_shared<SpotLight>(new_spotlight) });
        }
    }
    else
        return false;
    
    return true;
}

bool ResourceMgr::ImportTexture(string filename)
{
    Texture new_tex;
    bool success_load = false;

    success_load = new_tex.LoadTextureWIC(to_mw(filename));

    if (!success_load)
        success_load = new_tex.LoadTextureDDS(to_mw(filename));

    if (!success_load)
        return false;

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    resdic_texture.insert(make_pair(id, new_tex));

    return false;
}

bool ResourceMgr::ImportSprite(string filename)
{
    DATA->LoadSheetFile(filename);

    auto strs1 = split(filename, '/');
    auto name = strs1[max((int)strs1.size() - 1, 0)];
    auto strs2 = split(name, '.');
    name = strs2[0];

    auto sheet = DATA->LoadSheet(name);

    if (!sheet)
        return false;

    auto item = sheet->LoadItem(name);
    if (item == NULL)
        return false;

    string str_type = item->GetValue("type");

    if (str_type == "")
        return false;

    E_EffectType type = (E_EffectType)stoi(str_type);

    switch (type)
    {
    case UV_SPRITE:
    {
        UVSprite uv_sprite;
        uv_sprite.tex_id = item->GetValue("tex_id");

        auto uvListItem = sheet->LoadItem("uvList");
        // 리스트에서 가장 높은 프레임의 값을 가져온다.
        int max = 0;
        for (int i = 1; true; i++)
        {
            if (uvListItem->values[to_string(i)] == "")
            {
                max = i - 1;
                break;
            }

        }
        // 가장 낮은 프레임부터 가장 높은 프레임까지 파싱해서 uv값을 넣어준다.
        uv_sprite.uv_list.clear();
        for (int i = 0; i < max; i++)
        {
            // 0 0 25 25 형식
            auto splited_str = split(uvListItem->values[to_string(i + 1)], ' ');
            uv_sprite.uv_list.push_back({ { stol(splited_str[0]), stol(splited_str[1]) }, { stol(splited_str[2]), stol(splited_str[3]) } });
        }

        // 로딩한 스프라이트를 리스트에 넣는다.
        resdic_sprite.insert({ name, make_shared<UVSprite>(uv_sprite) });
    } break;
    case TEX_SPRITE:
    {
        TextureSprite tex_sprite;

        // TODO : 데이터 형태 수정해야할듯
        auto texListItem = sheet->LoadItem("texList");
        // 리스트에서 가장 높은 프레임의 값을 가져온다.
        int max = 0;
        for (int i = 1; true; i++)
        {
            if (texListItem->values[to_string(i)] == "")
            {
                max = i - 1;
                break;
            }
        }
        // 가장 낮은 프레임부터 가장 높은 프레임까지 파싱해서 tex_id값을 넣어준다.
        tex_sprite.tex_id_list.clear();
        for (int i = 0; i < max; i++)
        {
            tex_sprite.tex_id_list.push_back(texListItem->values[to_string(i + 1)]);
        }
        // 로딩한 스프라이트를 리스트에 넣는다.
        resdic_sprite.insert({ name,  make_shared<TextureSprite>(tex_sprite) });
    } break;
    }

    return true;
}

bool ResourceMgr::SaveSprite(string name, shared_ptr<Sprite> new_sprite)
{
    if (resdic_sprite.find(name) != resdic_sprite.end())
        return false;
    else
    {
        resdic_sprite.insert({ name, new_sprite });
        return true;
    }
    
}

bool ResourceMgr::ImportEffect(string filename)
{
    DATA->LoadSheetFile(filename);

    auto strs1 = split(filename, '/');
    auto name = strs1[max((int)strs1.size() - 1, 0)];
    auto strs2 = split(name, '.');
    name = strs2[0];

    auto sheet = DATA->LoadSheet(name);

    if (sheet == NULL)
        return false;

    Effect effect_data;

    for (auto pair : sheet->resdic_item)
    {
        auto item = pair.second;

        if (stoi(item->GetValue("type")) != EMITTER)
            return false;

        Emitter emitter;
        ParseEmitter(item.get(), emitter);

        effect_data.emitters.insert({ item->GetValue("Name"), emitter});
    }

    // resdic_effect 추가
    resdic_effect.insert({ name, effect_data });

    return true;
}

bool ResourceMgr::SaveEffect(string name, Effect new_effect)
{
    if (resdic_sprite.find(name) != resdic_sprite.end())
        return false;
    else
    {
        resdic_effect.insert({ name, new_effect });
        return true;
    }
}

bool ResourceMgr::ImportMaterial(string filename)
{
    Material material;
    material.LoadAndCreate(filename);

    auto strs = split(filename, '/');
    string id = strs[strs.size() - 1];

    RESOURCE->PushResource<Material>(id, material);

    return true;
}

void ResourceMgr::ParseEmitter(DataItem* emitter_data, Emitter& emitter)
{
    // type
    emitter.type = (E_EffectType)stoi(emitter_data->GetValue("type"));

    // sprite_id
    emitter.sprite_id = emitter_data->GetValue("sprite_id");

    // emit_type
    emitter.emit_type = (E_EmitType)stoi(emitter_data->GetValue("emit_type"));
    // emit_once
    emitter.emit_once = stoi(emitter_data->GetValue("emit_once"));
    // emit_per_second
    emitter.emit_per_second = stoi(emitter_data->GetValue("emit_per_second"));
    // emit_time
    emitter.emit_time = stof(emitter_data->GetValue("emit_time"));



    vector<string> splited_str;
    vector<string> splited_str2;

    // life_time
    {
        splited_str = split(emitter_data->GetValue("life_time"), ' ');
        if (splited_str.size() < 2)
            return;
        emitter.life_time[0] = stof(splited_str[0]);
        emitter.life_time[1] = stof(splited_str[1]);
    }

    // SettingType
    {
        emitter.color_setting_type = (E_EmitterAttributeType)stoi(emitter_data->GetValue("color_setting_type"));
        emitter.size_setting_type = (E_EmitterAttributeType)stoi(emitter_data->GetValue("size_setting_type"));
        emitter.rotation_setting_type = (E_EmitterAttributeType)stoi(emitter_data->GetValue("rotation_setting_type"));
        emitter.position_setting_type = (E_EmitterAttributeType)stoi(emitter_data->GetValue("position_setting_type"));
    }

    // COLOR
    {
        // initial_color
        splited_str = split(emitter_data->GetValue("initial_color"), ' ');
        if (splited_str.size() < 4)
            return;
        emitter.initial_color.x = stof(splited_str[0]);
        emitter.initial_color.y = stof(splited_str[1]);
        emitter.initial_color.z = stof(splited_str[2]);
        emitter.initial_color.w = stof(splited_str[3]);

        // color_timeline_map
        {
            string str_color_map = emitter_data->GetValue("color_timeline_map");
            if (str_color_map.size())
            {
                splited_str = split(str_color_map, '~');
                for (auto value : splited_str)
                {
                    auto splited_map_value = split(value, '-');

                    if (splited_map_value.size() == 0)
                        return;
                    int time = stoi(splited_map_value[0]);

                    auto splited_map_xyzw = split(splited_map_value[1], ' ');
                    XMFLOAT4 color = { stof(splited_map_xyzw[0]), stof(splited_map_xyzw[1]), stof(splited_map_xyzw[2]), stof(splited_map_xyzw[3]) };

                    emitter.color_timeline_map.insert({ time, color });
                }
                ComputeColorTimeline(emitter.color_timeline_map, emitter.color_timeline);
            }
        }

    }

    // SIZE
    {
        // initial_size
        {
            splited_str = split(emitter_data->GetValue("initial_size"), '~');
            if (splited_str.size() < 2)
                return;
            // min
            splited_str2 = split(splited_str[0], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_size[0].x = stof(splited_str2[0]);
            emitter.initial_size[0].y = stof(splited_str2[1]);
            emitter.initial_size[0].z = stof(splited_str2[2]);
            // max
            splited_str2 = split(splited_str[1], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_size[1].x = stof(splited_str2[0]);
            emitter.initial_size[1].y = stof(splited_str2[1]);
            emitter.initial_size[1].z = stof(splited_str2[2]);
        }
        // add_size_per_lifetime
        {
            splited_str = split(emitter_data->GetValue("add_size_per_lifetime"), '~');
            if (splited_str.size() < 2)
                return;
            // min
            splited_str2 = split(splited_str[0], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.add_size_per_lifetime[0].x = stof(splited_str2[0]);
            emitter.add_size_per_lifetime[0].y = stof(splited_str2[1]);
            emitter.add_size_per_lifetime[0].z = stof(splited_str2[2]);
            // max
            splited_str2 = split(splited_str[1], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.add_size_per_lifetime[1].x = stof(splited_str2[0]);
            emitter.add_size_per_lifetime[1].y = stof(splited_str2[1]);
            emitter.add_size_per_lifetime[1].z = stof(splited_str2[2]);
        }
        // size_timeline_map
        {
            string str_size_map = emitter_data->GetValue("size_timeline_map");
            if (str_size_map.size())
            {
                splited_str = split(str_size_map, '~');
                for (auto value : splited_str)
                {
                    auto splited_map_value = split(value, '-');

                    if (splited_map_value.size() == 0)
                        return;
                    int time = stoi(splited_map_value[0]);

                    auto splited_map_xyz = split(splited_map_value[1], ' ');
                    XMFLOAT3 size = { stof(splited_map_xyz[0]), stof(splited_map_xyz[1]), stof(splited_map_xyz[2]) };

                    emitter.size_timeline_map.insert({ time, size });
                }
                ComputeSizeTimeline(emitter.size_timeline_map, emitter.size_timeline);
            }
        }


    }

    // ROTATION
    {
        // initial_rotation
        {
            splited_str = split(emitter_data->GetValue("initial_rotation"), '~');
            if (splited_str.size() < 2)
                return;
            // min
            splited_str2 = split(splited_str[0], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_rotation[0].x = stof(splited_str2[0]);
            emitter.initial_rotation[0].y = stof(splited_str2[1]);
            emitter.initial_rotation[0].z = stof(splited_str2[2]);
            // max
            splited_str2 = split(splited_str[1], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_rotation[1].x = stof(splited_str2[0]);
            emitter.initial_rotation[1].y = stof(splited_str2[1]);
            emitter.initial_rotation[1].z = stof(splited_str2[2]);
        }
        // add_rotation_per_lifetime
        {
            splited_str = split(emitter_data->GetValue("add_rotation_per_lifetime"), '~');
            if (splited_str.size() < 2)
                return;
            // min
            splited_str2 = split(splited_str[0], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.add_rotation_per_lifetime[0].x = stof(splited_str2[0]);
            emitter.add_rotation_per_lifetime[0].y = stof(splited_str2[1]);
            emitter.add_rotation_per_lifetime[0].z = stof(splited_str2[2]);
            // max
            splited_str2 = split(splited_str[1], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.add_rotation_per_lifetime[1].x = stof(splited_str2[0]);
            emitter.add_rotation_per_lifetime[1].y = stof(splited_str2[1]);
            emitter.add_rotation_per_lifetime[1].z = stof(splited_str2[2]);
        }
        // rotation_timeline_map
        {
            string str_rot_map = emitter_data->GetValue("rotation_timeline_map");
            if (str_rot_map.size())
            {
                splited_str = split(str_rot_map, '~');
                for (auto value : splited_str)
                {
                    auto splited_map_value = split(value, '-');

                    if (splited_map_value.size() == 0)
                        return;
                    int time = stoi(splited_map_value[0]);

                    auto splited_map_xyz = split(splited_map_value[1], ' ');
                    XMFLOAT3 rotation = { stof(splited_map_xyz[0]), stof(splited_map_xyz[1]), stof(splited_map_xyz[2]) };

                    emitter.rotation_timeline_map.insert({ time, rotation });
                }
                ComputeRotationTimeline(
                    emitter.rotation_timeline_map, emitter.rotation_timeline);
            }
        }
    }

    // VELOCITY
    {
        // initial_position
        {
            splited_str = split(emitter_data->GetValue("initial_position"), '~');
            if (splited_str.size() < 2)
                return;
            // min
            splited_str2 = split(splited_str[0], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_position[0].x = stof(splited_str2[0]);
            emitter.initial_position[0].y = stof(splited_str2[1]);
            emitter.initial_position[0].z = stof(splited_str2[2]);
            // max
            splited_str2 = split(splited_str[1], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_position[1].x = stof(splited_str2[0]);
            emitter.initial_position[1].y = stof(splited_str2[1]);
            emitter.initial_position[1].z = stof(splited_str2[2]);
        }
        // initial_velocity
        {
            splited_str = split(emitter_data->GetValue("initial_velocity"), '~');
            if (splited_str.size() < 2)
                return;
            // min
            splited_str2 = split(splited_str[0], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_velocity[0].x = stof(splited_str2[0]);
            emitter.initial_velocity[0].y = stof(splited_str2[1]);
            emitter.initial_velocity[0].z = stof(splited_str2[2]);
            // max
            splited_str2 = split(splited_str[1], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.initial_velocity[1].x = stof(splited_str2[0]);
            emitter.initial_velocity[1].y = stof(splited_str2[1]);
            emitter.initial_velocity[1].z = stof(splited_str2[2]);
        }
        // accelation_per_lifetime
        {
            splited_str = split(emitter_data->GetValue("accelation_per_lifetime"), '~');
            if (splited_str.size() < 2)
                return;
            // min
            splited_str2 = split(splited_str[0], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.accelation_per_lifetime[0].x = stof(splited_str2[0]);
            emitter.accelation_per_lifetime[0].y = stof(splited_str2[1]);
            emitter.accelation_per_lifetime[0].z = stof(splited_str2[2]);
            // max
            splited_str2 = split(splited_str[1], ' ');
            if (splited_str2.size() < 3)
                return;
            emitter.accelation_per_lifetime[1].x = stof(splited_str2[0]);
            emitter.accelation_per_lifetime[1].y = stof(splited_str2[1]);
            emitter.accelation_per_lifetime[1].z = stof(splited_str2[2]);
        }
        // velocity_timeline_map
        {
            string str_vel_map = emitter_data->GetValue("velocity_timeline_map");
            if (str_vel_map.size())
            {
                splited_str = split(str_vel_map, '~');
                for (auto value : splited_str)
                {
                    auto splited_map_value = split(value, '-');

                    if (splited_map_value.size() == 0)
                        return;
                    int time = stoi(splited_map_value[0]);

                    auto splited_map_xyz = split(splited_map_value[1], ' ');
                    XMFLOAT3 vel = { stof(splited_map_xyz[0]), stof(splited_map_xyz[1]), stof(splited_map_xyz[2]) };

                    emitter.velocity_timeline_map.insert({ time, vel });
                }
                ComputeVelocityTimeline(emitter.velocity_timeline_map, emitter.velocity_timeline);
            }
        }
    }

    // GRAVITY
    emitter.gravity_on_off = stoi(emitter_data->GetValue("gravity_on_off"));

    // BS
    emitter.bs_state = (E_EffectBS)stoi(emitter_data->GetValue("BS"));

    // DS
    emitter.ds_state = (E_EffectDS)stoi(emitter_data->GetValue("DS"));

    emitter.vs_id = emitter_data->GetValue("vs_id");
    emitter.geo_id = emitter_data->GetValue("geo_id");
    emitter.mat_id = emitter_data->GetValue("mat_id");
}

// 타임라인 -> 배열 계산 메소드들
void ResourceMgr::ComputeColorTimeline(map<int, XMFLOAT4>& timeline, XMFLOAT4* arr)
{

    for (int i = 0; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline.begin()->second;
    }

    // map의 개수가 1개라면 메소드 종료
    if (timeline.size() == 1)
        return;

    // 2. 이후의 값들을 보정해서 계산
    int last_time = -1;
    for (const auto& pair : timeline)
    {
        // 첫 입력이면 last_time에만 저장하고 다음 배열로
        if (last_time == -1)
        {
            last_time = pair.first;
            continue;
        }

        // 아니라면 앞의 값과 보정
        int cur_time = pair.first;
        int time_dif = cur_time - last_time + 1;

        XMFLOAT4 value_dif_step = {
            (timeline[cur_time].x - timeline[last_time].x) / time_dif,
            (timeline[cur_time].y - timeline[last_time].y) / time_dif,
            (timeline[cur_time].z - timeline[last_time].z) / time_dif,
            (timeline[cur_time].w - timeline[last_time].w) / time_dif
        };

        for (int i = 0; i < time_dif; i++)
        {
            XMFLOAT4 add_value = { value_dif_step.x * i, value_dif_step.y * i, value_dif_step.z * i, value_dif_step.w * i };
            arr[last_time + i] = {
                timeline[last_time].x + add_value.x,
                timeline[last_time].y + add_value.y,
                timeline[last_time].z + add_value.z,
                timeline[last_time].w + add_value.w };
        }

        last_time = pair.first;
    }

    int timeline_last_time = last_time;
    for (int i = timeline_last_time; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline[last_time];
    }
}

void ResourceMgr::ComputeSizeTimeline(map<int, XMFLOAT3>& timeline, XMFLOAT3* arr)
{

    for (int i = 0; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline.begin()->second;
    }

    if (timeline.size() == 1)
        return;

    int last_time = -1;
    for (const auto& pair : timeline)
    {

        if (last_time == -1)
        {
            last_time = pair.first;
            continue;
        }

        int cur_time = pair.first;
        int time_dif = cur_time - last_time + 1;

        XMFLOAT3 value_dif_step = {
            (timeline[cur_time].x - timeline[last_time].x) / time_dif,
            (timeline[cur_time].y - timeline[last_time].y) / time_dif,
            (timeline[cur_time].z - timeline[last_time].z) / time_dif
        };

        for (int i = 0; i < time_dif; i++)
        {
            XMFLOAT3 add_value = { value_dif_step.x * i, value_dif_step.y * i, value_dif_step.z * i };
            arr[last_time + i] = {
                timeline[last_time].x + add_value.x,
                timeline[last_time].y + add_value.y,
                timeline[last_time].z + add_value.z
            };
        }

        last_time = pair.first;
    }

    int timeline_last_time = last_time;
    for (int i = timeline_last_time; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline[last_time];
    }
}

void ResourceMgr::ComputeRotationTimeline(map<int, XMFLOAT3>& timeline, XMFLOAT3* arr)
{

    for (int i = 0; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline.begin()->second;
    }

    if (timeline.size() == 1)
        return;

    int last_time = -1;
    for (const auto& pair : timeline)
    {

        if (last_time == -1)
        {
            last_time = pair.first;
            continue;
        }

        
        int cur_time = pair.first;
        int time_dif = cur_time - last_time + 1;

        XMFLOAT3 value_dif_step = {
            (timeline[cur_time].x - timeline[last_time].x) / time_dif,
            (timeline[cur_time].y - timeline[last_time].y) / time_dif,
            (timeline[cur_time].z - timeline[last_time].z) / time_dif
        };

        for (int i = 0; i < time_dif; i++)
        {
            XMFLOAT3 add_value = { value_dif_step.x * i, value_dif_step.y * i, value_dif_step.z * i };
            arr[last_time + i] = {
                timeline[last_time].x + add_value.x,
                timeline[last_time].y + add_value.y,
                timeline[last_time].z + add_value.z
            };
        }

        last_time = pair.first;
    }

    int timeline_last_time = last_time;
    for (int i = timeline_last_time; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline[last_time];
    }
}

void ResourceMgr::ComputeVelocityTimeline(map<int, XMFLOAT3>& timeline, XMFLOAT3* arr)
{

    for (int i = 0; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline.begin()->second;
    }

    if (timeline.size() == 1)
        return;

    int last_time = -1;
    for (const auto& pair : timeline)
    {

        if (last_time == -1)
        {
            last_time = pair.first;
            continue;
        }

        int cur_time = pair.first;
        int time_dif = cur_time - last_time + 1;

        XMFLOAT3 value_dif_step = {
            (timeline[cur_time].x - timeline[last_time].x) / time_dif,
            (timeline[cur_time].y - timeline[last_time].y) / time_dif,
            (timeline[cur_time].z - timeline[last_time].z) / time_dif
        };

        for (int i = 0; i < time_dif; i++)
        {
            XMFLOAT3 add_value = { value_dif_step.x * i, value_dif_step.y * i, value_dif_step.z * i };
            arr[last_time + i] = {
                timeline[last_time].x + add_value.x,
                timeline[last_time].y + add_value.y,
                timeline[last_time].z + add_value.z
            };
        }

        last_time = pair.first;
    }

    int timeline_last_time = last_time;
    for (int i = timeline_last_time; i < EFFECT_TIMELINE_SIZE; i++)
    {
        arr[i] = timeline[last_time];
    }
}