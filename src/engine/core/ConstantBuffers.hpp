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
	float baseAlpha;
	float padding1;
	DirectX::XMFLOAT3 gridColor;
	float padding2;
	DirectX::XMFLOAT3 axisXColor;
	float padding3;
	DirectX::XMFLOAT3 axisZColor;
	float padding4;
	float axisWidth;
	float fadeDistance;
	float padding5[2];
};

__declspec(align(16)) struct CameraParams {
	DirectX::XMFLOAT3 cameraPos;
	float pad;
};