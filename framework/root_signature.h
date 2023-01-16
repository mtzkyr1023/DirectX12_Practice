#ifndef _ROOT_SIGNATURE_H_
#define _ROOT_SIGNATURE_H_

#include <d3d12.h>

#include <wrl/client.h>

#include <unordered_map>
#include <vector>

class RootSignature {
public:
	RootSignature() :
		m_descriptorTableId(0)
	{}

	~RootSignature() = default;

	bool create(ID3D12Device* device, D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlag);

	void addDescriptorCount(D3D12_SHADER_VISIBILITY shaderVisiblity, D3D12_DESCRIPTOR_RANGE_TYPE descType, UINT baseShaderRegister, UINT count, D3D12_ROOT_PARAMETER_TYPE type = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);

	ID3D12RootSignature* getRootSignature() { return m_rootSignature.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

	std::vector<D3D12_DESCRIPTOR_RANGE> m_range;
	std::vector<D3D12_SHADER_VISIBILITY> m_shaderVisiblity;
	std::vector<D3D12_ROOT_PARAMETER_TYPE> m_rootParameterType;

	UINT m_descriptorTableId;
};


#endif