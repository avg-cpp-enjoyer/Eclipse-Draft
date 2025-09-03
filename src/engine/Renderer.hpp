#pragma once

#include "RenderTarget.hpp"
#include "Mesh.hpp"
#include "Grid.hpp"
#include "MeshLoader.hpp"

class __declspec(novtable) Renderer {
public:
	explicit Renderer(HWND window);
	virtual ~Renderer() = default;

	void Start(int threadPriority, uintptr_t affinityMask);
	void Shutdown();
	void HandleResize();
	RenderTarget& GetRT();
protected:
	void RenderLoop();
	virtual void RenderFrame() = 0;
protected:
	RenderTarget m_renderTarget;
	std::thread m_renderThread;
	std::atomic_bool m_running{ false };
	HWND m_window;
};

class MainRenderer : public Renderer {
public:
	explicit MainRenderer(HWND window);
	~MainRenderer() override = default;
private:
	void RenderFrame() override;
	Mesh m_testMesh;
	Grid m_grid;
};