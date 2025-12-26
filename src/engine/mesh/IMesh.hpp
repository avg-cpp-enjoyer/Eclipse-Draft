#pragma once

#include <engine/scene/Camera.hpp>
#include <Windows.h>
#include <d3d11.h>

interface __declspec(novtable) IMesh {
	virtual ~IMesh() = default;
	virtual void CreateBuffers(ID3D11Device* device) = 0;
	virtual void Draw(ID3D11DeviceContext* context, const Camera& camera) const = 0;
	virtual void Update(float deltaTime) = 0;
};