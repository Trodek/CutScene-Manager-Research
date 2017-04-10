#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "p2Point.h"
#include <vector>

enum entity_states
{
	e_s_null,
	e_s_idle,
	e_s_move,
	e_s_action,
};

enum facing_side
{
	f_s_null,
	f_s_down,
	f_s_up,
	f_s_right,
	f_s_left,
};

class GameObject;
class PhysBody;
class b2Fixture;

class Stats
{
public:
	Stats() {};
	~Stats() {};

public:
	int base_speed = 0;
	int speed = 0;
	int restore_speed = 0;

	int base_hp = 100;
	int max_life = 100;
	int life = 100;

	int base_power = 0;
	int power = 0;
};

class Entity
{
public:
	Entity(iPoint pos, const char* xml_path = nullptr);
	virtual ~Entity();

	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt);
	virtual void Draw(float dt);
	virtual bool PostUpdate();

	virtual void CleanUp();

	virtual void OnColl(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB) {};
	virtual void OnCollEnter(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB) {};
	virtual void OnCollOut(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB) {};

	void MoveToWorld(iPoint target_pos);
	void MoveToMap(iPoint target_pos);

	iPoint GetWorldPos() const;
	iPoint GetMapPos() const;

	void DoAction();

private:

	virtual void Move(int delta_x, int delta_y);

	virtual void MoveUp(float speed);
	virtual void MoveDown(float speed);
	virtual void MoveLeft(float speed);
	virtual void MoveRight(float speed);

	virtual void MoveUpRight(float speed);
	virtual void MoveDownRight(float speed);
	virtual void MoveUpLeft(float speed);
	virtual void MoveDownLeft(float speed);

	virtual void ActionUp();
	virtual void ActionDown();
	virtual void ActionLeft();
	virtual void ActionRight();

	void CheckState();
	void SetIdle();
	void DoMovement();

public:

	bool				active = false;

	bool				to_delete = false;

	GameObject*			game_object = nullptr;

private:

	iPoint				size = NULLPOINT;

	iPoint				target_pos = NULLPOINT;
		
	entity_states		state = e_s_null;
	facing_side			side = f_s_null;

	float				speed = 0.0f;

	Stats				stats;

	bool				flip = false;

	std::vector<iPoint> path;

	int					path_index = 0;
};

#endif // !_ENTITY_H_

