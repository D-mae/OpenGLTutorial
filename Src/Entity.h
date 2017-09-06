#pragma once

/**
*@file Entity.h
*/

#ifndef ENTITY_H_INCLUDE
#define ENTITY_H_INCLUDE
#include<GL/glew.h>
#include"Mesh.h"
#include"Texture.h"
#include"Shader.h"
#include"UniformBuffer.h"
#include<glm/glm.hpp>
#include<glm/gtx/quaternion.hpp>
#include<memory>
#include<functional>

namespace Entity {

	class Entity;
	class Buffer;
	typedef std::shared_ptr<Buffer> BufferPtr; //エンティティバッファポインタ型

	//typedef std::function<CollisionRemovalTarget(Entity&, Entity&)>CollisionHandlerType;

	static const int maxGroupId = 31;

	/**
	*衝突判定形状
	*/
	struct CollisionData {
		glm::vec3 min;
		glm::vec3 max;
	};

	class Entity {
		friend class  Buffer;
	public:
		//状態更新関数型
		typedef std::function<
			void(Entity&, void*, double, const glm::mat4&, const glm::mat4&)> UpdateFuncType;

		void Position(const glm::vec3& v) { position = v; }
		const glm::vec3& Position()const { return position; }
		void Rotation(const glm::quat& q) { rotation = q; }
		const glm::quat& Rotation()const { return rotation; }
		void Scale(const glm::vec3& v) { scale = v; }
		const glm::vec3& Scale()const { return scale; }
		void Velocity(const glm::vec3& v) { velocity = v; }
		const glm::vec3& Velocity()const { return velocity; }
		void UpdateFunc(const UpdateFuncType& func) { updateFunc = func; }
		const UpdateFuncType& UpdateFunc()const { return updateFunc; }

		glm::mat4 CalcModeMatrix()const;

	private:
		Entity() = default;
		~Entity() = default;
		Entity(const Entity&) = default;
		Entity& operator=(const Entity&) = default;

	private:
		glm::vec3 position;
		glm::vec3 scale = glm::vec3(1, 1, 1);
		glm::quat rotation;
		glm::vec3 velocity;
		UpdateFuncType updateFunc;
		Mesh::MeshPtr mesh;
		TexturePtr texture;
		Shader::ProgramPtr program;
		GLintptr uboOffset;
		bool isActive = false;
	};

	/**
	*エンティティバッファ
	*/
	class Buffer {

	public:
		static BufferPtr Create(size_t maxEntityCount, GLsizeiptr ubSizePerEntity,
			int bindingPoint, const char*name);

		Entity*AddEntity(const glm::vec3& pos, const Mesh::MeshPtr& m, const TexturePtr& t,
			const Shader::ProgramPtr& p, Entity::UpdateFuncType func);
		void RemoveEntity(Entity*entity);
		void Update(double delta, const glm::mat4& matView, const glm::mat4& matProj);
		void Draw(const Mesh::BufferPtr& meshBuffer)const;

	private:
		Buffer() = default;
		~Buffer() = default;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

	private:
		//エンティティ用リンクリスト
		struct Link {
			void Insert(Link*e);
			void Remove();
			Link*prev = this;
			Link*next = this;
		};
		//リンク付きエンティティ
		struct LinkEntity : public Link,public Entity{};
		//リンク付きエンティティ配列の削除用関数オブジェクト
		struct EntityArrayDeleter { void operator()(LinkEntity*p) { delete[] p; } };

		std::unique_ptr<LinkEntity[], EntityArrayDeleter> buffer;
		size_t bufferSize;
		Link freeList;
		Link activeList;
		GLsizeiptr ubSizePerentity;
		UniformBufferPtr ubo;
		Link* itrUpdate = nullptr;
	};


}


#endif
