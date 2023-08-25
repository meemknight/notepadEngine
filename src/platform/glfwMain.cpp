#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <stb_truetype/stb_truetype.h>
#include "gl2d/gl2d.h"
#include <iostream>
#include <ctime>
#include "platformTools.h"
#include "config.h"
#include <raudio.h>
#include "platformInput.h"
#include "otherPlatformFunctions.h"
#include "gameLayer.h"
#include <fstream>
#include <chrono>
#include <notepad.h>

#define REMOVE_IMGUI 0

#if REMOVE_IMGUI == 0
	#include "imgui.h"
	#include "backends/imgui_impl_glfw.h"
	#include "backends/imgui_impl_opengl3.h"
	#include "imguiThemes.h"
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#undef min
#undef max


bool windowFocus = 1;

void windowSizeCallback(GLFWwindow *window, int x, int y)
{
	platform::internal::resetInputsToZero();
}


#pragma region platform functions

GLFWwindow *wind = 0;

bool makeTheWindowVisible = 0;

namespace platform
{

	bool writeEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ofstream f(name, std::ios::binary);

		if(!f.is_open())
		{
			return 0;
		}

		f.write((char*)buffer, size);

		f.close();

		return 1;
	}


	bool readEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ifstream f(name, std::ios::binary);

		if (!f.is_open())
		{
			return 0;
		}

		f.read((char *)buffer, size);

		f.close();

		return 1;
	}

};
#pragma endregion


int main()
{

#ifdef _WIN32
#ifdef _MSC_VER 
#if INTERNAL_BUILD
	AllocConsole();
	(void)freopen("conin$", "r", stdin);
	(void)freopen("conout$", "w", stdout);
	(void)freopen("conout$", "w", stderr);
	std::cout.sync_with_stdio();
#endif
#endif
#endif


#pragma region window and opengl

	permaAssertComment(glfwInit(), "err initializing glfw");
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_VISIBLE, makeTheWindowVisible);

	int w = 500;
	int h = 500;
	wind = glfwCreateWindow(w, h, "geam", nullptr, nullptr);
	glfwMakeContextCurrent(wind);
	glfwSwapInterval(2);

	glfwSetWindowSizeCallback(wind, windowSizeCallback);

	//permaAssertComment(gladLoadGL(), "err initializing glad");
	permaAssertComment(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "err initializing glad");


#pragma endregion

#pragma region gl2d
	gl2d::init();
#pragma endregion


#pragma region imgui
	#if REMOVE_IMGUI == 0
		ImGui::CreateContext();
		//ImGui::StyleColorsDark();
		imguiThemes::embraceTheDarkness();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
	
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			//style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 0.f;
			style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
		}
	
		ImGui_ImplGlfw_InitForOpenGL(wind, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	#endif
#pragma endregion

#pragma region audio
	//InitAudioDevice();

	//Music m = LoadMusicStream(RESOURCES_PATH "target.ogg");
	//Music m = {};
	//UpdateMusicStream(m);
	//StopMusicStream(m);
	//PlayMusicStream(m);

#pragma endregion

#pragma region init notepad

	permaAssertComment(openTheNotepadInstance(), "err initializing the notepad");

#pragma endregion

#pragma region initGame
	if (!initGame())
	{
		return 0;
	}
#pragma endregion


	//long lastTime = clock();
	
	auto stop = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(wind))
	{
		Sleep(2);
		//UpdateMusicStream(m);
		//PlayMusicStream(m);

	#pragma region deltaTime

		//long newTime = clock();
		//float deltaTime = (float)(newTime - lastTime) / CLOCKS_PER_SEC;
		//lastTime = clock();
		auto start = std::chrono::high_resolution_clock::now();

		float deltaTime = (std::chrono::duration_cast<std::chrono::nanoseconds>(start - stop)).count() / 1000000000.0;
		stop = std::chrono::high_resolution_clock::now();

		float augmentedDeltaTime = deltaTime;
		if (augmentedDeltaTime > 1.f / 10) { augmentedDeltaTime = 1.f / 10; }
	
	#pragma endregion

	#pragma region imgui
		#if REMOVE_IMGUI == 0
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		#endif
	#pragma endregion

	#pragma region game logic

		if (!gameLogic(augmentedDeltaTime))
		{
			closeGame();
			return 0;
		}

	#pragma endregion

	#pragma region reset flags

		//mouseMovedFlag = 0;
		platform::internal::updateAllButtons(deltaTime);
		platform::internal::resetTypedInput();

	#pragma endregion

	#pragma region notepad stuff

		if (!checkNotepadOpen()) { glfwSetWindowShouldClose(wind, true); }

		UpdateText();

	#pragma endregion

	#pragma region window stuff

		#pragma region imgui
				#if REMOVE_IMGUI == 0
				ImGui::Render();
				int display_w, display_h;
				glfwGetFramebufferSize(wind, &display_w, &display_h);
				glViewport(0, 0, display_w, display_h);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				// Update and Render additional Platform Windows
				// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
				//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					GLFWwindow* backup_current_context = glfwGetCurrentContext();
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
					glfwMakeContextCurrent(backup_current_context);
				}
			#endif
		#pragma endregion

		if (makeTheWindowVisible)
		{
			glfwSwapBuffers(wind);
		}

		glfwPollEvents();

	#pragma endregion
	
	#pragma region input
		{

			auto fromIndexToKey = [](int i)
			{
				if (i == VK_BACK) { return 8; }

				if (i >= 'A' && i <= 'Z')
				{
					int index = i - 'A';
					return index;
				}
				else if (i >= VK_NUMPAD0 && i <= VK_NUMPAD9)
				{
					int index = i - VK_NUMPAD0;
					return index;
				}

				if (i == VK_ACCEPT)
				{
					return (int)platform::Button::Enter;
				}
				else
				if (i == VK_ESCAPE)
				{
					return (int)platform::Button::Escape;
				}
				else
				if (i == VK_UP)
				{
					return (int)platform::Button::Up;
				}
				else
				if (i == VK_DOWN)
				{
					return (int)platform::Button::Down;
				}
				else
				if (i == VK_LEFT)
				{
					return (int)platform::Button::Left;
				}
				else
				if (i == VK_RIGHT)
				{
					return (int)platform::Button::Right;
				}
				else
				if (i == VK_CONTROL)
				{
					return (int)platform::Button::LeftCtrl;
				}else
				if (i == VK_TAB)
				{
					return (int)platform::Button::Tab;
				}
				
				return -1;

			};

			for (int i = 8; i < 128; i++)
			{
				int rez = GetAsyncKeyState(i);
				int key = fromIndexToKey(i);

				if (key < 0) { continue; }

				platform::internal::setButtonState(key, rez);

				if (rez & 0x01)
				{
					platform::internal::addToTypedInput(key);
				}
			}


			platform::internal::setLeftMouseState(GetAsyncKeyState(VK_LBUTTON));
			platform::internal::setRightMouseState(GetAsyncKeyState(VK_RBUTTON));
						

		}
	#pragma endregion


	}

	closeGame();

	return 0;
	//if you want the console to stay after closing the window
	//std::cin.clear();
	//std::cin.get();
}