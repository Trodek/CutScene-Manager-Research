#include "j1Viewports.h"
#include "j1Window.h"
#include "j1Input.h"
#include "j1Console.h"

j1Viewports::j1Viewports()
{
	name = "viewports";
}

j1Viewports::~j1Viewports()
{
}

bool j1Viewports::Awake()
{
	bool ret = true;

	return ret;
}

bool j1Viewports::Start()
{
	bool ret = true;

	number_of_views = 1;
	scale = 1.0f;

	App->win->GetWindowSize(win_w, win_h);

	return ret;
}

bool j1Viewports::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool j1Viewports::Update(float dt)
{
	bool ret = true;

	// Blit different layers
	DoLayerPrint();
	// ---------------------

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		camera1.x++;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		camera1.x--;
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		camera1.y++;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		camera1.y--;

	return ret;
}

bool j1Viewports::PostUpdate()
{
	bool ret = true;

	return ret;
}

bool j1Viewports::CleanUp()
{
	bool ret = true;

	return ret;
}

void j1Viewports::SetViews(uint number)
{
	if (number > 0 && number < 5 && number != 3)
	{
		number_of_views = number;

		switch (number)
		{
		case 1:
			scale = 2.0f;
			break;
		case 2:
			scale = 1.0f;
			break;
		case 4:
			scale = 1.0f;
			break;
		default:
			break;
		}
	}
}

uint j1Viewports::GetViews()
{
	return number_of_views;
}

SDL_Rect j1Viewports::GetViewportRect()
{
	SDL_Rect ret = NULLRECT;

	ret = { -camera1.x, -camera1.y, (int)win_w, (int)win_h };
		
	return ret;
}

void j1Viewports::MoveCamera(int x, int y)
{

	camera1.x += x;
	camera1.y += y;
		
}

void j1Viewports::SetCamera(int x, int y)
{
	camera1.x = x;
	camera1.y = y;
}

void j1Viewports::CenterCamera(int x, int y)
{
	
		camera1.x = -x + win_w / 4;
		camera1.y = -y + win_h / 4;
		
}

void j1Viewports::LayerBlit(int layer, SDL_Texture * texture, iPoint pos, const SDL_Rect section, float scale, bool use_camera, SDL_RendererFlip _flip, double angle, int pivot_x, int pivot_y)
{
	layer_blit lblit(texture, pos, section, scale, use_camera, _flip, angle, pivot_x, pivot_y);

	layer_list1.Push(lblit, layer);
		
}

void j1Viewports::LayerDrawQuad(const SDL_Rect rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, int layer, bool use_camera)
{
	layer_quad q(rect, r, g, b, a, filled, use_camera);

	quad_list1.Push(q, layer);
		
}

void j1Viewports::LayerDrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera)
{
	layer_line l(x1, y1, x2, y2, r, g, b, a, use_camera);

	line_list.push_back(l);
}

void j1Viewports::LayerDrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int layer, bool filled, bool use_camera)
{
	layer_circle c(x1, y1, redius, r, g, b, a, filled, use_camera);

	circle_list1.Push(c, layer);
		
}

void j1Viewports::DoLayerPrint()
{
	// Print
	for (p2PQueue_item<layer_blit>* curr = layer_list1.start; curr != nullptr; curr = curr->next)
	{
		float blit_scale = (curr->data.scale != -1.0f) ? curr->data.scale : scale;
		if(curr->data.use_camera)
			App->render->Blit(curr->data.texture, curr->data.pos.x + camera1.x, curr->data.pos.y + camera1.y, &curr->data.section, blit_scale, curr->data.use_camera, curr->data.flip, curr->data.angle, curr->data.pivot_x, curr->data.pivot_y);
		else
			App->render->Blit(curr->data.texture, curr->data.pos.x, curr->data.pos.y, &curr->data.section, blit_scale, curr->data.use_camera, curr->data.flip, curr->data.angle, curr->data.pivot_x, curr->data.pivot_y);

	}
	
	for (p2PQueue_item<layer_quad>* curr = quad_list1.start; curr != nullptr; curr = curr->next)
	{
		App->render->DrawQuad({ curr->data.rect.x + camera1.x, curr->data.rect.y + camera1.y, curr->data.rect.w, curr->data.rect.h }, curr->data.r, curr->data.g, curr->data.b, scale, curr->data.a, curr->data.filled, curr->data.use_camera);
	}
	
	for (int i = 0; i < line_list.size(); i++)
	{
		layer_line curr = line_list.at(i);
		App->render->DrawLine(curr.x1 + camera1.x, curr.y1 + camera1.y, curr.x2 + camera1.x, curr.y2 + camera1.y, curr.r, curr.g, curr.b, scale, curr.a, curr.use_camera);
	}

	for (p2PQueue_item<layer_circle>* curr = circle_list1.start; curr != nullptr; curr = curr->next)
	{
		App->render->DrawCircle(curr->data.x1 + camera1.x, curr->data.y1 + camera1.y, curr->data.redius, curr->data.r, curr->data.g, curr->data.b, scale, curr->data.a, curr->data.filled, curr->data.use_camera);
	}


	layer_list1.Clear();

	quad_list1.Clear();

	line_list.clear();
}

void j1Viewports::OnCommand(std::list<std::string>& tokens)
{
}

void j1Viewports::OnCVar(std::list<std::string>& tokens)
{
	
}

void j1Viewports::SaveCVar(std::string & cvar_name, pugi::xml_node & node) const
{
}

