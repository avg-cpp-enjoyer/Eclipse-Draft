#include "Application.hpp"

int __stdcall wWinMain([[maybe_unused]] _In_ HINSTANCE instance, _In_opt_ HINSTANCE, [[maybe_unused]] _In_ wchar_t* cmdLine, _In_ int cmdShow) {
	Application app;
	app.Initialize(800, 600);
	return app.Exec(cmdShow);
}