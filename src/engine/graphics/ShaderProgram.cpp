#include "ShaderProgram.hpp"
#include "IShader.hpp"

#include <memory>
#include <utility>

void ShaderProgram::AddShader(std::unique_ptr<IShader> shader) {
	GetInstance().m_shaders[shader->Name()] = std::move(shader);
}

ShaderProgram& ShaderProgram::GetInstance() {
	static ShaderProgram instance;
	return instance;
}
