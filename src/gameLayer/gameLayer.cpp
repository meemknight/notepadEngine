#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include "gameLayer.h"
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

gl2d::Texture t;


bool initGame(gl2d::FrameBuffer fbo)
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create(fbo.fbo);

	t.loadFromFile(RESOURCES_PATH "amogus.png");
	
	return true;
}



bool gameLogic(float deltaTime, gl2d::FrameBuffer fbo)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);

#pragma endregion


	static glm::vec2 pos = {100,100};

	renderer.renderRectangle({pos, 100, 100}, t);
	renderer.flush();

	if (platform::isKeyHeld(platform::Button::Left))
	{
		pos.x -= deltaTime * 50;
	}
	if (platform::isKeyHeld(platform::Button::Right))
	{
		pos.x += deltaTime * 50;
	}
	if (platform::isKeyHeld(platform::Button::Up))
	{
		pos.y -= deltaTime * 50;
	}
	if (platform::isKeyHeld(platform::Button::Down))
	{
		pos.y += deltaTime * 50;
	}


	//ImGui::ShowDemoWindow();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{


}
