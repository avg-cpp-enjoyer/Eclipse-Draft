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
	inline static std::unordered_map<std::wstring_view, std::unique_ptr<IShader>> m_shaders;
};

template <typename T> requires std::is_base_of_v<IShader, T>
T* ShaderProgram::GetShaderByName(std::wstring_view shaderName) {
	auto it = m_shaders.find(shaderName);
	return it != m_shaders.end() ? dynamic_cast<T*>(it->second.get()) : nullptr;
}