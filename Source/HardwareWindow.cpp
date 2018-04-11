#include "HardwareWindow.h"
#include "SDL\include\SDL.h"
#include "SDL\include\SDL_opengl.h"
#include "Brofiler\Brofiler.h"

HardwareWindow::HardwareWindow()
{
	active = false;
	window_name = "Hardware";
}

HardwareWindow::~HardwareWindow()
{
}

void HardwareWindow::DrawWindow()
{
	BROFILER_CATEGORY("Editor - Hardware: DrawWindow", Profiler::Color::Black);

	ImGui::Begin(window_name.c_str(), &active,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders);

	SDL_version version;
	SDL_GetVersion(&version);

	ImGui::Text("SDL Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d.%d.%d", version.major, version.minor, version.patch);
	ImGui::Separator();
	ImGui::Text("CPUs:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
	ImGui::Text("RAM:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.3fGb ", SDL_GetSystemRAM() / 1024.f);
	ImGui::Text("Caps:");
	ImGui::SameLine();
	std::string str;
	if (SDL_Has3DNow()) str += "3DNow, ";
	if (SDL_HasAVX()) str += "AVX, ";
	if (SDL_HasAVX2()) str += "AVX2, ";
	if (SDL_HasAltiVec()) str += "AltiVec, ";
	if (SDL_HasMMX()) str += "MMX, ";
	if (SDL_HasRDTSC()) str += "RDTSC, ";
	if (SDL_HasSSE()) str += "SSE, ";
	if (SDL_HasSSE2()) str += "SSE2, ";
	if (SDL_HasSSE3()) str += "SSE3, ";
	if (SDL_HasSSE41()) str += "SSE41, ";
	if (SDL_HasSSE41()) str += "SSE42";
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", str.c_str());
	ImGui::Separator();
	//const GLubyte* gpu_vendor = glGetString(GL_VENDOR);
	//const GLubyte* gpu_renderer = glGetString(GL_RENDERER);
	//const GLubyte* gpu_version = glGetString(GL_VERSION);
	//ImGui::Text("GPU Vendor:");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", gpu_vendor);
	//ImGui::Text("GPU Model:");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", gpu_renderer);
	//ImGui::Text("GPU Driver:");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", gpu_version);

	//std::string str_vendor(reinterpret_cast<const char*>(gpu_vendor));
	////For any reason GL(EW)_NVX_gpu_memory_info and GL(EW)_ATI_meminfo are not detecting the gpus correctly
	//if (str_vendor.find("NVIDIA") != std::string::npos/*GLEW_NVX_gpu_memory_info*/)
	//{
	//	int availableKB;
	//	int totalKB;
	//	int dedicatedKB;
	//	int evictedKB;
	//	int evicted_counts;
	//	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availableKB);
	//	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalKB);
	//	glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicatedKB);
	//	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &evictedKB);
	//	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &evicted_counts);

	//	ImGui::Text("Total VRAM:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", totalKB / 1024);
	//	ImGui::Text("Available VRAM:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", availableKB / 1024);
	//	ImGui::Text("Dedicated VRAM:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", dedicatedKB / 1024);
	//	ImGui::Text("Evicted Memory:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", evictedKB / 1024);
	//	ImGui::Text("Evicted Memory Counts:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", evicted_counts / 1024);
	//}
	//else if (str_vendor.find("ATI") != std::string::npos/*GLEW_ATI_meminfo*/)
	//{
	//	int texture_memory[4];
	//	int vbo_memory[4];
	//	int renderbuffer_memory[4];
	//	glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, texture_memory);
	//	glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, vbo_memory);
	//	glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, renderbuffer_memory);
	//	ImGui::Text("Textures Memory:");
	//	ImGui::BulletText("Total free memory:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", texture_memory[0] / 1024);
	//	ImGui::BulletText("Largest available dedicated memory free block:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", texture_memory[1] / 1024);
	//	ImGui::BulletText("Total shared free system memory:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", texture_memory[2] / 1024);
	//	ImGui::BulletText("Largest shared free system memory block:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", texture_memory[3] / 1024);
	//	ImGui::Text("VBO Memory:");
	//	ImGui::BulletText("Total free memory:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", vbo_memory[0] / 1024);
	//	ImGui::BulletText("Largest available dedicated memory free block:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", vbo_memory[1] / 1024);
	//	ImGui::BulletText("Total shared free system memory:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", vbo_memory[2] / 1024);
	//	ImGui::BulletText("Largest shared free system memory block:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", vbo_memory[3] / 1024);
	//	ImGui::Text("Renderbuffer Memory:");
	//	ImGui::BulletText("Total free memory:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", renderbuffer_memory[0] / 1024);
	//	ImGui::BulletText("Largest available dedicated memory free block:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", renderbuffer_memory[1] / 1024);
	//	ImGui::BulletText("Total shared free system memory:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", renderbuffer_memory[2] / 1024);
	//	ImGui::BulletText("Largest shared free system memory block:");
	//	ImGui::SameLine();
	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMB", renderbuffer_memory[3] / 1024);
	//}
	//else
	//{
	//	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Intel GPU memory details not available");
	//}
	ImGui::End();
}
