/**
*@file Entity.cpp
*/

#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>

/**
*エンティティに関するコードを格納する名前空間
*/
namespace Entity {

	/**
	*移動、回転、拡縮行列を削除する
	*
	*@return TRS　行列
	*/
	glm::mat4 Entity::CalcModeMatrix()const {

		const glm::mat4 t = glm::translate(glm::mat4(), position);
		const glm::mat4 r = glm::mat4_cast(rotation);
		const glm::mat4 s = glm::scale(glm::mat4(),scale);
		return t*r*s;

	}

	/**
	*自分自身をリンクリストから切り離す
	*
	*自分はどこにも接続されていない状態にする
	*/
	void Buffer::Link::Remove() {
		next->prev = prev;
		prev->next = next;
		prev = this;
		next = this;
	}

	void Buffer::Update(double delta, const glm::mat4&matView, const glm::mat4& matProj) {
		uint8_t*p = static_cast<uint8_t*>(ubo->MapBuffer());
		for (itrUpdate = activeList.next; itrUpdate != &activeList; itrUpdate = itrUpdate->next) {
			LinkEntity& e = *static_cast<LinkEntity*>(itrUpdate);
			e.position += e.velocity * static_cast<float>(delta);
			if (e.updateFunc) {
				e.updateFunc(e, p + e.uboOffset, delta, matView, matProj);
			}
		}
		itrUpdate = nullptr;
		ubo->UnmapBuffer();
	}

	/**
	*リンクオブジェクトを自分の手前に追加する
	*
	*@param p 追加するリンクオブジェクトへのポインタ
	*
	*pを所属元のリンクリストから切り離し、自分の手前に追加する
	*/
	void Buffer::Link::Insert(Link*p) {
		p->Remove();
		p->prev = prev;
		p->next = this;
		prev->next = p;
		prev = p;
	}

	/**
	*エンティティバッファを作成する
	*
	*@param maxEntityCount 扱えるエンティティの最大数
	*@param ubsizePerEntity エンティティごとのUniform Bufferのバイト数
	*@param bindingPoint エンティティ用UBOのバインディングポイント
	*@param ubname 　エンティティ用Uniform Buffer　の名前
	*
	*@return 作成したエンティティバッファへのポインタ
	*/
	BufferPtr Buffer::Create(size_t maxEntityCount, GLsizeiptr ubSizePerEntity,
		int bindingPoint, const char*ubName) {
		struct Impl : Buffer{Impl(){} ~Impl(){} };
		BufferPtr p = std::make_shared<Impl>();
		if (!p) {
			std::cerr << "WARNING in Entity::Buffer::Create: バッファの作成に失敗" << std::endl;
			return{};
		}
		p->ubo = UniformBuffer::Create(
			maxEntityCount * ubSizePerEntity, bindingPoint, ubName);
		p->buffer.reset(new LinkEntity[maxEntityCount]);
		if (!p->ubo || !p->buffer) {
			std::cerr << "WARNING in Entity::Buffer::Create: バッファの作成に失敗" << std::endl;
			return{};
		}
		p->bufferSize = maxEntityCount;
		p->ubSizePerentity = ubSizePerEntity;
		GLintptr offset = 0;
		const LinkEntity* const end = &p->buffer[maxEntityCount];
		for (LinkEntity* itr = &p->buffer[0]; itr != end; ++itr) {
			itr->uboOffset = offset;
			p->freeList.Insert(itr);
			offset += ubSizePerEntity;
		}
		return p;
	}

	/**
	*
	*
	*@param 
	*@param 
	*@param 
	*@param 
	*@param 
	*
	*@return 
	*
	*
	*
	*/
	Entity*Buffer::AddEntity(const glm::vec3& position, const Mesh::MeshPtr& mesh,
		const TexturePtr& texture, const Shader::ProgramPtr& program,
		Entity::UpdateFuncType func) {

		if (freeList.prev == freeList.next) {
			std::cerr << "WARNING inEntity::Buffer::AdEntity:""空きエンティティがありません" << std::endl;
			return nullptr;
		}
		LinkEntity*entity = static_cast<LinkEntity*>(freeList.prev);
		activeList.Insert(entity);

		entity->position = position;
		entity->rotation = glm::quat();
		entity->scale = glm::vec3(1, 1, 1);
		entity->velocity = glm::vec3();
		entity->mesh = mesh;
		entity->texture = texture;
		entity->program = program;
		entity->updateFunc = func;
		entity->isActive = true;

		return entity;
	}

	/**
	*エンティティを削除する
	*
	*@param 削除するエンティティのポインタ
	*/
	void  Buffer::RemoveEntity(Entity*entity) {
		if (!entity || !entity->isActive) {
			std::cerr << "WARNING inEntity::Buffer::RemoveEntity:"
				"非アクティブなエンティティを削除しようとしました" << std::endl;
			return;
		}
		LinkEntity*p = static_cast<LinkEntity*>(entity);
		if (p < &buffer[0] || p >= &buffer[bufferSize]) {
			std::cerr << "WARNING inEntity::Buffer::RemoveEntity:"
				"異なるバッファから取得したエンティティを削除しようとしました" << std::endl;
			return;
		}
		if (p == itrUpdate) {
			itrUpdate = p->prev;
		}
		freeList.Insert(p);
		p->mesh.reset();
		p->texture.reset();
		p->program.reset();
		p->updateFunc = nullptr;
		p->isActive = false;
	}



	/**
	*アクティブなエンティティを描画する
	*
	*@param meshBuffer  描画に使用するメッシュバッファへのポインタ
	*/
	void Buffer::Draw(const Mesh::BufferPtr& meshBuffer)const {
		meshBuffer->BindVAO();
		for (const Link* itr = activeList.next; itr != &activeList; itr = itr->next) {
			const LinkEntity& e = *static_cast<const LinkEntity*>(itr);
			if (e.mesh && e.texture && e.program) {
				e.program->UseProgram();
				e.program->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, e.texture->Id());
				ubo->BindBufferRange(e.uboOffset, ubSizePerentity);
				e.mesh->Draw(meshBuffer);
			}
		}
	}



}



