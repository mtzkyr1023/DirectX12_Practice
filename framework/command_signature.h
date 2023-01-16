#ifndef _COMMAND_SIGNATURE_H_
#define _COMMAND_SIGNATURE_H_

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>

class CommandSignature {
public:
	CommandSignature() = default;
	~CommandSignature() = default;

	void addConstantBuffer(UINT roodParameterIndex);
	void addShaderResource(UINT rootParameterIndex);
	void addUnorderedAccess(UINT rootParamterIndex);
	void addVertexBuffer(UINT slotIndex);
	void addDrawCommand();
	void addDrawIndexedCommand();
	void addDispatchCommand();

	bool createDrawIndirect(ID3D12Device* device, ID3D12RootSignature* rootSignature);
	bool createDispatchIndirect(ID3D12Device* device, ID3D12RootSignature* rootSignature);

	ID3D12CommandSignature* getCommandSignatue() { return m_commandSignature.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_commandSignature;

	std::vector<D3D12_INDIRECT_ARGUMENT_DESC> m_argDescArray;
};

#endif