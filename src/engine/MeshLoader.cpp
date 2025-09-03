#include "MeshLoader.hpp"

Mesh MeshLoader::LoadFromOBJ(const std::string& path, ID3D11Device* device) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open OBJ file: " + path);
	}

	std::vector<DirectX::XMFLOAT3A> positions;
	std::vector<DirectX::XMFLOAT3A> normals;
	std::vector<DirectX::XMFLOAT2A> uvs;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<VertexKey, uint32_t, VertexKeyHasher> uniqueVerts;
	
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#') {
			continue;
		}

		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v") {
			DirectX::XMFLOAT3A pos{};
			iss >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
		} else if (type == "vt") {
			DirectX::XMFLOAT2A uv{};
			iss >> uv.x >> uv.y;
			uvs.push_back(uv);
		} else if (type == "vn") {
			DirectX::XMFLOAT3A norm{};
			iss >> norm.x >> norm.y >> norm.z;
			normals.push_back(norm);
		} else if (type == "f") {
			std::vector<std::string> faceVerts;
			std::string vertStr;
			while (iss >> vertStr) {
				faceVerts.push_back(vertStr);
			}

			Face current = [&faceVerts]() -> Face {
				Face result;

				result.numTris = 3 * faceVerts.size() - 6;
				result.triOrder.reserve(result.numTris);

				size_t vert1 = 1, vert2 = 2;
				for (size_t i = 0; i < result.numTris; i++) {
					switch ((i + 3) % 3) {
					case 0: result.triOrder.push_back(0); break;
					case 1: result.triOrder.push_back(vert1++); break;
					case 2: result.triOrder.push_back(vert2++); break;
					}
				}

				return result;
			}();

			for (int t = 0; t < current.numTris; t++) {
				int idx = current.triOrder.at(t);

				uint32_t vIdx = 0, tIdx = 0, nIdx = 0;
				sscanf_s(faceVerts.at(idx).c_str(), "%d/%d/%d", &vIdx, &tIdx, &nIdx);

				VertexKey key{};
				key.pos = positions.at(vIdx - 1);
				key.norm = (nIdx > 0) ? normals.at(nIdx - 1) : DirectX::XMFLOAT3A{ 0, 0, 0 };
				key.uv = (tIdx > 0) ? uvs.at(tIdx - 1) : DirectX::XMFLOAT2A{ 0, 0 };

				auto it = uniqueVerts.find(key);
				if (it == uniqueVerts.end()) {
					Vertex vert{};
					vert.SetPosition(key.pos);
					vert.SetNormal(DirectX::XMLoadFloat3A(&key.norm));
					vert.SetColor(DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f));
					vert.SetUV(DirectX::XMLoadFloat2A(&key.uv));
					vertices.push_back(vert);

					uint32_t newIndex = static_cast<uint32_t>(vertices.size() - 1);
					uniqueVerts[key] = newIndex;
					indices.push_back(newIndex);
				} else {
					indices.push_back(it->second);
				}
			}
		}
	}

	Mesh mesh;
	mesh.SetVertices(vertices);
	mesh.SetIndices(indices);
	mesh.CreateBuffers(device);
	return mesh;
}