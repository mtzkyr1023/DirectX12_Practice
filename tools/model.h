#ifndef _MODEL_H_
#define _MODEL_H_

#include "../framework/device.h"

#include "../glm-master/glm/glm.hpp"
#include "../glm-master/glm/gtc/matrix_transform.hpp"
#include "../glm-master/glm/gtc/quaternion.hpp"

#include <vector>
#include <memory>

class Node;

class Model {
public:
	Model();
	~Model();

	bool create(ID3D12Device* device, ID3D12CommandQueue* queue, const char* foldername, const char* filename);

	int vertexBuffer() { return m_vertexBuffer; }
	int indexBuffer() { return m_indexBuffer; }

	int allIndexCount() { return m_allIndexCount; }

	int vertexCount(int index) { return m_vertexCount[index]; }
	int indexCount(int index) { return m_indexCount[index]; }

	int meshCount() { return m_meshCount; }
	int materialCount() { return m_materialCount; }

	int materialIndex(int index) { return m_materialIndex[index]; }

	int albedoIndex(int index) { return index < m_albedoIndex.size() ? m_albedoIndex[index] : 0; }
	int normalIndex(int index) { return index < m_normalIndex.size() ? m_normalIndex[index] : 0; }
	int roughMetalIndex(int index) { return index < m_roughMetalIndex.size() ? m_roughMetalIndex[index] : 0; }

private:
	int m_vertexBuffer;
	int m_indexBuffer;

	int m_allIndexCount;

	int m_meshCount;
	int m_materialCount;


	std::vector<int> m_materialIndex;

	std::vector<int> m_vertexCount;
	std::vector<int> m_indexCount;
	std::vector<int> m_albedoIndex;
	std::vector<int> m_normalIndex;
	std::vector<int> m_roughMetalIndex;

	std::vector<std::shared_ptr<Node>> m_nodes;
};

class Node {
public:
	Node();
	~Node();

	void dirtyUpdate();
	void setParent(Node* parent) { m_parent = parent; }

	glm::vec3& localPosition() { dirtyUpdate(); return m_localPosition; }
	glm::quat& localRotation() { dirtyUpdate(); return m_localRotation; }

	glm::vec3& position() { dirtyUpdate(); return m_position; }
	glm::quat& rotation() { dirtyUpdate(); return m_rotation; }

	glm::mat4& localMatrix() { dirtyUpdate(); return m_localMatrix; }
	glm::mat4& offsetMatrix() { dirtyUpdate(); return m_offsetMatrix; }
	glm::mat4& worldMatrix() { dirtyUpdate(); return m_worldMatrix; }

	int childCount() { return m_children.size(); }
	std::vector<std::shared_ptr<Node>>& children() { return m_children; }

	void setDirty() { m_isDirty = true; for (auto& ite : m_children) ite->setDirty(); }

private:
	glm::mat4 m_localMatrix;
	glm::mat4 m_offsetMatrix;
	glm::mat4 m_worldMatrix;
	glm::vec3 m_localPosition;
	glm::quat m_localRotation;
	glm::vec3 m_position;
	glm::quat m_rotation;
	Node* m_parent;
	std::vector<std::shared_ptr<Node>> m_children;

	bool m_isDirty;
};

#endif