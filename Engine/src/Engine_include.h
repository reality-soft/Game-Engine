#pragma once

#include "Engine/SingletonClass/Engine.h"
#include "Engine/SingletonClass/TimeMgr.h"
#include "Engine/SingletonClass/ResourceMgr.h"
#include "Engine/SingletonClass/FbxMgr.h"
#include "Engine/SingletonClass/FmodMgr.h"
#include "Engine/SingletonClass/GUIMgr.h"
#include "Engine/SingletonClass/SceneMgr.h"
#include "Engine/SingletonClass/PhysicsMgr.h"
#include "Engine/SingletonClass/EventMgr.h"
#include "Engine/SingletonClass/InputEventMgr.h"
#include "Engine/SingletonClass/DataMgr.h"
#include "Engine/SingletonClass/RenderTargetMgr.h"
#include "Engine/SingletonClass/QuadTreeMgr.h"
#include "Engine/SingletonClass/WriteMgr.h"

#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/SoundSystem.h"
#include "ECS/Systems/LightingSystem.h"
#include "ECS/Systems/EffectSystem.h"
#include "ECS/Systems/MovementSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Actors/Actor.h"
#include "ECS/Actors/Character.h"
#include "ECS/Actors/FX_BaseEffectActor.h"
#include "ECS/Actors/FX_Effect.h"
#include "ECS/Actors/UIActor.h"
#include "ECS/Actors/UI_Ingame_Actor.h"

#include "Engine/Scene.h"
#include "Engine/FbxLoader.h"
#include "Engine/StaticObject.h"
#include "Engine/Level.h"
#include "Engine/SkySphere.h"
#include "Engine/LightMeshLevel.h"
#include "Engine/Log.h"
#include "Engine/UIBase.h"
#include "Engine/UI_Image.h"
#include "Engine/UI_Button.h"
#include "Engine/UI_Text.h"
