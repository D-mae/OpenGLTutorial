#pragma once

/**
*@file GameEngine.h
*/
#ifndef GAMEENGINE_H_INCLUDE
#define GAMEENGINE_H_INCLUDE
#include <GL/glew.h>
#include "UniformBuffer.h"
#include "OffscreenBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Entity.h"
#include "Uniform.h"
#include "GamePad.h"
#include <glm/glm.hpp>
#include <functional>
#include <random>


/**
*ゲームエンジンクラス
*/
class GameEngine {

public:
	typedef std::function<void(double)>UpdateFuncType;
	//カメラデータ
	struct CameraData {
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up;
	};

	static GameEngine& Instance();
	bool Init(int w, int h, const char*title);
	void Run();
	void UpdateFunc(const UpdateFuncType& func);
	const UpdateFuncType&UpdateFunc() const;



	bool LoadTextureFromFile(const char* filename);
	bool LoadMeshFromFile(const char*filename);
	Entity::Entity* AddEntity(const glm::vec3& pos, const char*meshName,
		const char*texName, Entity::Entity::UpdateFuncType func);
	void RemoveEntity(Entity::Entity*);
	void Light(int index, const Uniform::PointLight& light);
	const Uniform::PointLight& Light(int index)const;
	void AmbientLight(const glm::vec4& color);
	const glm::vec4& AmbientLight()const;
	void Camera(const CameraData& cam);
	const CameraData& Camera() const;
	std::mt19937& Rand();
	const GamePad& GetGamePad()const;

	const TexturePtr& GetTexture(const char* filename)const 
	{ return textureBuffer.find(filename)->second; }

private:
	GameEngine() = default;
	~GameEngine();
	GameEngine(const GameEngine&) = delete;
	GameEngine& operator=(const GameEngine&) = delete;
	void Update(double delta);
	void Render() const;

private:
	bool isInitialized = false;
	UpdateFuncType updateFunc;

private:

	GLuint vbo = 0;
	GLuint ibo = 0;
	GLuint vao = 0;
	UniformBufferPtr uboLight;
	UniformBufferPtr uboPostEffect;
	Shader::ProgramPtr progTutorial;
	Shader::ProgramPtr progColorFilter;
	OffscreenBufferPtr offscreen;

	std::unordered_map<std::string, TexturePtr> textureBuffer;
	Mesh::BufferPtr meshBuffer;
	Entity::BufferPtr entityBuffer;

	Uniform::LightData lightData;
	CameraData camera;
	std::mt19937 rand;

};



#endif


