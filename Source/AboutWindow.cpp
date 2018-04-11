#include "AboutWindow.h"
#include "Brofiler\Brofiler.h"

AboutWindow::AboutWindow()
{
	active = false;
	window_name = "About";
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::DrawWindow()
{
	BROFILER_CATEGORY("Editor - About: DrawWindow", Profiler::Color::Black);
	
	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);
	ImGui::SetWindowFontScale(1.1f);
	ImGui::Text("The Creator v0.3");
	ImGui::SetWindowFontScale(1);
	ImGui::Text("The next generation 3D Game Engine");
	ImGui::Text("By Adria Martin & Sergi Perez");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("3rd Party Libraries used:");
	ImGui::BulletText("SDL 2.0.6");
	ImGui::BulletText("SDL Mixer 2.0.0");
	ImGui::BulletText("Cereal 1.2.2");
	ImGui::BulletText("Glew 2.0.0");
	ImGui::BulletText("ImGui 1.51");
	ImGui::BulletText("MathGeoLib 1.5");
	ImGui::BulletText("OpenGL 3.1");
	ImGui::BulletText("Assimp 3.1.1");
	ImGui::BulletText("Devil 1.7.8");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("License:");
	ImGui::Spacing();
	ImGui::Text("MIT License");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Copyright (c) 2017 Sergi Perez & Adria Martin");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this software and associated documentation files(the 'Software'), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions :");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("The above copyright notice and this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE.");

	ImGui::End();
}
