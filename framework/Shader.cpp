#include "shader.h"


bool Shader::createVertexShader(const wchar_t* filename) {
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	//if(FAILED(hr)) Handle error...

	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	//if(FAILED(hr)) Handle error...

	UINT codePage = CP_UTF8;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	hr = library->CreateBlobFromFile(filename, &codePage, &sourceBlob);
	//if(FAILED(hr)) Handle file loading error...

	LPCWSTR args[] = {
		L"-Vd",
	};

	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	hr = compiler->Compile(
		sourceBlob.Get(), // pSource
		filename, // pSourceName
		L"main", // pEntryPoint
		L"vs_6_0", // pTargetProfile
		args, 0, // pArguments, argCount
		NULL, 0, // pDefines, defineCount
		NULL, // pIncludeHandler
		&result); // ppResult
	if (SUCCEEDED(hr))
		result->GetStatus(&hr);
	if (FAILED(hr))
	{
		if (result)
		{
			Microsoft::WRL::ComPtr<IDxcBlobEncoding> errorsBlob;
			hr = result->GetErrorBuffer(&errorsBlob);
			if (SUCCEEDED(hr) && errorsBlob)
			{
				OutputDebugString((const char*)errorsBlob->GetBufferPointer());
			}
		}
		// Handle compilation error...
	}

	result->GetResult(m_bytecode.GetAddressOf());

	if (FAILED(hr))
		return false;

	return true;
}

bool Shader::createGeometoryShader(const wchar_t* filename) {
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	//if(FAILED(hr)) Handle error...

	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	//if(FAILED(hr)) Handle error...

	UINT codePage = CP_UTF8;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	hr = library->CreateBlobFromFile(filename, &codePage, &sourceBlob);
	//if(FAILED(hr)) Handle file loading error...

	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	hr = compiler->Compile(
		sourceBlob.Get(), // pSource
		filename, // pSourceName
		L"main", // pEntryPoint
		L"gs_6_0", // pTargetProfile
		NULL, 0, // pArguments, argCount
		NULL, 0, // pDefines, defineCount
		NULL, // pIncludeHandler
		&result); // ppResult
	if (SUCCEEDED(hr))
		result->GetStatus(&hr);
	if (FAILED(hr))
	{
		if (result)
		{
			Microsoft::WRL::ComPtr<IDxcBlobEncoding> errorsBlob;
			hr = result->GetErrorBuffer(&errorsBlob);
			if (SUCCEEDED(hr) && errorsBlob)
			{
				OutputDebugString((const char*)errorsBlob->GetBufferPointer());
			}
		}
		// Handle compilation error...
	}

	result->GetResult(m_bytecode.GetAddressOf());

	if (FAILED(hr))
		return false;

	return true;
}

bool Shader::createPixelShader(const wchar_t* filename) {
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	//if(FAILED(hr)) Handle error...

	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	//if(FAILED(hr)) Handle error...

	UINT codePage = CP_UTF8;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	hr = library->CreateBlobFromFile(filename, &codePage, &sourceBlob);
	//if(FAILED(hr)) Handle file loading error...

	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	hr = compiler->Compile(
		sourceBlob.Get(), // pSource
		filename, // pSourceName
		L"main", // pEntryPoint
		L"ps_6_0", // pTargetProfile
		NULL, 0, // pArguments, argCount
		NULL, 0, // pDefines, defineCount
		NULL, // pIncludeHandler
		&result); // ppResult
	if (SUCCEEDED(hr))
		result->GetStatus(&hr);
	if (FAILED(hr))
	{
		if (result)
		{
			Microsoft::WRL::ComPtr<IDxcBlobEncoding> errorsBlob;
			hr = result->GetErrorBuffer(&errorsBlob);
			if (SUCCEEDED(hr) && errorsBlob)
			{
				OutputDebugString((const char*)errorsBlob->GetBufferPointer());
			}
		}
		// Handle compilation error...
	}

	result->GetResult(m_bytecode.GetAddressOf());

	if (FAILED(hr))
		return false;

	return true;
}

bool Shader::createComputeShader(const wchar_t* filename) {
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	//if(FAILED(hr)) Handle error...

	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	//if(FAILED(hr)) Handle error...

	UINT codePage = CP_UTF8;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	hr = library->CreateBlobFromFile(filename, &codePage, &sourceBlob);
	//if(FAILED(hr)) Handle file loading error...

	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	hr = compiler->Compile(
		sourceBlob.Get(), // pSource
		filename, // pSourceName
		L"main", // pEntryPoint
		L"cs_6_0", // pTargetProfile
		NULL, 0, // pArguments, argCount
		NULL, 0, // pDefines, defineCount
		NULL, // pIncludeHandler
		&result); // ppResult
	if (SUCCEEDED(hr))
		result->GetStatus(&hr);
	if (FAILED(hr))
	{
		if (result)
		{
			Microsoft::WRL::ComPtr<IDxcBlobEncoding> errorsBlob;
			hr = result->GetErrorBuffer(&errorsBlob);
			if (SUCCEEDED(hr) && errorsBlob)
			{
				OutputDebugString((const char*)errorsBlob->GetBufferPointer());
			}
		}
		// Handle compilation error...
	}

	result->GetResult(m_bytecode.GetAddressOf());

	if (FAILED(hr))
		return false;

	return true;
}