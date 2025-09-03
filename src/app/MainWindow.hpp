#pragma once

#include "Win32Base.hpp"
#include "engine/Renderer.hpp"

#include <windowsx.h>

class MainWindow : public Win32Base<MainWindow> {
public:
	MainWindow() = default;
	~MainWindow() = default;

	const wchar_t* ClassName() const override;
	intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) override;
private:
	void OnCreate();
	void OnDestroy();
	void OnSizeChanged();
private:
	std::unique_ptr<MainRenderer> m_renderer;
	bool m_pendingResize = false;
};