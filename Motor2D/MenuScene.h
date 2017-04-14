#ifndef _MENUSCENE_H_
#define _MENUSCENE_H_

#include "Scene.h"
#include "j1Gui.h"
#include "j1Render.h"

class Entity;

class MenuScene : public Scene
{
public:
	MenuScene();
	~MenuScene();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnColl(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB);
	void OnCommand(std::list<std::string>& tokens);

	void GoOptions();
	void GoMenu(); 

public:
	fPoint			   pos = NULLPOINT;

	UI_Window*		   menu_window = nullptr;

private:

	UI_Button*		   play_button = nullptr;

	UI_Text*		   play_text = nullptr; 

};


#endif
