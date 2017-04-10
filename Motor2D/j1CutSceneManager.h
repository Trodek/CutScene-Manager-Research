#ifndef _CUTSCENEMANAGER_H_
#define _CUTSCENEMANAGER_H_

#include "j1Module.h"
#include <string>
#include "j1Render.h"

class Entity;
class UI_Text;

enum elements_groups
{
	e_g_map,
	e_g_image,
	e_g_entity,
	e_g_music,
	e_g_sound_effect,
	e_g_text,
	e_g_null
};

class CutsceneElement
{
public:

	CutsceneElement(elements_groups group, const char* path, bool active = true);
	virtual ~CutsceneElement() {};

public:
	string			path;
	bool			active = false;
	elements_groups group = e_g_null;
	int				id = -1;
};

class CutsceneMap : public CutsceneElement
{
public:
	CutsceneMap(elements_groups group, const char* path, bool active = true);
};

class CutsceneImage : public CutsceneElement
{
public:
	CutsceneImage(elements_groups group, const char* path, bool active = true, iPoint pos = { 0,0 }, SDL_Rect rect = { 0,0,0,0 });

	SDL_Texture* GetTexture()const;
	SDL_Rect GetRect()const;
	iPoint GetPos()const;

private:
	SDL_Texture* texture = nullptr;
	SDL_Rect	 rect = { 0,0,0,0 };
	iPoint		 pos = NULLPOINT;

};

class CutsceneEntity : public CutsceneElement
{
public:
	CutsceneEntity(elements_groups group, const char* path, bool active = true, iPoint pos = {0,0});
	~CutsceneEntity();

	Entity* GetEntity()const;

private:
	Entity*	entity;
};

class CutsceneMusic : public CutsceneElement
{
public:
	CutsceneMusic(elements_groups group, const char* path, bool active = true);
	~CutsceneMusic();

	bool IsPlaying()const;

private:
	bool playing = false;
};

class CutsceneSoundEffect : public CutsceneElement
{
public:
	CutsceneSoundEffect(elements_groups group, const char* path, bool active = true, int loops = 0);
	~CutsceneSoundEffect();

private:
	int loops = 0;
	int id = -1;
};

class CutsceneText : public CutsceneElement
{
public:
	CutsceneText(elements_groups group, const char* path, bool active = true, int loops = 0);
	~CutsceneText();

	void SetText(const char* txt);

private:
	UI_Text* text;
};

class j1CutSceneManager : public j1Module
{
public:
	j1CutSceneManager();
	~j1CutSceneManager();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	int GetNextID() const;
private:

public:

private:

	std::list<CutsceneElement*>	elements;
};


#endif
