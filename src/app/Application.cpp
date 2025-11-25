#include "Application.hpp"
#include <engine/GraphicsDevice.hpp>
#include <thread>

int Application::Initialize(int width, int height) {
	if (!m_mainWindow.Create(L"Eclipse Draft", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		0, CW_USEDEFAULT, CW_USEDEFAULT, width, height)) {
		return -1;
	}
	return 0;
}

int Application::Exec(int cmdShow) {
	ShowWindow(m_mainWindow.Window(), cmdShow);

	MSG msg{};
	while (msg.message != WM_QUIT) {
		GraphicsDevice::ExecuteRenderJobs();
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return static_cast<int>(msg.wParam);
}
