#pragma once

#include <windows.h>
#include <QtWidgets/QMainWindow>

class Window : public QMainWindow
{
	Q_OBJECT

public:
	Window(WId hwnd);
	bool event(QEvent* event);
	bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
	void setUnityFocusFlag(bool flag) { m_focusInUnity = flag; }
	void setUnityFocus(bool focus);
private:
	void hookUnityProc();
private:
	HWND m_unityHandle;
	bool m_focusInUnity = true;
};