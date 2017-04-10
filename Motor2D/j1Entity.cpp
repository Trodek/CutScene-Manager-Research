#include "j1Entity.h"
#include "p2Log.h"
#include "GameObject.h"
#include "Functions.h"
#include "j1Timer.h"
#include "Entity.h"



j1Entity::j1Entity()
{
	name = "entity";
}

j1Entity::~j1Entity()
{
}

bool j1Entity::Awake(pugi::xml_node &)
{
	bool ret = true;


	return ret;
}

bool j1Entity::Start()
{
	bool ret = true;


	return ret;
}

bool j1Entity::PreUpdate()
{
	bool ret = true;

	RemoveEntities();

	for(list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
		ret = (*it)->PreUpdate();

	return ret;
}

bool j1Entity::Update(float dt)
{
	bool ret = true;

	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
	{
		if ((*it)->active == true)
		{
			ret = (*it)->Update(dt);
			(*it)->Draw(dt);
		}
	}

	return ret;
}

bool j1Entity::PostUpdate()
{
	bool ret = true;

	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
		ret = (*it)->PostUpdate();

	return ret;
}

bool j1Entity::CleanUp()
{
	bool ret = true;

	ClearEntities();

	return ret;
}

void j1Entity::OnCollision(PhysBody * bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{
	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
	{
		(*it)->OnColl(bodyA, bodyB, fixtureA, fixtureB);
	}
}

void j1Entity::OnCollisionEnter(PhysBody * bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{
	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
	{
		if ((*it) != nullptr)
			(*it)->OnCollEnter(bodyA, bodyB, fixtureA, fixtureB);
	}
}

void j1Entity::OnCollisionOut(PhysBody * bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{
	if (!entity_list.empty())
	{
		for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
		{
			if ((*it) != nullptr)
				(*it)->OnCollOut(bodyA, bodyB, fixtureA, fixtureB);
		}
	}
}

Entity* j1Entity::CreateEntity(iPoint pos, const char* xml)
{
	Entity* ret = new Entity(pos,xml);

	if (ret != nullptr)
	{
		ret->Start();
		entity_list.push_back(ret);
	}
	else
		LOG("Entity creation returned nullptr");

	return ret;
}

void j1Entity::ClearEntities()
{
	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
	{
		if ((*it) != nullptr)
		{
			(*it)->to_delete = true;
		}
	}	
}

Entity * j1Entity::FindEntityByBody(PhysBody* body)
{
	Entity* ret = nullptr;

	// Look on entities
	for(list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
	{
		if ((*it)->game_object != nullptr && body == (*it)->game_object->pbody)
		{
			ret = *it;
			break;
		}
	}

	return ret;
}

void j1Entity::DeleteEntity(Entity* entity)
{
	entity->to_delete = true;
}

void j1Entity::RemoveEntities()
{
	if (!entity_list.empty())
	{
		for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end();)
		{
			if ((*it)->to_delete == true)
			{
				(*it)->CleanUp();
				RELEASE(*it);
				it = entity_list.erase(it);
			}
			else
				++it;
		}
	}
}

