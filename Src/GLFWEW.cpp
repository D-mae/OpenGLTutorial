/**
*@file GLFWEW.cpp
*/
#include "GLFWEW.h"
#include <iostream>

/**
*GLFWからのエラー報告を処理する
*
*@param エラーの番号
*@param エラーの内容
*/
void ErrorCallback(int error, const char*desc) {
	std::cerr << "ERROR:" << desc << std::endl;
}

///GLFWとGLEWをラップするための名前空間
namespace GLFWEW {

	/**
	* シングルトンインスタンス
	*
	*@return Windowのシングルトンインスタンス
	*/
	Window& Window::Instance() {
		static Window instance;
		return instance;
	}

	/**
	*コンストラクタ
	*/
	Window::Window() : isGLFWInitialized(false), isInitialized(false), window(nullptr) {
	}

	/**
	*デストラクタ
	*/
	Window::~Window() {
		if (isGLFWInitialized) {
			glfwTerminate();
		}
	}

	/**
	*GLFW/GLEWの初期化
	*
	*@param w　ウィンドウの描画範囲の幅(ピクセル)
	*@param h　ウィンドウの描画範囲の高さ(ピクセル)
	*@param title　ウィンドウタイトル(UTF-8の0終端文字列)
	*
	*@retval true　初期化成功
	*@retval false　初期化失敗
	*/
	bool Window::Init(int w, int h, const char* title) {

		if (isInitialized) {
			std::cerr << "ERROR: GLFWEWは既に初期化されています" << std::endl;
			return false;
		}

		if (!isGLFWInitialized) {
			glfwSetErrorCallback(ErrorCallback);
			if (glfwInit() != GL_TRUE) {
				return false;
			}
			isGLFWInitialized = true;
		}

		if (!window) {
			window = glfwCreateWindow(w, h, title, nullptr, nullptr);
			if (!window) {
				return false;
			}
			glfwMakeContextCurrent(window);
		}

		if (glewInit() != GLEW_OK) {
			std::cerr << "ERROR: GLEWの初期化に失敗しました" << std::endl;
			return false;
		}

		const GLubyte*renderer = glGetString(GL_RENDERER);
		std::cout << "Renderer:" << renderer << std::endl;
		const GLubyte*version = glGetString(GL_VERSION);
		std::cout << "Version:" << version << std::endl;
		const GLubyte*extensions = glGetString(GL_EXTENSIONS);
		std::cout << "Extensions:" << extensions << std::endl;
		isInitialized = true;
		return true;
	}

	/**
	*ウィンドウを閉じるべきか調べる
	*
	*@retval　true 閉じる
	*@retval false 閉じない
	*/
	bool Window::ShouldClose()const {
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	*フロントバッファとバックバッファを切り替える
	*/
	void Window::SwapBuffers()const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	/**
	*ゲームパッドの状態を取得する
	*
	*@return ゲームパッドの状態
	*/
	const GamePad& Window::GetGamePad()const {
		return gamepad;
	}

	/**
	*ジョイスティックのアナログ入力装置ID
	*
	*@note XBOX360コントローラー基準
	*/
	enum GLFWAXESID {
		GLFWAXESID_LeftX,
		GLFWAXESID_LeftY,
		GLFWAXESID_BackX,
		GLFWAXESID_RightX,
		GLFWAXESID_RightY,
	};

	/**
	*ジョイスティックのデジタル入力装置ID
	*
	*@note XBOX360コントローラー基準
	*/
	enum GLFWBUTTONID {
		GLFWBUTTONID_A,
		GLFWBUTTONID_B,
		GLFWBUTTONID_X,
		GLFWBUTTONID_Y,
		GLFWBUTTONID_L,
		GLFWBUTTONID_R,
		GLFWBUTTONID_Back,
		GLFWBUTTONID_Start,
		GLFWBUTTONID_LThumb,
		GLFWBUTTONID_RThumb,
		GLFWBUTTONID_Up,
		GLFWBUTTONID_Right,
		GLFWBUTTONID_Down,
		GLFWBUTTONID_Left,
	};

	/**
	*ゲームパッドの状態を更新
	*/
	void Window::UpdateGamePad() {

		const uint32_t prevButtons = gamepad.buttons;
		int axesCount, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const uint8_t* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		if (axes && buttons && axesCount >= 2 && buttonCount >= 8) {
			gamepad.buttons &= ~(GamePad::DPAD_UP | GamePad::DPAD_DOWN | GamePad::DPAD_LEFT | GamePad::DPAD_RIGHT);
			static const float threshould = 0.3f;
			if (axes[GLFWAXESID_LeftY] >= threshould) {
				gamepad.buttons |= GamePad::DPAD_UP;
			}
			else if (axes[GLFWAXESID_LeftY] <= -threshould) {
				gamepad.buttons |= GamePad::DPAD_DOWN;
			}
			if (axes[GLFWAXESID_LeftX] >= threshould) {
				gamepad.buttons |= GamePad::DPAD_LEFT;
			}
			else if (axes[GLFWAXESID_LeftX] <= -threshould) {
				gamepad.buttons |= GamePad::DPAD_RIGHT;
			}
			static const struct {
				int glfwCode;
				uint32_t gamepadCode;
			}keyMap[] = {
				{ GLFWBUTTONID_A,GamePad::A},
				{ GLFWBUTTONID_B,GamePad::B },
				{ GLFWBUTTONID_X,GamePad::X },
				{ GLFWBUTTONID_Y,GamePad::Y },
				{ GLFWBUTTONID_Start,GamePad::START },
			};
			for (const auto& e : keyMap) {
				if (buttons[e.glfwCode] == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadCode;
				}
				else if (buttons[e.glfwCode] == GLFW_RELEASE) {
					gamepad.buttons &= ~e.gamepadCode;
				}
			}
		}
		else {
			static const struct {
				int glfwCode;
				uint32_t gamepadCode;
			}keyMap[] = {
				{ GLFW_KEY_UP,GamePad::DPAD_UP},
				{ GLFW_KEY_DOWN,GamePad::DPAD_DOWN },
				{ GLFW_KEY_LEFT,GamePad::DPAD_LEFT },
				{ GLFW_KEY_RIGHT,GamePad::DPAD_RIGHT },
				{ GLFW_KEY_ENTER,GamePad::START },
				{ GLFW_KEY_A,GamePad::A },
				{ GLFW_KEY_S,GamePad::B },
				{ GLFW_KEY_X,GamePad::X },
				{ GLFW_KEY_Y,GamePad::Y },
			};
			for (const auto& e : keyMap) {
				const int key = glfwGetKey(window, e.glfwCode);
				if (key == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadCode;
				}
				else if (key == GLFW_RELEASE) {
					gamepad.buttons &= ~e.gamepadCode;
				}
			}
		}

		gamepad.buttomDown = gamepad.buttons & prevButtons;
	}


}//namespace GLFWEW

