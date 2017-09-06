#pragma once

/**
*@file Uniform.h
*/
#ifndef UNIFORM_H_INCLUDE
#define UNIFORM_H_INCLUDE
#include <glm/glm.hpp>


namespace Uniform {
	//頂点シェーダのパラメータ型
	struct VertexData {
		glm::mat4 matMVP;
		glm::mat4 matModel;
		glm::mat3x4 matNormal;
	};

	const int maxLightCount = 4; //ライトの数

	/**
	*ライトデータ(点光源)
	*/
	struct PointLight
	{
		glm::vec4 position; //座標
		glm::vec4 color; //明るさ
	};

	/**
	*ライティングパラメータ
	*/
	struct LightData {
		glm::vec4 ambientColor; //環境光
		PointLight light[maxLightCount]; //ライトのリスト
	};

	/**
	*ポストエフェクトデータ
	*/
	struct PostEffectData {
		glm::mat4x4 matColor;
	};

}

#endif


