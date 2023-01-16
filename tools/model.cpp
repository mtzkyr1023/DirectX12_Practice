#include "model.h"

#include "../glm-master/glm/glm.hpp"
#include "../glm-master/glm/gtc/matrix_transform.hpp"
#include "../glm-master/glm/gtc/quaternion.hpp"

#include "../resource_manager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

Model::Model() {

}

Model::~Model() {

}

bool Model::create(ID3D12Device* device, ID3D12CommandQueue* queue, const char* foldername, const char* filename) {

    auto& resMgr = ResourceManager::Instance();
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 nor;
        glm::vec3 tan;
        glm::vec2 tex;
    };

	Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(std::string(filename),
        aiPostProcessSteps::aiProcess_CalcTangentSpace |
        aiPostProcessSteps::aiProcess_Triangulate
    );

    std::unordered_map<aiMesh*, int> meshIdMap;

    if (scene->HasMeshes()) {
        m_meshCount = scene->mNumMeshes;

        std::vector<Vertex> vertices;
        std::vector<int> indices;

        int indexOffset = 0;

        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            for (int j = 0; j < mesh->mNumVertices; j++) {
                Vertex vertex;
                vertex.pos = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
                vertex.nor = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
                vertex.tan = glm::vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
                vertex.tex = glm::vec2(mesh->mTextureCoords[0][j].x, 1.0f - mesh->mTextureCoords[0][j].y);
                vertices.push_back(vertex);
            }

            for (int j = 0; j < mesh->mNumFaces; j++) {
                aiFace& face = mesh->mFaces[j];
                for(int k = 0; k < face.mNumIndices; k++) {
                    indices.push_back(face.mIndices[k]);
                }
            }

            indexOffset += mesh->mNumVertices;

            meshIdMap[mesh] = i;

            m_materialIndex.push_back(mesh->mMaterialIndex);
            m_indexCount.push_back(mesh->mNumFaces * 3);
            m_vertexCount.push_back(mesh->mNumVertices);
        }


        m_vertexBuffer = resMgr.createVertexBuffer(device, queue, 1, sizeof(Vertex), sizeof(Vertex) * vertices.size(), vertices.data());
        m_indexBuffer = resMgr.createIndexBuffer(device, queue, 1, sizeof(uint32_t), sizeof(uint32_t) * indices.size(), indices.data());

        m_allIndexCount = indices.size();


        m_nodes.resize(scene->mNumMeshes);
        for (auto& ite : m_nodes) {
            ite = std::make_shared<Node>();
        }
    }

    if (scene->hasSkeletons()) {
        for (int i = 0; i < scene->mNumSkeletons; i++) {
            aiSkeleton* skeleton = scene->mSkeletons[i];
            for (int j = 0; j < skeleton->mNumBones; j++) {
                aiSkeletonBone* bone = skeleton->mBones[j];
                if (!m_nodes[meshIdMap[bone->mMeshId]]) {
                    m_nodes[meshIdMap[bone->mMeshId]] = std::make_shared<Node>();
                }

                m_nodes[meshIdMap[bone->mMeshId]]->setParent(m_nodes[bone->mParent].get());
                m_nodes[bone->mParent]->children().push_back(m_nodes[meshIdMap[bone->mMeshId]]);
            }
        }
    }

    if (scene->HasMaterials()) {
        m_albedoIndex.resize(scene->mNumMaterials);
        m_normalIndex.resize(scene->mNumMaterials);
        m_roughMetalIndex.resize(scene->mNumMaterials);
        for (int i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* material = scene->mMaterials[i];
            for (int j = 0; j < material->GetTextureCount(aiTextureType_BASE_COLOR); j++) {
                aiString path;
                material->GetTexture(aiTextureType_BASE_COLOR, j, &path);

                std::string fullpath = foldername;
                fullpath += path.C_Str();

                m_albedoIndex[i] = (resMgr.createTexture(device, queue, 1, DXGI_FORMAT_R8G8B8A8_UNORM, fullpath.c_str(), true));
            }
            for (int j = 0; j < material->GetTextureCount(aiTextureType_NORMALS); j++) {
                aiString path;
                material->GetTexture(aiTextureType_NORMALS, j, &path);

                std::string fullpath = foldername;
                fullpath += path.C_Str();

                m_normalIndex[i] = (resMgr.createTexture(device, queue, 1, DXGI_FORMAT_R8G8B8A8_UNORM, fullpath.c_str(), true));
            }
            for (int j = 0; j < material->GetTextureCount(aiTextureType_METALNESS); j++) {
                aiString path;
                material->GetTexture(aiTextureType_METALNESS, j, &path);

                std::string fullpath = foldername;
                fullpath += path.C_Str();

                m_roughMetalIndex[i] = (resMgr.createTexture(device, queue, 1, DXGI_FORMAT_R8G8B8A8_UNORM, fullpath.c_str(), true));
            }
        }
    }

    return true;
}


Node::Node() : m_parent(nullptr), m_isDirty(true) {

}

Node::~Node() {

}

void Node::dirtyUpdate() {
    if (!m_isDirty) return;

    m_localMatrix = glm::mat4(m_localRotation) * glm::translate(glm::identity<glm::mat4>(), m_localPosition);

    if (m_parent != nullptr) {
        m_worldMatrix = m_parent->worldMatrix() * m_localMatrix;
    }
    else {
        m_worldMatrix = m_localMatrix;
    }

    m_position = glm::vec3(m_worldMatrix[3][0], m_worldMatrix[3][1], m_worldMatrix[3][2]);
    m_rotation = glm::quat(m_worldMatrix);
}