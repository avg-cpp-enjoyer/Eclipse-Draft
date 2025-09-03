#pragma once

#include <Windows.h>
#include <cstdint>
#include <type_traits>

template <typename T>
class __declspec(novtable) Win32Base {
public:
	Win32Base() : m_window(nullptr) {}
	virtual ~Win32Base() noexcept = default;

	Win32Base(const Win32Base&) = delete;
	Win32Base& operator=(const Win32Base&) = delete;

	bool Create(const wchar_t* windowName, uint32_t style, uint32_t exStyle = 0,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT, HBRUSH bgBrush = nullptr, HWND parentWnd = nullptr,
		HMENU menu = nullptr);

	static intptr_t _stdcall WndProc(HWND window, uint32_t msg, uintptr_t wParam, intptr_t lParam);
	HWND Window() const noexcept;
protected:
	virtual const wchar_t* ClassName() const = 0;
	virtual intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) = 0;
protected:
	HWND m_window;
};

template <typename T>
inline bool Win32Base<T>::Create(const wchar_t* windowName, uint32_t style, uint32_t exStyle,
	int x, int y, int width, int height, HBRUSH bgBrush, HWND parentWnd, HMENU menu) {
	WNDCLASSEX wclassex{};

	wclassex.cbSize         = sizeof(WNDCLASSEX);
	wclassex.style          = CS_HREDRAW | CS_VREDRAW;
	wclassex.lpfnWndProc    = Win32Base<T>::WndProc;
	wclassex.hInstance      = GetModuleHandle(nullptr);
	wclassex.lpszClassName  = ClassName();
	wclassex.hbrBackground  = bgBrush;
	wclassex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wclassex.hIcon          = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wclassex) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
		return false;
	}
	m_window = CreateWindowEx(exStyle, ClassName(), windowName, style, x, y,
		width, height, parentWnd, menu, GetModuleHandle(nullptr), this);

	return m_window != nullptr;
}


template <typename T>
inline intptr_t Win32Base<T>::WndProc(HWND window, uint32_t msg, uintptr_t wParam, intptr_t lParam) {
	T* self = nullptr;

	if (msg == WM_NCCREATE) {
		auto* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		self = static_cast<T*>(create->lpCreateParams);
		SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<intptr_t>(self));
		self->m_window = window;
	} else {
		self = reinterpret_cast<T*>(GetWindowLongPtr(window, GWLP_USERDATA));
	}

	if (self) {
		return self->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProc(window, msg, wParam, lParam);
}

template <typename T>
inline HWND Win32Base<T>::Window() const noexcept {
	return m_window;
}