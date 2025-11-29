#include "VertexShader.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string_view>
#include <utils/Log.hpp>
#include <d3d11shader.h>
#include <wrl/client.h>
#include <d3dcommon.h>
#include <vector>
#include <cstdint>
#include <dxgiformat.h>
#include <cstring>
#include <cstddef>
#include <engine/mesh/Vertex.hpp>

void VertexShader::CompileFromFile(ID3D11Device* device, std::wstring_view path, std::string_view entryPoint) {
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HR_LOG(D3DCompileFromFile(path.data(), nullptr, nullptr, entryPoint.data(), "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &m_blob, &errorBlob));
	HR_LOG(device->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_shader));

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
	HR_LOG(D3DReflect(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), IID_PPV_ARGS(&reflector)));

	D3D11_SHADER_DESC shaderDesc;
	reflector->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (uint32_t i = 0; i < shaderDesc.InputParameters; i++) {
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflector->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC element{};
		element.SemanticName = paramDesc.SemanticName;
		element.SemanticIndex = paramDesc.SemanticIndex;
		element.InputSlot = 0;
		element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = 0;

		if      (paramDesc.Mask == 1)   element.Format = DXGI_FORMAT_R32_FLOAT;
		else if (paramDesc.Mask <= 3)   element.Format = DXGI_FORMAT_R32G32_FLOAT;
		else if (paramDesc.Mask <= 7)   element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		else if (paramDesc.Mask <= 15)  element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		uint32_t offset = D3D11_APPEND_ALIGNED_ELEMENT;
		if (std::strcmp(paramDesc.SemanticName, "POSITION") == 0) {
			offset = static_cast<uint32_t>(offsetof(Vertex, pos));
		} else if (std::strcmp(paramDesc.SemanticName, "NORMAL") == 0) {
			offset = static_cast<uint32_t>(offsetof(Vertex, normal));
		} else if (std::strcmp(paramDesc.SemanticName, "TEXCOORD") == 0) {
			offset = static_cast<uint32_t>(offsetof(Vertex, uv));
		} else if (std::strcmp(paramDesc.SemanticName, "COLOR") == 0) {
			offset = static_cast<uint32_t>(offsetof(Vertex, color));
		}

		element.AlignedByteOffset = offset;
		inputLayoutDesc.emplace_back(element);
	}

	HR_LOG(device->CreateInputLayout(inputLayoutDesc.data(), static_cast<uint32_t>(inputLayoutDesc.size()),
		m_blob->GetBufferPointer(), m_blob->GetBufferSize(), &m_inputLayout));
	m_name = path.substr(path.find_last_of('\\') + 1);
}

void VertexShader::Bind(ID3D11DeviceContext* context) {
	context->VSSetShader(m_shader.Get(), nullptr, 0);
	context->IASetInputLayout(m_inputLayout.Get());
}

std::wstring_view VertexShader::Name() const {
	return m_name;
}

ID3D11VertexShader* VertexShader::Get() const {
	return m_shader.Get();
}

ID3D11InputLayout* VertexShader::InputLayout() const {
	return m_inputLayout.Get();
}

ID3DBlob* VertexShader::Blob() const {
	return m_blob.Get();
}
