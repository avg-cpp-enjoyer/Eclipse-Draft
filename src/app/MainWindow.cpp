#include "MainWindow.hpp"

#include <windowsx.h>
#include <engine/core/GraphicsDevice.hpp>

const wchar_t* MainWindow::ClassName() const {
	return L"EclipseMainWindow";
}

intptr_t MainWindow::HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) {
	switch (msg) {
	case WM_EXITSIZEMOVE:
		OnSizeChanged();
		break;
	case WM_CREATE:
		OnCreate();
		break;
	case WM_MBUTTONDOWN: 
		m_renderer->GetRT().GetCamera().OnMouseButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SetCapture(m_window);
		break;
	case WM_MBUTTONUP:   
		m_renderer->GetRT().GetCamera().OnMouseButtonUp();
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:   
		m_renderer->GetRT().GetCamera().OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEWHEEL:  
		m_renderer->GetRT().GetCamera().OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(m_window, msg, wParam, lParam);
}

void MainWindow::OnCreate() {
	GraphicsDevice::Initialize();
	m_renderer.emplace(m_window);
	m_renderer->Start(THREAD_PRIORITY_ABOVE_NORMAL, 1ULL << 1);
}

void MainWindow::OnDestroy() {
	m_renderer->Shutdown();
}

void MainWindow::OnSizeChanged() {
	m_renderer->GetRT().HandleResize();
}
