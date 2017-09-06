#pragma once
/**
*@file GamePad.h
*/

#ifndef GAMEPAD_H_INCLUDE
#define GAMEPAD_H_INCLUDE
#include <stdint.h>

/**
*ゲームパッド情報
*/
struct GamePad {

	enum {
		DPAD_UP = 0x0001,
		DPAD_DOWN = 0x0002,
		DPAD_LEFT = 0x0004,
		DPAD_RIGHT = 0x0008,
		START = 0x0010,
		A = 0x0020,
		B = 0x0040,
		X = 0x0080,
		Y = 0x0100,
		L = 0x0200,
		R = 0x0400,
	};
	uint32_t buttons;    //押されている間フラグがたつ
	uint32_t buttomDown; //押された瞬間だけフラグがたつ

};



#endif // !GAMEPAD_H_INCLUDE






