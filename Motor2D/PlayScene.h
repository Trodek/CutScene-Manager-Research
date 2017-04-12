#ifndef _PLAYSCENE_H_
#define _PLAYSCENE_H_

#include "Scene.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

};


#endif // !_PLAYSCENE_H_

