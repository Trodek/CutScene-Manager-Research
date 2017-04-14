#include "MenuScene.h"
#include "j1Input.h"
#include "j1Viewports.h"
#include "j1Console.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "Entity.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "PlayScene.h"
#include "j1CutSceneManager.h"

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

bool MenuScene::Start()
{
	bool ret = false;

	change_scene = false;

	SDL_Rect screen = App->view->GetViewportRect();
	menu_window = App->gui->UI_CreateWin(iPoint(0, 0), screen.w, screen.h, 0, false);

	// Play ---
	play_button = menu_window->CreateButton(iPoint(screen.w/2 - 110, 150), 223, 60, false);
	play_button->AddImage("idle", { 128, 52, 220, 55 });
	play_button->SetImage("idle");

	play_text = menu_window->CreateText(iPoint(screen.w/2 - 50, 160), App->font->default);
	play_text->SetText("Play Test");
	play_text->click_through = true;
	// --------

	return true;
}

bool MenuScene::PreUpdate()
{
	return true;
}

bool MenuScene::Update(float dt)
{
	if (play_button->MouseClickEnterLeft())
	{
		App->cutscene->Play("test_cutscene.xml", this);
	}

	return true;
}

bool MenuScene::PostUpdate()
{
	

	return true;
}

bool MenuScene::CleanUp()
{
	// Free UI
	if(App->scene->GetCurrentScene() != App->scene->menu_scene)
		App->gui->DeleteElement(menu_window);
	// -------

	
	return true;
}

void MenuScene::OnColl(PhysBody * bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{

}

void MenuScene::OnCommand(std::list<std::string>& tokens)
{
}

void MenuScene::GoOptions()
{

}

void MenuScene::GoMenu()
{

}
