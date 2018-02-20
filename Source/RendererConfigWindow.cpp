#include "RendererConfigWindow.h"
#include "ModuleRenderer3D.h"
#include "Application.h"
#include "OpenGL.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

RendererConfigWindow::RendererConfigWindow()
{
	active = false;
	window_name = "Renderer Config";

	SetDefaultValues();
}

RendererConfigWindow::~RendererConfigWindow()
{
}

void RendererConfigWindow::DrawWindow()
{
	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);

	bool depth_test = App->renderer3D->GetActiveDepthTest();
	ImGui::Checkbox("GL_DEPTH_TEST", &depth_test);
	App->renderer3D->SetActiveDepthTest(depth_test);

	bool cull_test = App->renderer3D->GetActiveCullTest();
	ImGui::Checkbox("GL_CULL_TEST", &cull_test);
	App->renderer3D->SetActiveCullTest(cull_test);

	bool fog = App->renderer3D->GetActiveFog();
	ImGui::Checkbox("GL_FOG", &fog);
	App->renderer3D->SetActiveFog(fog);
	if (fog)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Test Fog");
		ImGui::SliderFloat4("Fog Color", fog_color, 0.0, 1.0);
		glFogfv(GL_FOG_COLOR, fog_color);
		ImGui::SliderFloat("Fog Density", &fog_density, 0.0f, 1.0f);
		glFogf(GL_FOG_DENSITY, fog_density);
		const char* modes[] = { "Linear", "EXP", "EXP2"};
		static int mode = 1; //1 = EXP Mode. This is the initial mode. 
		ImGui::Combo("Fog Mode", &mode, modes, IM_ARRAYSIZE(modes));
		glFogi(GL_FOG_MODE, mode);
	}

	if (!active)
	{
		SetDefaultValues();
		App->renderer3D->DisableTestLight();
	}

	ImGui::End();
}

void RendererConfigWindow::SetDefaultValues()
{

	//Fog
	fog_color[0] = 0.0f;
	fog_color[1] = 0.0f;
	fog_color[2] = 0.0f;
	fog_color[3] = 0.0f;
	fog_density = 1.0f;

	//Light
	light_ambient[0] = 0.2f;
	light_ambient[1] = 0.2f;
	light_ambient[2] = 0.2f;
	light_ambient[3] = 0.2f;

	light_diffuse[0] = 1.0f;
	light_diffuse[1] = 1.0f;
	light_diffuse[2] = 1.0f;
	light_diffuse[3] = 1.0f;

	light_specular[0] = 1.0f;
	light_specular[1] = 1.0f;
	light_specular[2] = 1.0f;
	light_specular[3] = 1.0f;

	light_constant_attenuation = 1.0f;
	light_linear_attenuation = 0.0f;
	light_quadratic_attenuation = 0.0f;

	light_position[0] = -1.0f;
	light_position[1] = 1.0f;
	light_position[2] = 1.0f;
	light_position[3] = 0.0f;

	light_spot_direction[0] = 0.0f;
	light_spot_direction[1] = 0.0f;
	light_spot_direction[2] = -1.0f;
}
