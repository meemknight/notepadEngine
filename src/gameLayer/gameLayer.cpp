#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <notepad.h>

gl2d::Renderer2D renderer;

glm::ivec2 immageSize;
stbi_uc *immageData;

float sampleImmage(float x, float y)
{
	if (x < 0 || x>1 || y < 0 || y>1) { return 0; }

	glm::ivec2 pos = glm::ivec2(glm::vec2(x, y) * glm::vec2(immageSize - glm::ivec2(1,1)));

	glm::vec4 data;

	data.r = immageData[(pos.x + pos.y * immageSize.x) * 4] /255.f;
	data.g = immageData[(pos.x + pos.y * immageSize.x) * 4+1]/255.f;
	data.b = immageData[(pos.x + pos.y * immageSize.x) * 4+2]/255.f;
	data.a = immageData[(pos.x + pos.y * immageSize.x) * 4+3]/255.f;

	if (data.a < 0.1) { return 0; }

	return glm::length(glm::vec3(data));
}

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();


	int channels = 0;
	stbi_set_flip_vertically_on_load(0);
	immageData = stbi_load(RESOURCES_PATH "amogus.png", &immageSize.x, &immageSize.y, &channels, 4);

	
	return true;
}



bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);

#pragma endregion


	for (int y = 0; y < getNotepadBufferSize().y; y++)
		for (int x = 0; x < getNotepadBufferSize().x; x++)
		{
			writeInBuffer(x, y, ' ');
		}


	for (int y = 0; y < getNotepadBufferSize().y; y++)
		for (int x = 0; x < getNotepadBufferSize().x; x++)
		{

			glm::vec2 uv = glm::vec2(x, y) / glm::vec2(getNotepadBufferSize());

			auto s = sampleImmage(uv.x, uv.y);

			if (s < 0.1)
			{
				writeInBuffer(x, y, 'X');
			}
			else if (s < 0.6)
			{
				writeInBuffer(x, y, 'o');
			}
			else if (s < 0.8)
			{
				writeInBuffer(x, y, '.');
			}
			else
			{
				writeInBuffer(x, y, ' ');
			}


		}



	renderer.flush();


	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{


}
