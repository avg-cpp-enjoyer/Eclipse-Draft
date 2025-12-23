#pragma once

#include <DirectXMath.h>

__declspec(align(16)) struct TransformBuffer {
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

__declspec(align(16)) struct LightBuffer {
	DirectX::XMFLOAT3 lightPos;
	float pad1;
	DirectX::XMFLOAT3 lightColor;
	float pad2;
};

__declspec(align(16)) struct GridParams {
	float cellSize;
	float lineWidth;
	float majorStep;
	float majorWidth;
	float baseAlpha;
	DirectX::XMFLOAT3 gridColor;
	DirectX::XMFLOAT3 majorColor;
	DirectX::XMFLOAT3 axisXColor;
	DirectX::XMFLOAT3 axisZColor;
	float axisWidth;
	float fadeDistance;
	float _pad[2];
};

__declspec(align(16)) struct CameraParams {
	DirectX::XMFLOAT3 cameraPos;
	float pad;
};