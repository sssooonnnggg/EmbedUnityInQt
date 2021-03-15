#include "window.h"
#include <windows.h>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QResizeEvent>
#include <QWindow>
#include <QTimer>
#include <QtDebug>
#include <QMetaEnum>

static Window* WindowInstance = nullptr;
WNDPROC OriginalUnityProc = nullptr;
LRESULT CALLBACK HookedUnityProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// before Unity process click events, we set focus to Unity window
	if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN)
	{
		if (WindowInstance)
		{
			WindowInstance->setUnityFocusFlag(true);
			WindowInstance->setUnityFocus(true);
		}
	}
	
	return OriginalUnityProc(hWnd, message, wParam, lParam);
}

Window::Window(WId hwnd)
	: QMainWindow()
{
	WindowInstance = this;

	// add toolbar
	QToolBar* bar = addToolBar("toolbar");
	for (int i = 0; i < 10; ++i)
		bar->addAction(QString("Button %1").arg(i));

	auto addDock = [this](auto&& area)
	{
		QDockWidget* dockWidget = new QDockWidget(tr("Dock Widget"), this);
		addDockWidget(area, dockWidget);
		QWidget* content = new QWidget(dockWidget);
		dockWidget->setWidget(content);

		content->setStyleSheet("QWidget{background:red;}");

		QLineEdit* edit = new QLineEdit(content);
		edit->setMinimumSize(QSize(200, 50));
		edit->setStyleSheet("QLineEdit{background:white;}");
		edit->installEventFilter(this);

		return dockWidget;
	};

	// add dock with line edit (to test keyboard focus)
	addDock(Qt::LeftDockWidgetArea);
	addDock(Qt::RightDockWidgetArea);

	HWND unityHandle = (HWND)hwnd;
	m_unityHandle = unityHandle;
	
	// add Unity window as central widget
	QWindow* window = QWindow::fromWinId(hwnd);
	QWidget* unityContainer = QWidget::createWindowContainer(window);

	setCentralWidget(unityContainer);

	// hook Unity window proc
	hookUnityProc();
}

bool Window::event(QEvent* event)
{
	if (event->type() == QEvent::WindowActivate && m_focusInUnity)
		setUnityFocus(true);
	else if (event->type() == QEvent::Close)
	{
		::SendMessageA(m_unityHandle, WM_CLOSE, 0, 0);
		return true;
	}

	return QMainWindow::event(event);
}


void Window::setUnityFocus(bool focus)
{
	if (focus)
		SendMessageA(m_unityHandle, WM_ACTIVATE, WA_ACTIVE, 0);
	else
	{
		SendMessageA(m_unityHandle, WM_ACTIVATE, WA_INACTIVE, 0);
		::SetFocus((HWND)effectiveWinId());
	}
}

void Window::hookUnityProc()
{
	int wndProcOffset = -4;
	OriginalUnityProc = (WNDPROC)(::GetWindowLongPtrW((HWND)m_unityHandle, wndProcOffset));
	if (!OriginalUnityProc)
	{
		char errorMesage[256];
		sprintf(errorMesage, "Hook Unity window failed, error %d", GetLastError());
		MessageBoxA(NULL, errorMesage, "Error", MB_OK);
		ExitProcess(0);
	}

	::SetWindowLongPtrW((HWND)m_unityHandle, wndProcOffset, (LONG_PTR)HookedUnityProc);
}

bool Window::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
	// before clicks in Qt, set focus back to main window
	MSG* msg = (MSG*)message;
	switch (msg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		setUnityFocusFlag(false);
		setUnityFocus(false);
		break;
	}
	return false;
}