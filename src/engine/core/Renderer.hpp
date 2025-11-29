#pragma once

#include "RenderTarget.hpp"
#include <engine/mesh/Mesh.hpp>
#include <engine/scene/Grid.hpp>
#include <engine/mesh/MeshLoader.hpp>

class Renderer {
public:
	explicit Renderer(HWND window);
	virtual ~Renderer() = default;

	void Start(int threadPriority, uintptr_t affinityMask);
	void Shutdown();
	RenderTarget& GetRT();
protected:
	void RenderLoop();
	void RenderFrame();
protected:
	RenderTarget m_renderTarget;
	std::thread m_renderThread;
	std::atomic_bool m_running{ false };
	HWND m_window;
	Mesh m_testMesh;
	Grid m_grid;
};