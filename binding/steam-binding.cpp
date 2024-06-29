#include "binding-util.h"

#ifdef MKXPZ_STEAM
#include "sharedstate.h"
#include "steam/steam.h"
#endif

RB_METHOD(steamEnabled)
{
	RB_UNUSED_PARAM;

#ifdef MKXPZ_STEAM
	return Qtrue;
#else
	return Qfalse;
#endif
}

RB_METHOD(steamUnlock)
{
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);

#ifdef MKXPZ_STEAM
	shState->steam().unlock(name);
#endif

	return Qnil;
}

RB_METHOD(steamLock)
{
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);

#ifdef MKXPZ_STEAM
	shState->steam().lock(name);
#endif

	return Qnil;
}

RB_METHOD(steamUnlocked)
{
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);

#ifdef MKXPZ_STEAM
	return shState->steam().isUnlocked(name) ? Qtrue : Qfalse;
#else
	return Qfalse;
#endif
}

void steamBindingInit()
{
	VALUE module = rb_define_module("Steam");

#ifdef MKXPZ_STEAM
	rb_const_set(module, rb_intern("USER_NAME"), rb_str_new2(shState->steam().userName().c_str()));
	if (shState->steam().lang().empty())
		rb_const_set(module, rb_intern("LANG"), Qnil);
	else
		rb_const_set(module, rb_intern("LANG"), rb_str_new2(shState->steam().lang().c_str()));
#else
	rb_const_set(module, rb_intern("USER_NAME"), Qnil);
	rb_const_set(module, rb_intern("LANG"), Qnil);
#endif

	_rb_define_module_function(module, "enabled?", steamEnabled);
	_rb_define_module_function(module, "unlock", steamUnlock);
	_rb_define_module_function(module, "lock", steamLock);
	_rb_define_module_function(module, "unlocked?", steamUnlocked);
}
