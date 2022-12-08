#include "oneshot.h"
#include "notifications.h"
#include "etc.h"
#include "binding-util.h"
#include "binding-types.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "debugwriter.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

RB_METHOD(modshotNotify)
{
	RB_UNUSED_PARAM;
	char* title;
	char* info;
	VALUE icon = Qnil;
	rb_get_args(argc, argv, "zz|o", &title, &info, &icon RB_ARG_END);

#ifdef _WIN32
	if (!shState->notifi().hasTrayIcon())
		shState->notifi().addTrayIcon("OneShot");
#elif defined __linux__
	if (!shState->notifi().hasGApp())
		shState->notifi().regApp("org.ModShot.Notifier");
#endif

	switch (TYPE(icon))
	{
		case T_NIL:
			shState->notifi().send(title, info, 0, NULL);
			break;
		case T_FIXNUM:
			shState->notifi().send(title, info, NUM2INT(icon), NULL);
			break;
		case T_STRING:
		{
			std::string iconStr = std::string(RSTRING_PTR(icon), RSTRING_LEN(icon));
			shState->notifi().send(title, info, 0, iconStr.c_str());
			break;
		}
		default:
			shState->notifi().send(title, info, 0, NULL);
			break;
	}

	return Qnil;
}

RB_METHOD(modshotNotifyCleanup)
{
	RB_UNUSED_PARAM;

#ifdef _WIN32
	if (shState->notifi().hasTrayIcon())
		shState->notifi().delTrayIcon();
#elif defined __linux__
	if (shState->notifi().hasGApp())
		shState->notifi().quitApp();
#endif

	return Qnil;
}

RB_METHOD(modwindowGetPosition)
{
	int x, y;

	SDL_GetWindowPosition(shState->sdlWindow(), &x, &y);

	return rb_ary_new3(2, LONG2FIX(x), LONG2FIX(y));
}

RB_METHOD(modwindowSetPosition)
{
	int x, y;
	rb_get_args(argc, argv, "ii", &x, &y);

	SDL_SetWindowPosition(shState->sdlWindow(), x, y);

	return Qnil;
}

RB_METHOD(modwindowGetTitle)
{
	RB_UNUSED_PARAM;

	rb_check_argc(argc, 0);

	return rb_utf8_str_new_cstr(SDL_GetWindowTitle(shState->sdlWindow()));
}

RB_METHOD(modwindowSetTitle)
{
	RB_UNUSED_PARAM;

	VALUE s;
	rb_scan_args(argc, argv, "1", &s);
	SafeStringValue(s);

	shState->eThread().requestWindowRename(RSTRING_PTR(s));

	return s;
}

RB_METHOD(modwindowSetIcon)
{
	char *path;
	rb_get_args(argc, argv, "z", &path);
	SDL_Surface *icon = IMG_Load(path);

	if (!icon) {
		std::string excText("Setting icon failed: ");
		excText.append(IMG_GetError());
		rb_raise(rb_eRuntimeError, excText.c_str());
	}

	SDL_SetWindowIcon(shState->sdlWindow(), icon);

	return Qnil;
}

void modshotBindingInit()
{
	VALUE modshot_mod = rb_define_module("ModShot");
	VALUE modwindow_mod = rb_define_module("ModWindow");

	// ModShot module
	_rb_define_module_function(modshot_mod, "notify", modshotNotify);
	_rb_define_module_function(modshot_mod, "notify_cleanup", modshotNotifyCleanup);

	// ModWindow module
	_rb_define_module_function(modwindow_mod, "get_position", modwindowGetPosition);
	_rb_define_module_function(modwindow_mod, "set_position", modwindowSetPosition);
	_rb_define_module_function(modwindow_mod, "title", modwindowGetTitle);
	_rb_define_module_function(modwindow_mod, "title=", modwindowSetTitle);
	_rb_define_module_function(modwindow_mod, "set_icon", modwindowSetIcon);
}
