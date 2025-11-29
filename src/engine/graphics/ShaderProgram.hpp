#pragma once

#include "IShader.hpp"

#include <unordered_map>
#include <string_view>
#include <memory>
#include <type_traits>

class ShaderProgram {
public:
	static void AddShader(std::unique_ptr<IShader> shader);
	template <typename T> requires std::is_base_of_v<IShader, T>
	static T* GetShaderByName(std::wstring_view shaderName);
private:
	ShaderProgram() = default;
	~ShaderProgram() = default;
	static ShaderProgram& GetInstance();
private:
	std::unordered_map<std::wstring_view, std::unique_ptr<IShader>> m_shaders;
};

template <typename T> requires std::is_base_of_v<IShader, T>
T* ShaderProgram::GetShaderByName(std::wstring_view shaderName) {
	auto& shaders = GetInstance().m_shaders;
	auto it = shaders.find(shaderName);
	return it != shaders.end() ? dynamic_cast<T*>(it->second.get()) : nullptr;
}