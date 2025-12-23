#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <string_view>

interface __declspec(novtable) IShader {
	virtual ~IShader() = default;
	virtual void CompileFromFile(ID3D11Device* device, std::wstring_view path, std::string_view entryPoint) = 0;
	virtual void Bind(ID3D11DeviceContext* context) = 0;
	virtual std::wstring_view Name() const = 0;
};