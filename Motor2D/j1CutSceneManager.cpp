#include "j1CutSceneManager.h"
#include "j1Textures.h"
#include "Entity.h"
#include "j1Entity.h"
#include "j1Audio.h"
#include "j1XMLLoader.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1Window.h"
#include "j1Viewports.h"

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
	uint w, h;
	App->win->GetWindowSize(w, h);

	gui_win = App->gui->UI_CreateWin({ 0,0 }, w, h, 0, false);

	return true;
}

bool j1CutSceneManager::PreUpdate()
{
	return true;
}

bool j1CutSceneManager::Update(float dt)
{
	//Remove from active actions the ones already finished.
	for (std::list<CutsceneAction*>::iterator act = active_actions.begin(); act != active_actions.end();)
	{
		if ((*act)->start + (*act)->duration < scene_timer.ReadSec())
		{
			RELEASE(*act);
			act = active_actions.erase(act);
		}
		else
			++act;
	}

	//Add to active actions the ones that need to start
	while (!remaining_actions.empty() && remaining_actions.top()->start < scene_timer.ReadSec())
	{
		active_actions.push_back(remaining_actions.top());
		remaining_actions.pop();
	}

	PerformActions(dt);

	UpdateElements(dt);
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

void j1CutSceneManager::Play(const char * path)
{
	Load(path);
	scene_timer.Start();
}

void j1CutSceneManager::Load(const char * path)
{
	pugi::xml_document doc;
	App->xmls->LoadXML(path, doc);

	pugi::xml_node elements = doc.child("file").child("elements");

	for (pugi::xml_node group = elements.child("type"); group; group = group.next_sibling("type"))
	{
		string type = group.attribute("group").as_string();

		if (type == "map")
		{
			for (pugi::xml_node map = group.child("map"); map; map = map.next_sibling("map"))
			{
				LoadMap(map);
			}
		}
		else if (type == "image")
		{
			for (pugi::xml_node image = group.child("image"); image; image = image.next_sibling("image"))
			{
				LoadImage(image);
			}
		}
		else if (type == "entity")
		{
			for (pugi::xml_node entity = group.child("entity"); entity; entity = entity.next_sibling("entity"))
			{
				LoadEntity(entity);
			}
		}
		else if (type == "music")
		{
			for (pugi::xml_node music = group.child("music"); music; music = music.next_sibling("music"))
			{
				LoadMusic(music);
			}
		}
		else if (type == "sound_effect")
		{
			for (pugi::xml_node fx = group.child("sound_effect"); fx; fx = fx.next_sibling("sound_effect"))
			{
				LoadSoundEffect(fx);
			}
		}
		else if (type == "text")
		{
			for (pugi::xml_node txt = group.child("text"); txt; txt = txt.next_sibling("text"))
			{
				LoadText(txt);
			}
		}
	}

	pugi::xml_node scene = doc.child("file").child("scene");

	for (pugi::xml_node act = scene.child("act"); act; act = act.next_sibling("act"))
	{
		string type = act.attribute("type").as_string();

		if (type == "move")
		{
			LoadMove(act);
		}
		else if (type == "modify")
		{
			LoadModify(act);
		}
		else
		{
			LoadAction(act);
		}
	}

}

elements_groups j1CutSceneManager::GetElementGroup(const char * ele) const
{
	for (std::list<CutsceneElement*>::const_iterator e = elements.begin(); e != elements.end(); ++e)
	{
		if ((*e)->name == ele)
			return (*e)->group;
	}

	return e_g_null;
}

void j1CutSceneManager::PerformActions(float dt)
{

}

void j1CutSceneManager::LoadMap(pugi::xml_node & node)
{
	if (node.attribute("preload").as_bool() == false)
	{
		if (App->map->Load(node.attribute("path").as_string()))
		{
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
				App->pathfinding->SetMap(w, h, data);

			RELEASE_ARRAY(data);
		}
	}

	CutsceneMap* m = new CutsceneMap(e_g_map, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool());

	elements.push_back(m);
}

void j1CutSceneManager::LoadImage(pugi::xml_node & node)
{
	CutsceneImage* i = new CutsceneImage(e_g_image, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool(), { node.attribute("pos_x").as_int(),node.attribute("pos_y").as_int() },
	{ node.attribute("rect_x").as_int(),node.attribute("rect_y").as_int(),node.attribute("rect_w").as_int(),node.attribute("rect_h").as_int() }, node.attribute("layer").as_int());

	elements.push_back(i);

}

void j1CutSceneManager::LoadEntity(pugi::xml_node & node)
{
	CutsceneEntity* e = new CutsceneEntity(e_g_entity, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool(), { node.attribute("pos_x").as_int(),node.attribute("pos_y").as_int() });

	elements.push_back(e);
}

void j1CutSceneManager::LoadMusic(pugi::xml_node & node)
{
	CutsceneMusic* m = new CutsceneMusic(e_g_music, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool());

	elements.push_back(m);
}

void j1CutSceneManager::LoadSoundEffect(pugi::xml_node & node)
{
	CutsceneSoundEffect* fx = new CutsceneSoundEffect(e_g_sound_effect, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool(), node.attribute("loops").as_int());

	if (node.attribute("active").as_bool() == true)
	{
		App->audio->PlayFx(fx->GetID(),fx->GetLoops());
	}

	elements.push_back(fx);
}

void j1CutSceneManager::LoadText(pugi::xml_node & node)
{
	CutsceneText* t = new CutsceneText(e_g_text, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool(), { node.attribute("pos_x").as_int(),node.attribute("pos_y").as_int() });

	t->SetText(node.attribute("txt").as_string());

	elements.push_back(t);
}

void j1CutSceneManager::UpdateElements(float dt)
{
	//Just maps and images need to be updated. Entities, music, sound_efects and text have modules that take care of them (entities, audio, gui)
	for (std::list<CutsceneElement*>::iterator ele = elements.begin(); ele != elements.end(); ++ele)
	{
		if ((*ele)->group == e_g_map)
		{
			if ((*ele)->active == true)
				App->map->Draw();
		}
		else if ((*ele)->group == e_g_image)
		{
			if ((*ele)->active == true)
			{
				CutsceneImage* image = dynamic_cast<CutsceneImage*>(*ele);
				App->view->LayerBlit(image->GetLayer(), image->GetTexture(), image->GetPos(), image->GetRect());
			}
		}
	}
}

void j1CutSceneManager::LoadAction(pugi::xml_node & node)
{
	string type = node.attribute("type").as_string();

	actions a = a_null;

	if (type == "action")
		a = a_action;
	else if (type == "play")
		a = a_play;
	else if (type == "stop")
		a = a_stop;
	else if (type == "enable")
		a = a_enable;
	else if (type == "disable")
		a = a_disable;

	CutsceneAction* action = new CutsceneAction(a, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

	remaining_actions.push(action);
}

void j1CutSceneManager::LoadMove(pugi::xml_node & node)
{
	CutsceneMove* m = new CutsceneMove(a_move, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

	pugi::xml_node move = node.child("move");

	// set destination
	m->dest = { move.attribute("x").as_int(),move.attribute("y").as_int() };
	

	// set reference type (local, map, global). if ref is not valit global is assigned as default
	reference_type ref = r_t_null;
	string ref_str = move.attribute("ref").as_string();

	if (ref_str == "local")
		ref = r_t_local;
	else if (ref_str == "map")
		ref = r_t_map;
	else
		ref = r_t_global;

	m->reference = ref;

	//Check is easing is needed
	if (move.attribute("bezier").as_bool() == true)
		m->bezier = new BezierEasing({ move.child("p1").attribute("x").as_float(),move.child("p1").attribute("y").as_float() }, { move.child("p2").attribute("x").as_float(),move.child("p2").attribute("y").as_float() });
	else
		m->bezier = new BezierEasing({ 0,0 }, { 0,0 });

	remaining_actions.push(m);
}

void j1CutSceneManager::LoadModify(pugi::xml_node & node)
{
	string ele = node.attribute("element").as_string();
	pugi::xml_node modify = node.child("modify");

	switch (GetElementGroup(ele.c_str()))
	{
	case e_g_entity:
	{
		CutsceneModifyEntity* me = new CutsceneModifyEntity(a_modify, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

		entity_actions e_action = e_a_null;
		string e_action_str = modify.attribute("action").as_string();

		if (e_action_str == "kill")
			e_action = e_a_kill;
		else if (e_action_str == "spawn")
			e_action = e_a_spawn;
		else
			e_action = e_a_change_pos;

		me->entity_action = e_action;

		me->pos = { modify.attribute("x").as_int(), modify.attribute("y").as_int() };

		remaining_actions.push(me);

		break; 
	}
	case e_g_image:
	{
		CutsceneModifyImage* mi = new CutsceneModifyImage(a_modify, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

		mi->var = modify.attribute("var").as_string();

		mi->rect = { modify.attribute("x").as_int(),  modify.attribute("y").as_int() , modify.attribute("w").as_int() , modify.attribute("h").as_int() };

		mi->path = modify.attribute("path").as_string();

		remaining_actions.push(mi);

		break; 
	}
	case e_g_text:
	{
		CutsceneModifyText* mt = new CutsceneModifyText(a_modify, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

		mt->txt = modify.attribute("txt").as_string();

		remaining_actions.push(mt);

		break;
	}
	default:
		break;
	}
}

void j1CutSceneManager::LoadChangeScene(pugi::xml_node & node)
{
	CutsceneChangeScene* cs = new CutsceneChangeScene(a_move, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

	cs->path = node.child("change_scene").attribute("path").as_string();

	remaining_actions.push(cs);
}

//-----------------------
// Element
//-----------------------

CutsceneElement::CutsceneElement(elements_groups group, const char * path, const char* name, bool active) : group(group), path(path), active(active), name(name)
{
	id = App->cutscene->GetNextID();
}
//----------------------

//-----------------------
// Map
//-----------------------

CutsceneMap::CutsceneMap(elements_groups group, const char * path, const char* name, bool active) : CutsceneElement(group, path, name, active)
{
}
//----------------------

//-----------------------
// Image
//-----------------------

CutsceneImage::CutsceneImage(elements_groups group, const char * path, const char* name, bool active, iPoint pos, SDL_Rect rect, int layer) : CutsceneElement(group, path, name, active), pos(pos), layer(layer)
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
int CutsceneImage::GetLayer() const
{
	return layer;
}
//--------------------

//-----------------------
// Entity
//-----------------------

CutsceneEntity::CutsceneEntity(elements_groups group, const char * path, const char* name, bool active, iPoint pos) : CutsceneElement(group, path, name, active)
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
//----------------------

//-----------------------
// Music
//-----------------------

CutsceneMusic::CutsceneMusic(elements_groups group, const char * path, const char* name, bool active) : CutsceneElement(group, path, name, active)
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
//------------------------

//-----------------------
// Sound Effect
//-----------------------

CutsceneSoundEffect::CutsceneSoundEffect(elements_groups group, const char * path, const char* name, bool active, int loops) : CutsceneElement(group, path, name, active), loops(loops)
{
	id = App->audio->LoadFx(path);
}

CutsceneSoundEffect::~CutsceneSoundEffect()
{
}
int CutsceneSoundEffect::GetID() const
{
	return id;
}
int CutsceneSoundEffect::GetLoops() const
{
	return loops;
}
//----------------

//-----------------------
// Text
//-----------------------

CutsceneText::CutsceneText(elements_groups group, const char * path, const char* name, bool active, iPoint pos) : CutsceneElement(group, path, name, active)
{
	text = App->cutscene->gui_win->CreateText(pos, App->font->default);
}

CutsceneText::~CutsceneText()
{
}

void CutsceneText::SetText(const char * txt)
{
	text->SetText(txt);
}
//---------------------

//---------------------
// Actions
//---------------------

CutsceneAction::CutsceneAction(actions action, const char * name, int start_time, int duration) : action(action), element_name(name), start(start_time), duration(duration)
{
}

CutsceneMove::CutsceneMove(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutsceneMove::~CutsceneMove()
{
	RELEASE(bezier);
}

CutsceneModifyEntity::CutsceneModifyEntity(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name,start_time,duration)
{
}

CutsceneModifyImage::CutsceneModifyImage(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutsceneModifyText::CutsceneModifyText(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutsceneChangeScene::CutsceneChangeScene(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutSceneAction_Cmp::CutSceneAction_Cmp()
{
}
