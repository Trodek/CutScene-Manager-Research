#include "j1CutSceneManager.h"
#include "j1Textures.h"
#include "Entity.h"
#include "j1Entity.h"
#include "j1Audio.h"

j1CutSceneManager::j1CutSceneManager()
{
	name = "cutscene";
}

j1CutSceneManager::~j1CutSceneManager()
{
}

bool j1CutSceneManager::Awake(pugi::xml_node &)
{
	return true;
}

bool j1CutSceneManager::Start()
{
	return true;
}

bool j1CutSceneManager::PreUpdate()
{
	return true;
}

bool j1CutSceneManager::Update(float dt)
{
	return true;
}

bool j1CutSceneManager::PostUpdate()
{
	return true;
}

bool j1CutSceneManager::CleanUp()
{
	return true;
}

int j1CutSceneManager::GetNextID() const
{
	return elements.size();
}

CutsceneElement::CutsceneElement(elements_groups group, const char * path, bool active) : group(group), path(path), active(active)
{
	id = App->cutscene->GetNextID();
}

CutsceneMap::CutsceneMap(elements_groups group, const char * path, bool active) : CutsceneElement(group, path, active)
{
}

CutsceneImage::CutsceneImage(elements_groups group, const char * path, bool active, iPoint pos, SDL_Rect rect) : CutsceneElement(group, path, active), pos(pos)
{
	texture = App->tex->LoadTexture(path);
	this->rect = rect;
}

SDL_Texture * CutsceneImage::GetTexture() const
{
	return texture;
}

SDL_Rect CutsceneImage::GetRect() const
{
	return rect;
}

iPoint CutsceneImage::GetPos() const
{
	return pos;
}

CutsceneEntity::CutsceneEntity(elements_groups group, const char * path, bool active, iPoint pos) : CutsceneElement(group, path, active)
{
	entity = App->entity->CreateEntity(pos, path);
	entity->active = active;
}

CutsceneEntity::~CutsceneEntity()
{
	App->entity->DeleteEntity(entity);
}

Entity * CutsceneEntity::GetEntity() const
{
	return entity;
}

CutsceneMusic::CutsceneMusic(elements_groups group, const char * path, bool active) : CutsceneElement(group, path, active)
{
	if (active)
	{
		App->audio->PlayMusic(path);
		playing = true;
	}
}

CutsceneMusic::~CutsceneMusic()
{
}

bool CutsceneMusic::IsPlaying() const
{
	return playing;
}

CutsceneSoundEffect::CutsceneSoundEffect(elements_groups group, const char * path, bool active, int loops) : CutsceneElement(group, path, active), loops(loops)
{
	id = App->audio->LoadFx(path);
}

CutsceneSoundEffect::~CutsceneSoundEffect()
{
}

CutsceneText::CutsceneText(elements_groups group, const char * path, bool active, int loops) : CutsceneElement(group, path, active)
{
}

CutsceneText::~CutsceneText()
{
}

void CutsceneText::SetText(const char * txt)
{

}
