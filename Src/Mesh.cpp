/**
*@file Mesh.cpp
*/


#include "Mesh.h"
#include <fbxsdk.h>
#include<iostream>

/**
*���f���f�[�^�Ǘ��̂��߂̖��O���
*/
namespace Mesh {

	struct Vertex {
		glm::vec3 position; //���W
		glm::vec4 color;    //�F
		glm::vec2 texCoord; //�e�N�X�`�����W
		glm::vec3 normal;   //�@��
	};


	/**
	* Vertex Buffer Object���쐬����
	*
	*@param size ���_�f�[�^�̃T�C�Y
	*@param data ���_�f�[�^�̃|�C���^
	*
	*@return �쐬����VBO
	*/
	GLuint CreateVBO(GLsizeiptr size, const GLvoid* data) {
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return vbo;
	}

	/**
	* Index Buffer Object�@���쐬����
	*
	*@param size �C���f�b�N�X�̃T�C�Y
	*@param data�@�C���f�b�N�X�f�[�^�̃|�C���^
	*
	*@return �쐬����IBO
	*/
	GLuint CreateIBO(GLsizeiptr size, const GLvoid*data) {

		GLuint ibo = 0;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return ibo;
	}

	/**
	*���_�A�g���r���[�g��ݒ肷��
	*
	*@param ���_�A�g���r���[�g�̃C���f�b�N�X
	*@param ���_�f�[�^�^��
	*@param ���_�A�g���r���[�g�ɐݒ肷��cls�̃����o�ϐ���
	*/
#define SetVertexAttribPointer(index,cls,mbr)SetVertexAttribPointerI(\
 index,\
 sizeof(cls::mbr) / sizeof(float),\
 sizeof(cls),\
 reinterpret_cast<GLvoid*>(offsetof(cls,mbr)))

	void SetVertexAttribPointerI(
		GLuint index, GLint size, GLsizei stride, const GLvoid*pointer)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);

	}

	/**
	*vertex Array Object�@���쐬����
	*
	*@param  vbo�@VAO�@�Ɋ֘A�t������VBO
	*@param ibo VAO�Ɋ֘A�t������IBO
	*
	*@return �쐬����VAO
	*/
	GLuint CreateVAO(GLuint vbo, GLuint ibo) {
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		SetVertexAttribPointer(0, Vertex, position);
		SetVertexAttribPointer(1, Vertex, color);
		SetVertexAttribPointer(2, Vertex, texCoord);
		SetVertexAttribPointer(3, Vertex, normal);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBindVertexArray(0);
		return vao;
	}

	/**
	*FBX�x�N�g����glm�x�N�g���ɕϊ�����
	*
	*@param fbxVec FBX�x�N�g��
	*
	*@return glm�x�N�g��
	*/
	template<typename T>
	glm::vec4 ToVec4(const T& fbxVec) {
		return glm::vec4(static_cast<float>(fbxVec[0]), static_cast<float>(fbxVec[1]),
			static_cast<float>(fbxVec[2]), static_cast<float>(fbxVec[3]));
	}
	template<typename T>
	glm::vec3 ToVec3(const T& fbxVec) {
		return glm::vec3(static_cast<float>(fbxVec[0]), static_cast<float>(fbxVec[1]),
			static_cast<float>(fbxVec[2]));
	}
	template<typename T>
	glm::vec2 ToVec2(const T& fbxVec) {
		return glm::vec2(static_cast<float>(fbxVec[0]), static_cast<float>(fbxVec[1]));
	}

	/**
	*FBX�I�u�W�F�N�g��j�����邽�߂̃w���p�[�\����
	*/
	template<typename T>
	struct Deleter {
		void operator()(T*p) { if (p) { p->Destroy(); } }
	};

	/**
	*�}�e���A���̉��f�[�^
	*/
	struct TemporaryMaterial {
		glm::vec4 color = glm::vec4(1);
		std::vector<uint32_t> indexBuffer;
		std::vector<Vertex>vertexBuffer;
		std::vector<std::string>textureName;
	};

	/**
	*���b�V���̉��f�[�^
	*/
	struct TemporaryMesh {
		std::string name;
		std::vector<TemporaryMaterial>materialList;
	};

	/**
	*FBX�f�[�^�𒆊ԃf�[�^�ɕϊ�����N���X
	*/
	struct FbxLoader {
		bool Load(const char*filename);
		bool Convert(FbxNode*node);
		bool LoadMesh(FbxNode*node);

		std::vector<TemporaryMesh>meshList;
	};

	/**
	*FBX�t�@�C����ǂݍ���
	*
	*@param filename FBX�t�@�C����
	*
	*@retval true   �ǂݍ��ݐ���
	*@retval false  �ǂݍ��ݎ��s
	*/
	bool FbxLoader::Load(const char*filename) {
		std::unique_ptr<FbxManager, Deleter<FbxManager>> fbxManager(FbxManager::Create());
		if (!fbxManager) {
			std::cerr << "ERROR:" << filename << "�̓ǂݍ��݂Ɏ��s(FbxManager�̍쐬�Ɏ��s)" <<
				std::endl;
			return false;
		}
		FbxScene*fbxScene = FbxScene::Create(fbxManager.get(), "");
		if (!fbxScene) {
			std::cerr << "ERROR:" << filename << "�̓ǂݍ��݂Ɏ��s(FbxScene�̍쐬�Ɏ��s)" <<
				std::endl;
			return false;
		}
		else {
			std::unique_ptr<FbxImporter, Deleter<FbxImporter>> fbxImporter(
				FbxImporter::Create(fbxManager.get(),""));
			const bool importStatus = fbxImporter->Initialize(filename);
			if (!importStatus || !fbxImporter->Import(fbxScene)) {
				std::cerr << "ERROR:" << filename << "�̓ǂݍ��݂Ɏ��s\n" <<
					fbxImporter->GetStatus().GetErrorString() << std::endl;
				return false;
			}
		}
		if (!Convert(fbxScene->GetRootNode())) {
			std::cerr << "ERROR:" << filename << "�̕ϊ��Ɏ��s" << std::endl;
			return false;
		}
		return true;
	}

	/**
	*FBX�f�[�^�����f�[�^�ɕϊ�����
	*
	*@param fbxNode �ϊ��Ώۂ�FBX�m�[�h�ւ̃|�C���^
	*
	*@retval true  �ϊ�����
	*@retval false �ϊ����s
	*/
	bool FbxLoader::Convert(FbxNode*fbxNode) {
		if (!fbxNode) {
			return true;
		}
		if (!LoadMesh(fbxNode)) {
			return false;
		}
		const int childCount = fbxNode->GetChildCount();
		for (int i = 0; i < childCount; ++i) {
			if (!Convert(fbxNode->GetChild(i))) {
				return false;
			}
		}
		return true;
	}

	/**
	*FBX���b�V�������f�[�^�ɕϊ�����
	*
	*@param fbxNode �ϊ��Ώۂ�FBX�m�[�h�ւ̃|�C���^
	*
	*@retval true  �ϊ�����
	*@retval false �ϊ����s
	*/
	bool FbxLoader::LoadMesh(FbxNode*fbxNode) {

		TemporaryMesh mesh;
		mesh.name = fbxNode->GetName();

		FbxMesh*fbxMesh = fbxNode->GetMesh();
		if (!fbxMesh) {
			return true;
		}
		if (!fbxMesh->IsTriangleMesh()) {
			std::cerr << "WARNIG:" << mesh.name << "�ɂ͎O�p�`�ȊO�̖ʂ��܂܂�Ă��܂�" << std::endl;

		}

		//�}�e���A������ǂݎ��
		const int materialCount = fbxNode->GetMaterialCount();
		mesh.materialList.reserve(materialCount);
		for (int i = 0; i < materialCount; ++i) {
			TemporaryMaterial material;
			if (FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(i)) {
				//�}�e���A���̐F����ǂݎ��
				const FbxClassId classId = fbxMaterial->GetClassId();
				if (classId == FbxSurfaceLambert::ClassId ||
					classId == FbxSurfacePhong::ClassId) {
					const FbxSurfaceLambert*pLambert =
						static_cast<const FbxSurfaceLambert*>(fbxMaterial);
					material.color = glm::vec4(ToVec3(pLambert->Diffuse.Get()),
						static_cast<float>(1.0f - pLambert->TransparencyFactor));

				}
			}
			mesh.materialList.push_back(material);
		}
		if (mesh.materialList.empty()) {
			mesh.materialList.push_back(TemporaryMaterial());
		}

		//���_�v�f�̗L���𒲂ׂ�
		const bool hasColor = fbxMesh->GetElementVertexColorCount() > 0;
		const bool hasTexcoord = fbxMesh->GetElementUVCount() > 0;
		const bool hasNormal = fbxMesh->GetElementNormalCount() > 0;

		//UV�Z�b�g���̃��X�g���擾����
		FbxStringList uvSetNameList;
		fbxMesh->GetUVSetNames(uvSetNameList);

		//�F����ǂݎ�鏀��
		//
		//
		FbxGeometryElement::EMappingMode colorMappingMode = FbxLayerElement::eNone;
		bool isColorDirectRef = true;
		const FbxLayerElementArrayTemplate<int>* colorIndexList = nullptr;
		const FbxLayerElementArrayTemplate<FbxColor>* colorList = nullptr;
		if (hasColor) {
			const FbxGeometryElementVertexColor*fbxColorList = fbxMesh->GetElementVertexColor();
			colorMappingMode = fbxColorList->GetMappingMode();
			isColorDirectRef = fbxColorList->GetReferenceMode() == FbxLayerElement::eDirect;
			colorIndexList = &fbxColorList->GetIndexArray();
			colorList = &fbxColorList->GetDirectArray();
		}

		//���_���ǂ̃}�e���A���ɑ����邩�������}�e���A���C���f�b�N�X���擾����
		const FbxLayerElementArrayTemplate<int>* materialIndexList = nullptr;
		if (FbxGeometryElementMaterial* fbxMaterialLayer = fbxMesh->GetElementMaterial()) {
			materialIndexList = &fbxMaterialLayer->GetIndexArray();
		}

		//�|���S�����Ɋ�Â��ĉ��f�[�^�o�b�t�@�̗e�ʂ�\�񂷂�
		const  int polygonCount = fbxMesh->GetPolygonCount();
		for (auto& e : mesh.materialList) {
			const size_t avarageCapacity = polygonCount / mesh.materialList.size();
			e.indexBuffer.reserve(avarageCapacity);
			e.vertexBuffer.reserve(avarageCapacity);
		}

		const FbxAMatrix matTRS(fbxNode->EvaluateGlobalTransform());
		const FbxAMatrix matR(FbxVector4(0, 0, 0), matTRS.GetR(), FbxVector4(1, 1, 1));
		const FbxVector4*const fbxControlPoints = fbxMesh->GetControlPoints();
		int polygonVertex = 0;
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
			for (int pos = 0; pos < 3; ++pos) {
				Vertex v;
				const int cpIndex = fbxMesh->GetPolygonVertex(polygonIndex, pos);

				//���_���W
				v.position = ToVec3(matTRS.MultT(fbxControlPoints[cpIndex]));

				//���_�J���[
				v.color = glm::vec4(1);
				if (hasColor) {
					switch (colorMappingMode) {
					case FbxLayerElement::eByControlPoint:
						v.color = ToVec4((*colorList)[
							isColorDirectRef ? cpIndex : (*colorIndexList)[cpIndex]]);
						break;
					case FbxLayerElement::eByPolygonVertex:
						v.color = ToVec4((*colorList)[
							isColorDirectRef ? polygonVertex : (*colorIndexList)[polygonVertex]]);
						break;
					default:
						break;
					}
				}

				//UV���W
				v.texCoord = glm::vec2(0);
				if (hasTexcoord) {
					FbxVector2 uv;
					bool unmapped;
					fbxMesh->GetPolygonVertexUV(polygonIndex, pos, uvSetNameList[0], uv, unmapped);
					v.texCoord = ToVec2(uv);
				}

				//�@��
				v.normal = glm::vec3(0, 0, 1);
				if (hasNormal) {
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, pos, normal);
					v.normal = glm::normalize(ToVec3(matR.MultT(normal)));
				}

				//���_�ɑΉ����鉼�}�e���A���ɒ��_�f�[�^�ƃC���f�b�N�X�f�[�^��ǉ�����
				TemporaryMaterial& materialData = mesh.materialList[
					materialIndexList ? (*materialIndexList)[polygonIndex] : 0];
				materialData.indexBuffer.push_back(
					static_cast<uint32_t>(materialData.vertexBuffer.size()));
				materialData.vertexBuffer.push_back(v);

				++polygonVertex;
			}
		}
		meshList.push_back(std::move(mesh));

		return true;
	}

	/**
	*�R���X�g���N�^
	*
	*@param meshName ���b�V���f�[�^��
	*@param begin    �`�悷��}�e���A���̐擪�C���f�b�N�X
	*@param end      �`�悷��҂Ă�����̏I�[�C���f�b�N�X
	*/
	Mesh::Mesh(const std::string& meshName,size_t begin,size_t end) : 
		name(meshName),beginMaterial(begin),endMaterial(end){}


	/**
	*���b�V����`�悷��
	*
	*@param buffer �`��Ɏg�p���郁�b�V���o�b�t�@�ւ̃|�C���^
	*/
	void Mesh::Draw(const BufferPtr& buffer) const {
		if (!buffer) {
			return;
		}
		if (buffer->GetMesh(name.c_str()).get() != this) {
			std::cerr << "WARNING: �o�b�t�@�ɑ��݂��Ȃ����b�V��'" << name <<
				"'��`�悵�悤�Ƃ��܂���" << std::endl;
			return;
		}
		for (size_t i = beginMaterial; i < endMaterial; ++i) {
			const Material& m = buffer->GetMaterial(i);
			glDrawElementsBaseVertex(GL_TRIANGLES, m.size, m.type, m.offset, m.baseVertex);
		}
	}

	/**
	*���b�V���o�b�t�@���쐬����
	*
	*@param vboSize �o�b�t�@�Ɋi�[�\�ȑ����_��
	*@param iboSize �o�b�t�@�Ɋi�[�\�ȑ��C���f�b�N�X��
	*/
	BufferPtr Buffer::Create(int vboSize, int iboSize) {
		struct Impl : Buffer{Impl() {} ~Impl(){} };
		BufferPtr p = std::make_shared<Impl>();
		p->vbo = CreateVBO(vboSize * sizeof(Vertex), nullptr);
		if (!p->vbo) {
			return{};
		}

		p->ibo = CreateIBO(iboSize * sizeof(uint32_t), nullptr);
		if (!p->ibo) {
			return{};
		}
		p->vao = CreateVAO(p->vbo, p->ibo);
		if (!p->vao) {
			return{};
		}
		return p;
	}

	/**
	*�f�X�g���N�^
	*/
	Buffer::~Buffer() {
		if (vao) {
			glDeleteVertexArrays(1, &vao);
		}
		if (ibo) {
			glDeleteBuffers(1, &ibo);
		}
		if (vbo) {
			glDeleteBuffers(1, &vbo);
		}
	}

	/**
	*���b�V�����t�@�C������ǂݍ���
	*
	*@param �@filename�@���b�V���t�@�C����
	*
	*@retval true  �ǂݍ��ݐ���
	*@retval false �ǂݍ��ݎ��s
	*/
	bool Buffer::LoadMeshFromFile(const char*filename) {
		FbxLoader loader;
		if (!loader.Load(filename)) {
			return false;
		}

		GLint64 vboSize = 0;
		GLint64 iboSize = 0;
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vboSize);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glGetBufferParameteri64v(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &iboSize);
		for (TemporaryMesh&e : loader.meshList) {
			for (TemporaryMaterial&material : e.materialList) {
				const GLsizeiptr verticesBytes = material.vertexBuffer.size() * sizeof(Vertex);
				if (vboEnd + verticesBytes >= vboSize) {
					std::cerr << "WARNING: VBO�T�C�Y���s�����Ă��܂�(" <<
						vboEnd << '/' << vboSize << ')' << std::endl;
					continue;
				}
				const GLsizei indexSize = static_cast<GLsizei>(material.indexBuffer.size());
				const GLsizeiptr indicesBytes = indexSize * sizeof(uint32_t);
				if (iboEnd + indicesBytes >= iboSize) {
					std::cerr << "WARNING: IBO�T�C�Y���s�����Ă��܂�(" <<
						iboEnd << '/' << iboSize << ')' << std::endl;
					continue;
				}
				glBufferSubData(GL_ARRAY_BUFFER, vboEnd, verticesBytes, material.vertexBuffer.data());
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, iboEnd, indicesBytes, material.indexBuffer.data());
				const GLint baseVertex = static_cast<uint32_t>(vboEnd / sizeof(Vertex));
				materialList.push_back({ GL_UNSIGNED_INT,indexSize,
				reinterpret_cast<GLvoid*>(iboEnd),baseVertex,material.color });
				vboEnd += verticesBytes;
				iboEnd += indicesBytes;
			}

			struct Impl : public Mesh {
				Impl(const std::string& n,size_t b,size_t e) : Mesh(n,b,e){}
				~Impl(){}
			};
			const size_t endMaterial = materialList.size();
			const size_t beginMaterial = endMaterial - e.materialList.size();
			meshList.insert(std::make_pair(e.name, std::make_shared<Impl>(e.name, beginMaterial, endMaterial)));
		}
		return true;
	}

	/**
	*���b�V�����擾����
	*
	*@param name ���b�V����
	*
	*@return name �ɑΉ����郁�b�V���ւ̃|�C���^
	*/
	const MeshPtr& Buffer::GetMesh(const char* name) const {
		auto itr = meshList.find(name);
		if (itr == meshList.end()) {
			static const MeshPtr dummy;
			return dummy;
		}
		return itr->second;
	}

	/**
	*�}�e���A�����擾����
	*
	*@param index �}�e���A���C���f�b�N�X
	*
	*@return index �ɑΉ�����}�e���A��
	*/
	const Material& Buffer::GetMaterial(size_t index) const {
		if (index >= materialList.size()) {
			static const Material dummy{ GL_UNSIGNED_BYTE,0,0,0,glm::vec4(1) };
			return dummy;
		}
		return materialList[index];
	}

	/**
	*�o�b�t�@���ێ�����VAO��OpenGL�̏����Ώۂɐݒ肷��
	*/
	void Buffer::BindVAO()const {
		glBindVertexArray(vao);
	}

}