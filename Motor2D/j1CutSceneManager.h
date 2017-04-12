#ifndef _CUTSCENEMANAGER_H_
#define _CUTSCENEMANAGER_H_

#include "j1Module.h"
#include <string>
#include "j1Render.h"
#include "BezierEasing.h"
#include "j1Timer.h"

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

//---------------------
// Elements
//---------------------

class CutsceneElement
{
public:

	CutsceneElement(elements_groups group, const char* path, const char* name, bool active = true);
	virtual ~CutsceneElement() {};

public:
	string			path;
	bool			active = false;
	elements_groups group = e_g_null;
	int				id = -1;
	string			name;
};

class CutsceneMap : public CutsceneElement
{
public:
	CutsceneMap(elements_groups group, const char* path, const char* name, bool active = true);
};

class CutsceneImage : public CutsceneElement
{
public:
	CutsceneImage(elements_groups group, const char* path, const char* name, bool active = true, iPoint pos = { 0,0 }, SDL_Rect rect = { 0,0,0,0 }, int layer = 0);

	SDL_Texture* GetTexture()const;
	SDL_Rect GetRect()const;
	iPoint GetPos()const;
	int	GetLayer()const;

private:
	SDL_Texture* texture = nullptr;
	SDL_Rect	 rect = { 0,0,0,0 };
	iPoint		 pos = NULLPOINT;
	int			 layer = 0;

};

class CutsceneEntity : public CutsceneElement
{
public:
	CutsceneEntity(elements_groups group, const char* path, const char* name, bool active = true, iPoint pos = {0,0});
	~CutsceneEntity();

	Entity* GetEntity()const;

private:
	Entity*	entity;
};

class CutsceneMusic : public CutsceneElement
{
public:
	CutsceneMusic(elements_groups group, const char* path, const char* name, bool active = true);
	~CutsceneMusic();

	bool IsPlaying()const;

private:
	bool playing = false;
};

class CutsceneSoundEffect : public CutsceneElement
{
public:
	CutsceneSoundEffect(elements_groups group, const char* path, const char* name, bool active = true, int loops = 0);
	~CutsceneSoundEffect();

	int GetID()const;
	int GetLoops()const;

private:
	int loops = 0;
	int id = 0;
};

class CutsceneText : public CutsceneElement
{
public:
	CutsceneText(elements_groups group, const char* path, const char* name, bool active = true, iPoint pos = { 0,0 });
	~CutsceneText();

	void SetText(const char* txt);

private:
	UI_Text* text;
};

//---------------------
// Actions
//---------------------

enum actions
{
	a_move,
	a_action,
	a_play,
	a_stop,
	a_modify,
	a_enable,
	a_disable,
	a_change_scene,
	a_null,
};

class CutsceneAction
{
public:
	CutsceneAction(actions action, const char* name, int start_time, int duration = 0);

	bool operator()(const CutsceneAction* a1, const CutsceneAction* a2)
	{
		return a1->start > a2->start;
	}

public:
	string	element_name;
	actions action = a_null;
	int		start = -1;
	int		duration = 0;
};

enum reference_type
{
	r_t_local,
	r_t_global,
	r_t_map,
	r_t_null,
};

class CutSceneAction_Cmp
{
public:
	CutSceneAction_Cmp();

	bool operator()(CutsceneAction*& a1, CutsceneAction*& a2)
	{
		return a1->start > a2->start;
	}
};

class CutsceneMove : public CutsceneAction
{
public:
	CutsceneMove(actions action, const char* name, int start_time, int duration = 0);
	~CutsceneMove();

public:
	BezierEasing*	bezier = nullptr;
	iPoint			dest = NULLPOINT;
	reference_type	reference = r_t_null;

};

enum entity_actions
{
	e_a_kill,
	e_a_spawn,
	e_a_change_pos,
	e_a_null
};

class CutsceneModifyEntity : public CutsceneAction
{
public:
	CutsceneModifyEntity(actions action, const char* name, int start_time, int duration = 0);

public:
	entity_actions entity_action = e_a_null;
	iPoint pos = NULLPOINT;

};

class CutsceneModifyImage : public CutsceneAction
{
public:
	CutsceneModifyImage(actions action, const char* name, int start_time, int duration = 0);

public:
	string	 var;
	string	 path;
	SDL_Rect rect = NULLRECT;

};

class CutsceneModifyText : public CutsceneAction
{
public:
	CutsceneModifyText(actions action, const char* name, int start_time, int duration = 0);

public:
	string txt;

};

enum change_scene_effects
{
	c_s_e_fade,
	c_s_e_null,
};

class CutsceneChangeScene : public CutsceneAction
{
public:
	CutsceneChangeScene(actions action, const char* name, int start_time, int duration = 0);

public:
	string					path;
	change_scene_effects	effect = c_s_e_null;

};

//---------------------
// Manager
//---------------------

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

	void Play(const char* path);

private:

	void Load(const char* path);

	elements_groups GetElementGroup(const char* ele)const;

	void PerformActions(float dt);

	// Load elements
	void LoadMap(pugi::xml_node& node);
	void LoadImage(pugi::xml_node& node);
	void LoadEntity(pugi::xml_node& node);
	void LoadMusic(pugi::xml_node& node);
	void LoadSoundEffect(pugi::xml_node& node);
	void LoadText(pugi::xml_node& node);

	// Update Elements
	void UpdateElements(float dt);

	//Load Actions
	void LoadAction(pugi::xml_node& node);
	void LoadMove(pugi::xml_node& node);
	void LoadModify(pugi::xml_node& node);
	void LoadChangeScene(pugi::xml_node& node);

public:
	UI_Window*					gui_win = nullptr;

private:

	std::list<CutsceneElement*>																elements;

	std::priority_queue<CutsceneAction*, std::vector<CutsceneAction*>, CutSceneAction_Cmp>	remaining_actions;
	std::list<CutsceneAction*>																active_actions;

	j1Timer																					scene_timer;
};


#endif
