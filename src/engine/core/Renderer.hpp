#pragma once

#include "RenderTarget.hpp"

#include <engine/scene/Scene.hpp>
#include <thread>
#include <atomic>
#include <Windows.h>
#include <cstdarg>

class Renderer {
public:
	explicit Renderer(HWND window);
	~Renderer() = default;

	void Start(int threadPriority, uintptr_t affinityMask);
	void Shutdown();
	RenderTarget& GetRT();
protected:
	void RenderLoop();
	void RenderFrame();
protected:
	RenderTarget m_renderTarget;
	std::thread m_renderThread;
	std::atomic<bool> m_running{ false };
	HWND m_window;
	Scene m_scene;
};