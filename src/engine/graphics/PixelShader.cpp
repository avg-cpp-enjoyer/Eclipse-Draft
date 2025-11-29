#include "PixelShader.hpp"

#include <d3d11.h>
#include <string_view>
#include <utils/Log.hpp>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <d3dcommon.h>

void PixelShader::CompileFromFile(ID3D11Device* device, std::wstring_view path, std::string_view entryPoint) {
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HR_LOG(D3DCompileFromFile(path.data(), nullptr, nullptr, entryPoint.data(), "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &m_blob, &errorBlob));
	HR_LOG(device->CreatePixelShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_shader));
	m_name = path.substr(path.find_last_of('\\') + 1);
}

void PixelShader::Bind(ID3D11DeviceContext* context) {
	context->PSSetShader(m_shader.Get(), nullptr, 0);
}

std::wstring_view PixelShader::Name() const {
	return m_name;
}

ID3D11PixelShader* PixelShader::Get() const {
	return m_shader.Get();
}

ID3DBlob* PixelShader::Blob() const {
	return m_blob.Get();
}
