#include "plugin.h"
#include "window.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <windows.h>

struct UserData
{
	unsigned long processId;
	HWND windowHandle;
};

BOOL isMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	UserData& data = *(UserData*)lParam;
	unsigned long processId = 0;
	GetWindowThreadProcessId(handle, &processId);
	if (data.processId != processId || !isMainWindow(handle))
		return TRUE;
	data.windowHandle = handle;
	return FALSE;
}

WId findUnityWindow()
{
	UserData data;
	data.processId = GetCurrentProcessId();
	data.windowHandle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return (WId)data.windowHandle;
}

extern "C"
{
	void showQtApp()
	{
		int argc = 0;
		QApplication* app = new QApplication(argc, nullptr);
		WId hwnd = findUnityWindow();
		auto window = new Window(hwnd);
		window->showMaximized();
	}
}