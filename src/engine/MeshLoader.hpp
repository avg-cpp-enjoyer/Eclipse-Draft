#pragma once

#include "Mesh.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

struct Face {
	std::vector<size_t> triOrder;
	size_t numTris;
};

struct VertexKey {
	DirectX::XMFLOAT3A pos;
	DirectX::XMFLOAT3A norm;
	DirectX::XMFLOAT2A uv;

	bool operator==(const VertexKey& other) const {
		return memcmp(this, &other, sizeof(VertexKey)) == 0;
	}
};

struct VertexKeyHasher {
	size_t operator()(const VertexKey& v) const noexcept {
		size_t h1 = std::hash<float>{}(v.pos.x)  ^ (std::hash<float>{}(v.pos.y) << 1)  ^ (std::hash<float>{}(v.pos.z) << 2);
		size_t h2 = std::hash<float>{}(v.norm.x) ^ (std::hash<float>{}(v.norm.y) << 1) ^ (std::hash<float>{}(v.norm.z) << 2);
		size_t h3 = std::hash<float>{}(v.uv.x)   ^ (std::hash<float>{}(v.uv.y) << 1);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};

class MeshLoader {
public:
	static Mesh LoadFromOBJ(const std::string& path, ID3D11Device* device);
};