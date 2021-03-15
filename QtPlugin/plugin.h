#pragma once

#if defined(QT_PLUGIN_EXPORT)
#define QT_PLUGIN_API __declspec(dllexport)
#else
#define QT_PLUGIN_API __declspec(dllimport)
#endif

extern "C"
{
	QT_PLUGIN_API void showQtApp();
}