#pragma once

/**
*@file Uniform.h
*/
#ifndef UNIFORM_H_INCLUDE
#define UNIFORM_H_INCLUDE
#include <glm/glm.hpp>


namespace Uniform {
	//���_�V�F�[�_�̃p�����[�^�^
	struct VertexData {
		glm::mat4 matMVP;
		glm::mat4 matModel;
		glm::mat3x4 matNormal;
	};

	const int maxLightCount = 4; //���C�g�̐�

	/**
	*���C�g�f�[�^(�_����)
	*/
	struct PointLight
	{
		glm::vec4 position; //���W
		glm::vec4 color; //���邳
	};

	/**
	*���C�e�B���O�p�����[�^
	*/
	struct LightData {
		glm::vec4 ambientColor; //����
		PointLight light[maxLightCount]; //���C�g�̃��X�g
	};

	/**
	*�|�X�g�G�t�F�N�g�f�[�^
	*/
	struct PostEffectData {
		glm::mat4x4 matColor;
	};

}

#endif


