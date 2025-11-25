#include "MeshLoader.hpp"

Mesh MeshLoader::LoadFromOBJ(const std::string& path, ID3D11Device* device) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open OBJ file: " + path);
	}

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<VertexKey, uint32_t, VertexKeyHasher> uniqueVerts;
	std::vector<DirectX::XMFLOAT3A> positions;
	std::vector<DirectX::XMFLOAT3A> normals;
	std::vector<DirectX::XMFLOAT2A> uvs;
	uint32_t currentSmoothGroup = 0;

	bool hasSmoothGroups = false;

	auto parseFaceToken = [](const std::string& token, uint32_t& vIdx, uint32_t& tIdx, uint32_t& nIdx) {
		vIdx = tIdx = nIdx = 0;
		std::stringstream ss(token);
		std::string part;
		int idx = 0;
		while (std::getline(ss, part, '/') && idx < 3) {
			if (!part.empty()) {
				try {
					if (idx == 0) vIdx = static_cast<uint32_t>(std::stoul(part));
					else if (idx == 1) tIdx = static_cast<uint32_t>(std::stoul(part));
					else if (idx == 2) nIdx = static_cast<uint32_t>(std::stoul(part));
				} catch (...)  {}
			}
			++idx;
		}
	};

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
		} else if (type == "s") {
			hasSmoothGroups = true;
			std::string arg;
			iss >> arg;
			if (arg == "off") currentSmoothGroup = 0;
			else currentSmoothGroup = static_cast<uint32_t>(std::stoul(arg));
		} else if (type == "f") {
			std::vector<std::string> faceVerts;
			std::string vertStr;
			while (iss >> vertStr) {
				faceVerts.push_back(vertStr);
			}

			size_t n = faceVerts.size();
			if (n < 3) {
				continue;
			}

			for (size_t i = 1; i + 1 < n; i++) {
				std::string t0 = faceVerts[0];
				std::string t1 = faceVerts[i];
				std::string t2 = faceVerts[i + 1];

				uint32_t vIdx = 0, tIdx = 0, nIdx = 0;

				parseFaceToken(t0, vIdx, tIdx, nIdx);
				VertexKey key0{};
				key0.pos = positions.at(vIdx - 1);
				key0.norm = (nIdx > 0) ? normals.at(nIdx - 1) : DirectX::XMFLOAT3A{ 0, 0, 0 };
				key0.uv = (tIdx > 0) ? uvs.at(tIdx - 1) : DirectX::XMFLOAT2A{ 0, 0 };
				key0.smoothGroup = currentSmoothGroup;

				auto it0 = uniqueVerts.find(key0);
				uint32_t idx0;
				if (it0 == uniqueVerts.end()) {
					Vertex vert{};
					vert.SetPosition(key0.pos);
					vert.SetNormal(DirectX::XMLoadFloat3A(&key0.norm));
					vert.SetColor(DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f));
					vert.SetUV(DirectX::XMLoadFloat2A(&key0.uv));
					vertices.push_back(vert);
					idx0 = static_cast<uint32_t>(vertices.size() - 1);
					uniqueVerts[key0] = idx0;
				} else {
					idx0 = it0->second;
				}

				parseFaceToken(t1, vIdx, tIdx, nIdx);
				VertexKey key1{};
				key1.pos = positions.at(vIdx - 1);
				key1.norm = (nIdx > 0) ? normals.at(nIdx - 1) : DirectX::XMFLOAT3A{ 0, 0, 0 };
				key1.uv = (tIdx > 0) ? uvs.at(tIdx - 1) : DirectX::XMFLOAT2A{ 0, 0 };
				key1.smoothGroup = currentSmoothGroup;

				auto it1 = uniqueVerts.find(key1);
				uint32_t idx1;
				if (it1 == uniqueVerts.end()) {
					Vertex vert{};
					vert.SetPosition(key1.pos);
					vert.SetNormal(DirectX::XMLoadFloat3A(&key1.norm));
					vert.SetColor(DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f));
					vert.SetUV(DirectX::XMLoadFloat2A(&key1.uv));
					vertices.push_back(vert);
					idx1 = static_cast<uint32_t>(vertices.size() - 1);
					uniqueVerts[key1] = idx1;
				} else {
					idx1 = it1->second;
				}

				parseFaceToken(t2, vIdx, tIdx, nIdx);
				VertexKey key2{};
				key2.pos = positions.at(vIdx - 1);
				key2.norm = (nIdx > 0) ? normals.at(nIdx - 1) : DirectX::XMFLOAT3A{ 0, 0, 0 };
				key2.uv = (tIdx > 0) ? uvs.at(tIdx - 1) : DirectX::XMFLOAT2A{ 0, 0 };
				key2.smoothGroup = currentSmoothGroup;

				auto it2 = uniqueVerts.find(key2);
				uint32_t idx2;
				if (it2 == uniqueVerts.end()) {
					Vertex vert{};
					vert.SetPosition(key2.pos);
					vert.SetNormal(DirectX::XMLoadFloat3A(&key2.norm));
					vert.SetColor(DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f));
					vert.SetUV(DirectX::XMLoadFloat2A(&key2.uv));
					vertices.push_back(vert);
					idx2 = static_cast<uint32_t>(vertices.size() - 1);
					uniqueVerts[key2] = idx2;
				} else {
					idx2 = it2->second;
				}

				indices.push_back(idx0);
				indices.push_back(idx1);
				indices.push_back(idx2);
			}
		}
	}

	if (hasSmoothGroups) {
		std::vector<DirectX::XMVECTOR> accum(vertices.size(), DirectX::XMVectorZero());

		for (size_t i = 0; i < indices.size(); i += 3) {
			uint32_t i0 = indices[i];
			uint32_t i1 = indices[i + 1];
			uint32_t i2 = indices[i + 2];

			DirectX::XMVECTOR p0 = vertices[i0].PositionVec();
			DirectX::XMVECTOR p1 = vertices[i1].PositionVec();
			DirectX::XMVECTOR p2 = vertices[i2].PositionVec();

			DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(p1, p0);
			DirectX::XMVECTOR e2 = DirectX::XMVectorSubtract(p2, p0);
			DirectX::XMVECTOR faceNormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e1, e2));

			accum[i0] = DirectX::XMVectorAdd(accum[i0], faceNormal);
			accum[i1] = DirectX::XMVectorAdd(accum[i1], faceNormal);
			accum[i2] = DirectX::XMVectorAdd(accum[i2], faceNormal);
		}

		for (size_t i = 0; i < vertices.size(); ++i) {
			DirectX::XMVECTOR n = DirectX::XMVector3Normalize(accum[i]);
			vertices[i].SetNormal(n);
		}
	}

	Mesh mesh;
	mesh.SetVertices(vertices);
	mesh.SetIndices(indices);
	mesh.CreateBuffers(device);
	return mesh;
}