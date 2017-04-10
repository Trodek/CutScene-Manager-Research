#include "Entity.h"
#include "GameObject.h"
#include "j1XMLLoader.h"
#include "Animation.h"
#include "CollisionFilters.h"
#include "j1Map.h"
#include "j1Viewports.h"
#include "j1Pathfinding.h"

Entity::Entity(iPoint pos, const char* xml_path)
{
	pugi::xml_document doc;

	if (xml_path != nullptr)
	{
		App->xmls->LoadXML(xml_path, doc);
		size = { doc.child("file").child("size").attribute("w").as_int(), doc.child("file").child("size").attribute("h").as_int() };
	}
	
	game_object = new GameObject(pos, size, App->cf->CATEGORY_PLAYER, App->cf->MASK_PLAYER, pbody_type::p_t_player, 0.0f);

	if (xml_path != nullptr)
	{
		game_object->SetTexture(game_object->animator->LoadAnimationsFromXML(doc, "animations"));
		game_object->SetAnimation("idle_down");

		pugi::xml_node stats_node = doc.child("file").child("stats");

		stats.life = stats.base_hp = stats.max_life = stats_node.attribute("hp").as_int();
		stats.base_speed = stats.speed = stats.restore_speed = stats_node.attribute("speed").as_int();

	}

	target_pos = App->map->WorldToMap(pos.x, pos.y);

}

Entity::~Entity()
{
}

bool Entity::Start()
{
	state = e_s_idle;
	side = f_s_down;
	SetIdle();
	return true;
}

bool Entity::PreUpdate()
{
	return true;
}

bool Entity::Update(float dt)
{
	CheckState();

	switch (state)
	{
	case e_s_idle:
		SetIdle();
		break;
	case e_s_move:
		speed = stats.speed*dt;
		DoMovement();
		break;
	case e_s_action:
		break;
	default:
		break;
	}
	return true;
}

void Entity::Draw(float dt)
{
	if (flip)
		App->view->LayerBlit(1, game_object->GetTexture(), { game_object->GetPos().x - size.x / 2, game_object->GetPos().y - size.y / 2 }, game_object->GetCurrentAnimationRect(dt), -1.0f, true, SDL_FLIP_HORIZONTAL);
	else
		App->view->LayerBlit(1, game_object->GetTexture(), { game_object->GetPos().x - size.x / 2, game_object->GetPos().y - size.y / 2 }, game_object->GetCurrentAnimationRect(dt), -1.0f, true, SDL_FLIP_NONE);
}

bool Entity::PostUpdate()
{
	return true;
}

void Entity::CleanUp()
{
}

void Entity::MoveToWorld(iPoint target_pos)
{
	iPoint prev_pos = this->target_pos;
	this->target_pos = App->map->WorldToMap(target_pos.x, target_pos.y);
	if (App->pathfinding->CreatePath(GetMapPos(), this->target_pos) != -1)
	{
		for (std::list<iPoint>::const_iterator it = App->pathfinding->GetLastPath()->begin(); it != App->pathfinding->GetLastPath()->end(); it++)
		{
			path.push_back(*it);
		}
		path_index = 0;
	}
	else
		this->target_pos = prev_pos;
}

void Entity::MoveToMap(iPoint target_pos)
{
	iPoint prev_pos = this->target_pos;
	this->target_pos = target_pos;
	if (App->pathfinding->CreatePath(GetMapPos(), this->target_pos) != -1)
	{
		for (std::list<iPoint>::const_iterator it = App->pathfinding->GetLastPath()->begin(); it != App->pathfinding->GetLastPath()->end(); it++)
		{
			path.push_back(*it);
		}
		path_index = 0;
	}
	else
		this->target_pos = prev_pos;
}

iPoint Entity::GetWorldPos() const
{
	return game_object->GetPos();
}

iPoint Entity::GetMapPos() const
{
	return App->map->WorldToMap(game_object->GetPos().x, game_object->GetPos().y);
}

void Entity::DoAction()
{
	state = e_s_action;

	switch (side)
	{
	case f_s_down:
		ActionDown();
		break;
	case f_s_up:
		ActionUp();
		break;
	case f_s_right:
		ActionRight();
		break;
	case f_s_left:
		ActionLeft();
		break;
	default:
		break;
	}
}

void Entity::Move(int delta_x, int delta_y)
{
	if (delta_x > 0)
	{
		if (delta_y > 0)
		{
			MoveDownRight(speed);
		}
		else if (delta_y < 0)
		{
			MoveUpRight(speed);
		}
		else {
			MoveRight(speed);
		}
	}
	else if (delta_x < 0)
	{
		if (delta_y > 0)
		{
			MoveDownLeft(speed);
		}
		else if (delta_y < 0)
		{
			MoveUpLeft(speed);
		}
		else {
			MoveLeft(speed);
		}
	}
	else
	{
		if (delta_y > 0)
		{
			MoveDown(speed);
		}
		else if (delta_y < 0)
		{
			MoveUp(speed);
		}
	}
}

void Entity::MoveUp(float speed)
{
	game_object->SetPos({ game_object->fGetPos().x, game_object->fGetPos().y - speed });
	side = f_s_up;
	flip = false;
	game_object->SetAnimation("run_up");
}

void Entity::MoveDown(float speed)
{
	game_object->SetPos({ game_object->fGetPos().x, game_object->fGetPos().y + speed });
	side = f_s_down;
	flip = false;
	game_object->SetAnimation("run_down");
}

void Entity::MoveLeft(float speed)
{
	game_object->SetPos({ game_object->fGetPos().x - speed, game_object->fGetPos().y });
	side = f_s_left;
	flip = true;
	game_object->SetAnimation("run_lateral");
}

void Entity::MoveRight(float speed)
{
	game_object->SetPos({ game_object->fGetPos().x + speed, game_object->fGetPos().y });
	side = f_s_right;
	flip = false;
	game_object->SetAnimation("run_lateral");
}

void Entity::MoveUpRight(float speed)
{
	fPoint s(speed * cos(DEGTORAD * 45), speed * sin(DEGTORAD * 45));
	game_object->SetPos({ game_object->fGetPos().x + s.x, game_object->fGetPos().y - s.y });
	side = f_s_right;
	flip = false;
}

void Entity::MoveDownRight(float speed)
{
	fPoint s(speed * cos(DEGTORAD * 45), speed * sin(DEGTORAD * 45));
	game_object->SetPos({ game_object->fGetPos().x + s.x, game_object->fGetPos().y + s.y });
	side = f_s_right;
	flip = false;
}

void Entity::MoveUpLeft(float speed)
{
	fPoint s(speed * cos(DEGTORAD * 45), speed * sin(DEGTORAD * 45));
	game_object->SetPos({ game_object->fGetPos().x - s.x, game_object->fGetPos().y - s.y });
	side = f_s_left;
	flip = true;
}

void Entity::MoveDownLeft(float speed)
{
	fPoint s(speed * cos(DEGTORAD * 45), speed * sin(DEGTORAD * 45));
	game_object->SetPos({ game_object->fGetPos().x - s.x, game_object->fGetPos().y + s.y });
	side = f_s_left;
	flip = false;
}

void Entity::ActionUp()
{
	game_object->SetAnimation("action_up");
	flip = false;
}

void Entity::ActionDown()
{
	game_object->SetAnimation("action_down");
	flip = false;
}

void Entity::ActionLeft()
{
	game_object->SetAnimation("action_left");
	flip = true;
}

void Entity::ActionRight()
{
	game_object->SetAnimation("action_right");
	flip = false;
}

void Entity::CheckState()
{
	switch (state)
	{
	case e_s_idle:
		if (target_pos != GetMapPos())
			state = e_s_move;
		break;
	case e_s_move:
		if (path_index == path.size())
			state = e_s_idle;
		break;
	case e_s_action:
		if (game_object->animator->GetCurrentAnimation()->Finished())
		{
			game_object->animator->GetCurrentAnimation()->Reset();
			state = e_s_idle;
		}
		break;
	default:
		break;
	}
}

void Entity::SetIdle()
{
	switch (side)
	{
	case f_s_down:
		game_object->SetAnimation("idle_down");
		flip = false;
		break;
	case f_s_up:
		game_object->SetAnimation("idle_up");
		flip = false;
		break;
	case f_s_right:
		game_object->SetAnimation("idle_lateral");
		flip = false;
		break;
	case f_s_left:
		game_object->SetAnimation("idle_lateral");
		flip = true;
		break;
	default:
		break;
	}
}

void Entity::DoMovement()
{
	if (path_index < path.size())
	{
		if (GetMapPos() == path.at(path_index))
			path_index++;
		else
		{
			iPoint target_pos = App->map->MapToWorld(path.at(path_index).x, path.at(path_index).y);
			target_pos.y += 16;
			target_pos.x += 16;

			Move(target_pos.x - GetWorldPos().x, target_pos.y - GetWorldPos().y);
		}
	}
	else
	{
		state = e_s_idle;
		path.clear();
	}
}