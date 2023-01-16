#ifndef _SHADER_H_
#define _SHADER_H_

#pragma comment(lib, "dxcompiler.lib")

#include <d3d12.h>
#include <dxcapi.h>

#include <wrl/client.h>

#include <memory>

class Shader {
public:
	Shader() = default;
	~Shader() = default;

	bool createVertexShader(const wchar_t* filename);
	bool createPixelShader(const wchar_t* filename);
	bool createGeometoryShader(const wchar_t* filename);
	bool createComputeShader(const wchar_t* filename);

	IDxcBlob* getByteCode() { return m_bytecode.Get(); }

private:
	Microsoft::WRL::ComPtr<IDxcBlob> m_bytecode;
};


using ShaderSp = std::shared_ptr<Shader>;

#endif