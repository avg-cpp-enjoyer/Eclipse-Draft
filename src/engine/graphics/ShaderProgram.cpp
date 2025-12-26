#include "ShaderProgram.hpp"
#include "IShader.hpp"

#include <memory>
#include <utility>

void ShaderProgram::AddShader(std::unique_ptr<IShader> shader) {
	m_shaders[shader->Name()] = std::move(shader);
}
