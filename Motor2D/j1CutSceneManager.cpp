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
#include "p2Log.h"

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

	if (change_scene)
		ChangeScene();

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
				LoadCSImage(image);
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
		else if (type == "change_scene")
		{
			LoadChangeScene(act);
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

CutsceneElement * j1CutSceneManager::GetElement(const char * ele) const
{
	for (std::list<CutsceneElement*>::const_iterator e = elements.begin(); e != elements.end(); ++e)
	{
		if ((*e)->name == ele)
			return *e;
	}

	return nullptr;
}

void j1CutSceneManager::PerformActions(float dt)
{
	for (std::list<CutsceneAction*>::iterator act = active_actions.begin(); act != active_actions.end(); ++act)
	{
		CutsceneElement* element = GetElement((*act)->element_name.c_str());

		switch ((*act)->action)
		{
		case a_move:
		{
			CutsceneMove* move = static_cast<CutsceneMove*>(*act);

			PerformMove(element, move);

			break;
		}
		case a_action:
			PerformAction(element);
			break;
		case a_play:
			PerformPlay(element);
			break;
		case a_stop:
			PerformStop(element);
			break;
		case a_modify:
			PerformModify(element, *act);
			break;
		case a_enable:
			PerformEnable(element);
			break;
		case a_disable:
			PerformDisable(element);
			break;
		case a_change_scene:
			PerformChangeScene(*act);
			break;
		default:
			break;
		}
	}
}

void j1CutSceneManager::ChangeScene()
{
	uint win_w, win_h;
	App->win->GetWindowSize(win_w, win_h);

	float rel_time = change_scene_timer.ReadSec() / (change_scene_duration/2);

	switch (change_scene_effect)
	{
	case c_s_e_fade:
		int alpha;

		if (rel_time < 1)
			alpha = 255 * rel_time;
		else
		{
			if(!changed)
			{
				ClearScene();
				Play(new_scene.c_str());
				changed = true;
			}
			alpha = 255 - 255 * (rel_time - 1);
		}

		App->view->LayerDrawQuad({ 0,0,(int)win_w,(int)win_h }, 0, 0, 0, alpha, true, 200);
		break;
	case c_s_e_circle:
		break;
	case c_s_e_star:
		break;
	case c_s_e_null:
		ClearScene();
		Play(new_scene.c_str());
		change_scene = false;
		break;
	default:
		break;
	}

	if (rel_time >= 2)
		change_scene = false;
}

void j1CutSceneManager::ClearScene()
{
	for (std::list<CutsceneElement*>::iterator it = elements.begin(); it != elements.end();)
	{
		RELEASE(*it);
		it = elements.erase(it);
	}

	while (!remaining_actions.empty())
	{
		CutsceneAction* a = remaining_actions.top();
		RELEASE(a);
		remaining_actions.pop();
	}

	for (std::list<CutsceneAction*>::iterator it = active_actions.begin(); it != active_actions.end();)
	{
		RELEASE(*it);
		it = active_actions.erase(it);
	}

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

void j1CutSceneManager::LoadCSImage(pugi::xml_node & node)
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

	string effect = node.child("change_scene").attribute("effect").as_string();

	if (effect == "fade")
		cs->effect = c_s_e_fade;
	else if (effect == "circle")
		cs->effect = c_s_e_circle;
	else if (effect == "star")
		cs->effect = c_s_e_star;
	else
		cs->effect = c_s_e_null;

	remaining_actions.push(cs);
}

void j1CutSceneManager::PerformMove(CutsceneElement * ele, CutsceneMove * move)
{
	float rel_time = (scene_timer.ReadSec()-(float)move->start) / (float)move->duration;

	LOG("t:%f", move->bezier->GetEasingProgress(rel_time));

	switch (ele->group)
	{
	case e_g_image:
	{
		CutsceneImage* image = static_cast<CutsceneImage*>(ele);

		if (move->first_time)
		{
			move->initial_pos = image->GetPos();
			move->first_time = false;
		}

		int delta_x = move->dest.x -move->initial_pos.x;
		int delta_y = move->dest.y -move->initial_pos.y;

		image->Move(move->initial_pos.x + move->bezier->GetEasingProgress(rel_time)*delta_x, move->initial_pos.y + move->bezier->GetEasingProgress(rel_time)*delta_y);

		break;
	}
	case e_g_entity:
	{
		CutsceneEntity* entity = static_cast<CutsceneEntity*>(ele);

		switch (move->reference)
		{
		case r_t_local:
			entity->GetEntity()->MoveToWorld({ entity->GetEntity()->GetMapPos().x + move->dest.x,entity->GetEntity()->GetMapPos().y + move->dest.y });
			break;
		case r_t_global:
			entity->GetEntity()->MoveToWorld({ move->dest.x,move->dest.y });
			break;
		case r_t_map:
			entity->GetEntity()->MoveToMap({ move->dest.x,move->dest.y });
			break;
		default:
			break;
		}

		break;
	}	
	case e_g_text:
	{
		CutsceneText* text = static_cast<CutsceneText*>(ele);

		if (move->first_time)
		{
			move->initial_pos = text->GetText()->GetPos();
			move->first_time = false;
		}

		int delta_x = move->dest.x - move->initial_pos.x;
		int delta_y = move->dest.y - move->initial_pos.y;

		text->Move(move->initial_pos.x + move->bezier->GetEasingProgress(rel_time)*delta_x, move->initial_pos.y + move->bezier->GetEasingProgress(rel_time)*delta_y);

		break;
	}
	default:
		break;
	}


}

void j1CutSceneManager::PerformAction(CutsceneElement * ele)
{
	//Just entities can do actions now
	if (ele->group == e_g_entity)
	{
		CutsceneEntity* e = static_cast<CutsceneEntity*>(ele);

		e->GetEntity()->DoAction();
	}
}

void j1CutSceneManager::PerformPlay(CutsceneElement * ele)
{
	//Just music and sound effect can be played
	if (ele->group == e_g_music)
	{
		CutsceneMusic* music = static_cast<CutsceneMusic*>(ele);

		music->Play();
	}
	else if (ele->group == e_g_sound_effect)
	{
		CutsceneSoundEffect* fx = static_cast<CutsceneSoundEffect*>(ele);

		fx->Play();
	}

}

void j1CutSceneManager::PerformStop(CutsceneElement * ele)
{
	//Just Music can be stoped now
	if (ele->group == e_g_music)
	{
		App->audio->StopMusic();
	}

}

void j1CutSceneManager::PerformModify(CutsceneElement * ele, CutsceneAction * act)
{
	//Just entity, image and text can be modified
	if (ele->group == e_g_entity)
	{
		CutsceneEntity* e = static_cast<CutsceneEntity*>(ele);
		CutsceneModifyEntity* modify = static_cast<CutsceneModifyEntity*>(act);

		switch (modify->entity_action)
		{
		case e_a_kill:
			if (e->GetEntity() != nullptr)
			{
				App->entity->DeleteEntity(e->GetEntity());
				e->SetNull();
			}
			break;
		case e_a_spawn:
			if (e->GetEntity() == nullptr)
			{
				e->SetEntity(App->entity->CreateEntity(modify->pos, e->path.c_str()));
			}
			break;
		case e_a_change_pos:
			e->GetEntity()->MoveToWorld(modify->pos); //should be a teleport to the position
			break;
		default:
			break;
		}
	}
	else if (ele->group == e_g_image)
	{
		CutsceneImage* i = static_cast<CutsceneImage*>(ele);
		CutsceneModifyImage* modify = static_cast<CutsceneModifyImage*>(act);

		if (modify->var == "tex")
			i->ChangeTex(modify->path.c_str());
		else if (modify->var == "rect")
			i->ChangeRect(modify->rect);
		else if (modify->var == "both")
		{
			i->ChangeTex(modify->path.c_str());
			i->ChangeRect(modify->rect);
		}
			
	}
	else if (ele->group == e_g_text)
	{
		CutsceneText* t = static_cast<CutsceneText*>(ele);
		CutsceneModifyText* modify = static_cast<CutsceneModifyText*>(act);

		t->SetText(modify->txt.c_str());
	}
}

void j1CutSceneManager::PerformEnable(CutsceneElement * ele)
{
	switch (ele->group)
	{
	case e_g_map:
		ele->active = true;
		break;
	case e_g_image:
		ele->active = true;
		break;
	case e_g_entity:
	{
		CutsceneEntity* e = static_cast<CutsceneEntity*>(ele);

		e->active = true;
		e->GetEntity()->active = true;

		break;
	}
	case e_g_text:
	{
		CutsceneText* t = static_cast<CutsceneText*>(ele);

		t->active = true;
		t->GetText()->enabled = true;

		break;
	}
	default:
		break;
	}
}

void j1CutSceneManager::PerformDisable(CutsceneElement * ele)
{
	switch (ele->group)
	{
	case e_g_map:
		ele->active = false;
		break;
	case e_g_image:
		ele->active = false;
		break;
	case e_g_entity:
	{
		CutsceneEntity* e = static_cast<CutsceneEntity*>(ele);

		e->active = false;
		e->GetEntity()->active = false;

		break;
	}
	case e_g_text:
	{
		CutsceneText* t = static_cast<CutsceneText*>(ele);

		t->active = false;
		t->GetText()->enabled = false;

		break;
	}
	default:
		break;
	}
}

void j1CutSceneManager::PerformChangeScene(CutsceneAction * act)
{
	CutsceneChangeScene* cs = static_cast<CutsceneChangeScene*>(act);

	change_scene = true;
	change_scene_duration = cs->duration;
	change_scene_effect = cs->effect;
	new_scene = cs->path;
	change_scene_timer.Start();
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
void CutsceneImage::Move(float x, float y)
{
	pos.x = x;
	pos.y = y;
}
void CutsceneImage::ChangeTex(const char * path)
{
	texture = App->tex->LoadTexture(path);
}
void CutsceneImage::ChangeRect(SDL_Rect r)
{
	rect = r;
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
void CutsceneEntity::SetNull()
{
	entity = nullptr;
}
void CutsceneEntity::SetEntity(Entity * e)
{
	entity = e;
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
void CutsceneMusic::Play()
{
	App->audio->PlayMusic(path.c_str());
	playing = true;
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
void CutsceneSoundEffect::Play()
{
	App->audio->PlayFx(id, loops);
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
UI_Text * CutsceneText::GetText() const
{
	return text;
}
void CutsceneText::Move(float x, float y)
{
	text->SetPos({ (int)x,(int)y });
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
