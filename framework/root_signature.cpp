#include "root_signature.h"

bool RootSignature::create(ID3D12Device* device, D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlag) {
	HRESULT res;
	std::vector<D3D12_ROOT_PARAMETER> rootParam;

	rootParam.reserve(m_range.size());
	for(size_t i = 0; i < m_range.size(); i++) {
		D3D12_ROOT_PARAMETER param{};
		param.ParameterType = m_rootParameterType[i];
		param.ShaderVisibility = m_shaderVisiblity[i];
		param.DescriptorTable.NumDescriptorRanges = 1;
		param.DescriptorTable.pDescriptorRanges = &m_range[i];

		rootParam.push_back(param);

		int a = 0;
	}

	D3D12_ROOT_SIGNATURE_DESC rsDesc{};
	rsDesc.NumParameters = (UINT)rootParam.size();
	rsDesc.pParameters = rootParam.data();
	rsDesc.Flags = rootSignatureFlag;

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;

	res = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.ReleaseAndGetAddressOf(),
		error.ReleaseAndGetAddressOf());
	if (FAILED(res)) {
		OutputDebugString((const char*)error->GetBufferPointer());
		return false;
	}

	res = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(res)) {
		MessageBox(NULL, "failed creating root signature.", "Error", MB_OK);
		return false;
	}

	return true;
}

void RootSignature::addDescriptorCount(D3D12_SHADER_VISIBILITY shaderVisiblity, D3D12_DESCRIPTOR_RANGE_TYPE descType, UINT baseShaderRegister, UINT count, D3D12_ROOT_PARAMETER_TYPE type) {
	D3D12_DESCRIPTOR_RANGE descRange{};
	descRange.RangeType = descType;
	descRange.NumDescriptors = count;
	descRange.BaseShaderRegister = baseShaderRegister;
	descRange.RegisterSpace = 0;
	descRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	m_range.push_back(descRange);
	m_shaderVisiblity.push_back(shaderVisiblity);

	m_rootParameterType.push_back(type);
}