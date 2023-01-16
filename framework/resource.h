#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <d3d12.h>

#include <wrl/client.h>
#include <vector>
#include <string>

enum class ResourceType {
	kVertexBuffer,
	kIndexBuffer,
	kConstanceBuffer,
	kStrucuredBuffer,
	kByteAddressBuffer,
	kTexture,
};

class Resource {
public:
	Resource() {}
	Resource(const char* name) :m_name(name) {}
	virtual ~Resource() = default;

	ID3D12Resource* getResource(UINT num) { return m_resource[num].Get(); }

	ResourceType GetResourceType() { return m_resourceType; }

	int getResourceCount() { return (int)m_resource.size(); }

	const char* getName() { return m_name.c_str(); }


	virtual void transitionResource(ID3D12GraphicsCommandList* command, UINT textureNum, D3D12_RESOURCE_BARRIER_FLAGS flag,
		D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {}

	void setId(int id) { m_id = id; }
	int getId() { return m_id; }

protected:
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_resource;

	ResourceType m_resourceType;

	std::string m_name;

	int m_id;
};

#endif