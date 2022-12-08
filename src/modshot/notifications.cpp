#include "notifications.h"
#include "eventthread.h"
#include "SDL2/SDL.h"
#include "debugwriter.h"

#ifdef _WIN32
	#include <windows.h>
	#include <gdiplus.h>
	#include <SDL2/SDL_syswm.h>
	#include "resource.h"
#elif defined __linux__
	#include <gio/gio.h>
#endif

#ifdef _WIN32
static WCHAR *w32_toWide(const char *str)
{
	if (str) {
		int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);

		if (size > 0) {
			WCHAR *ustr = new WCHAR[size];

			if (MultiByteToWideChar(CP_UTF8, 0, str, -1, ustr, size) == size)
				return ustr;

			delete [] ustr;
		}
	}

	// Return empty string
	WCHAR *ustr = new WCHAR[1];
	*ustr = 0;

	return ustr;
}
#endif

struct NotificationsPrivate
{
	SDL_Window *window;

#ifdef _WIN32
	bool hasTrayIcon;
#elif defined __linux__
	GApplication *gApp;
	bool hasGApp;
#endif
};

Notifications::Notifications(RGSSThreadData &threadData) : threadData(threadData)
{
	p = new NotificationsPrivate();

	p->window = threadData.window;

#ifdef _WIN32
	p->hasTrayIcon = false;
#elif __linux__
	p->hasGApp = false;
#endif
}

Notifications::~Notifications()
{
#ifdef _WIN32
	if (p->hasTrayIcon)
		delTrayIcon();
#elif defined __linux__
	if (p->hasGApp)
		quitApp();
#endif

	delete p;
}

#ifdef _WIN32
bool Notifications::addTrayIcon(const char *tip)
{
	if (p->hasTrayIcon)
		return false;

	// Convert to wide char string
	const wchar_t *wTip = w32_toWide(tip);

	// Get window handle
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(p->window, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	// Get Win32 handle
	HINSTANCE hInst = GetModuleHandle(NULL);

	// Prepare Notify Icon Data
	NOTIFYICONDATAW nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATAW));
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hWnd;
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = WM_APP + 1; // 32769
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPICON)); // main app icon
	wcscpy_s(nid.szTip, sizeof(nid.szTip), wTip);

	// Add notify icon on tray
	bool result = Shell_NotifyIconW(NIM_ADD, &nid);

	if (result)
		p->hasTrayIcon = true;

	return result;
}

bool Notifications::delTrayIcon()
{
	if (!p->hasTrayIcon)
		return true;

	// Get window handle
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(p->window, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	// Prepare Notify Icon Data
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uFlags = 0x0;

	// Delete notify icon from tray
	bool result = Shell_NotifyIcon(NIM_DELETE, &nid);

	if (result)
		p->hasTrayIcon = false;

	return result;
}

bool Notifications::hasTrayIcon()
{
	return p->hasTrayIcon;
}
#endif

#ifdef __linux__
bool Notifications::regApp(const char *appId)
{
	if (p->hasGApp) {
		Debug() << "Already registered Gio application";
		return true;
	}

	GError **gErr;
	p->gApp = g_application_new(appId, G_APPLICATION_FLAGS_NONE);
	g_application_register(p->gApp, nullptr, gErr);

	if (gErr) {
		Debug() << "Failed to register Gio application!";
		return false;
	} else {
		p->hasGApp = true;
		return true;
	}
}

bool Notifications::quitApp()
{
	if (!p->hasGApp)
		return true;

	g_application_quit(p->gApp);
	p->hasGApp = false;

	return true;
}

bool Notifications::hasGApp()
{
	return p->hasGApp;
}
#endif

bool Notifications::send(const char *title, const char *body, const int iconId, const char *iconPath)
{
#ifdef _WIN32
	if (!p->hasTrayIcon)
		return false;

	// Convert to wide char strings
	const wchar_t* wTitle = w32_toWide(title);
	const wchar_t* wBody = w32_toWide(body);

	// Get window handle
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(p->window, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	// Prepare Notify Icon Data
	NOTIFYICONDATAW nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATAW));
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hWnd;
	nid.uID = 0;
	nid.uFlags = NIF_INFO;
	wcscpy_s(nid.szInfo, sizeof(nid.szInfo), wBody);
	wcscpy_s(nid.szInfoTitle, sizeof(nid.szInfoTitle), wTitle);

	if (iconId > 0 && iconId <= 4) {
		switch (iconId)
		{
			case 1:
				// An information icon
				nid.dwInfoFlags = NIIF_INFO;
				break;
			case 2:
				// A warning icon
				nid.dwInfoFlags = NIIF_WARNING;
				break;
			case 3:
				// An error icon
				nid.dwInfoFlags = NIIF_ERROR;
				break;
			case 4:
				// An icon from executable
				nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
				break;
			default:
				// No icon
				nid.dwInfoFlags = NIIF_NONE;
				break;
		}
	} else if (iconPath) {
		const wchar_t* wIconPath = w32_toWide(iconPath);

		// Startup GDI+
		Gdiplus::GdiplusStartupInput gdiStartupInput;
		ULONG_PTR gdiToken;
		Gdiplus::GdiplusStartup(&gdiToken, &gdiStartupInput, NULL);

		// Load image and get icon handle
		Gdiplus::Bitmap* gdiBitmap = Gdiplus::Bitmap::FromFile(wIconPath, false);
		HICON hIcon;
		gdiBitmap->GetHICON(&hIcon);

		// Shutdown GDI+
		Gdiplus::GdiplusShutdown(gdiToken);

		nid.hBalloonIcon = hIcon;
		nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
	}

	// Modify notify icon data to show balloon
	bool result = Shell_NotifyIconW(NIM_MODIFY, &nid);

	return result;
#elif defined __linux__
	if (!p->hasGApp)
		return false;

	// Create Gio notification object
	g_autoptr(GNotification) gioNotify = g_notification_new(title);
	g_notification_set_body(gioNotify, body);

	// Set notification icon
	if (iconId > 0 && iconId <= 4) {
		// Set icon from FreeDesktop defineded
		switch (iconId)
		{
			case 1:
				// An information icon
				g_notification_set_icon(gioNotify, g_themed_icon_new("dialog-information"));
				break;
			case 2:
				// A warning icon
				g_notification_set_icon(gioNotify, g_themed_icon_new("dialog-warning"));
				break;
			case 3:
				// An error icon
				g_notification_set_icon(gioNotify, g_themed_icon_new("dialog-error"));
				break;
		}
	} else if (iconPath) {
		// Set icon from local file
		g_autoptr(GFile) gioFile = g_file_new_for_path(iconPath);
		g_autoptr(GIcon) gioIcon = g_file_icon_new(gioFile);
		g_notification_set_icon(gioNotify, gioIcon);
	}

	// Send notification to Gio application
	g_application_send_notification(p->gApp, "oneshot-notification", gioNotify);

	return true;
#endif
}
