#pragma once

#include "IShader.hpp"

#include <d3d11.h>
#include <string_view>
#include <d3dcommon.h>
#include <wrl/client.h>

class VertexShader : public IShader {
public:
	VertexShader() = default;
	void CompileFromFile(ID3D11Device* device, std::wstring_view path, std::string_view entryPoint) override;
	void Bind(ID3D11DeviceContext* context) override;
	std::wstring_view Name() const override;
	ID3D11VertexShader* Get() const;
	ID3D11InputLayout* InputLayout() const;
	ID3DBlob* Blob() const;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	std::wstring_view m_name;
};