#pragma once

/**
*@file GLFWEW.h
*/
#ifndef GLFWEW_INCLULED
#define GLFWEW_INCLULED
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include  "GamePad.h"

namespace GLFWEW {

/**
*@GLFWとGLEWのラッパークラス
*/
class Window {
public:
	static Window& Instance();
	bool Init(int w, int h, const char*title);
	bool ShouldClose() const;
	void SwapBuffers() const;
	const GamePad&  GetGamePad()const;
	void UpdateGamePad();

private:
	Window();
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool isGLFWInitialized;
	bool isInitialized;
	GLFWwindow*window;
	GamePad gamepad;

};


}




#endif //GLFWEW_INCLUDED

