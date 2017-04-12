#include "PlayScene.h"
#include "j1CutSceneManager.h"

PlayScene::PlayScene()
{
}

PlayScene::~PlayScene()
{
}

bool PlayScene::Start()
{
	App->cutscene->Play("test_cutscene.xml");

	return true;
}

bool PlayScene::PreUpdate()
{
	return true;
}

bool PlayScene::Update(float dt)
{
	return true;
}

bool PlayScene::PostUpdate()
{
	return true;
}

bool PlayScene::CleanUp()
{
	return true;
}
