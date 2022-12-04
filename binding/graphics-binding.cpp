/*
** graphics-binding.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "graphics.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "exception.h"

#if RAPI_MAJOR >= 2
#include <ruby/thread.h>
#endif

RB_METHOD(graphicsDelta)
{
	RB_UNUSED_PARAM;

	GFX_LOCK;
	VALUE ret = ULL2NUM(shState->graphics().getDelta());
	GFX_UNLOCK;

	return ret;
}

RB_METHOD(graphicsUpdate)
{
	RB_UNUSED_PARAM;

#if RAPI_MAJOR >= 2
	rb_thread_call_without_gvl([](void*) -> void* {
		GFX_LOCK;
		shState->graphics().update();
		GFX_UNLOCK;
		return 0;
	}, 0, 0, 0);
#else
	shState->graphics().update();
#endif

	return Qnil;
}

RB_METHOD(graphicsAverageFrameRate)
{
	RB_UNUSED_PARAM;

	GFX_LOCK;
	VALUE ret = rb_float_new(shState->graphics().averageFrameRate());
	GFX_UNLOCK;

	return ret;
}

RB_METHOD(graphicsFreeze)
{
	RB_UNUSED_PARAM;

	GFX_LOCK;
	shState->graphics().freeze();
	GFX_UNLOCK;

	return Qnil;
}

RB_METHOD(graphicsTransition)
{
	RB_UNUSED_PARAM;

	int duration = 8;
	const char *filename = "";
	int vague = 40;

	rb_get_args(argc, argv, "|izi", &duration, &filename, &vague RB_ARG_END);

	GFX_GUARD_EXC( shState->graphics().transition(duration, filename, vague); )

	return Qnil;
}

RB_METHOD(graphicsFrameReset)
{
	RB_UNUSED_PARAM;

	GFX_LOCK;
	shState->graphics().frameReset();
	GFX_UNLOCK;

	return Qnil;
}

#define DEF_GRA_PROP_I(PropName) \
	RB_METHOD(graphics##Get##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_fix_new(shState->graphics().get##PropName()); \
	} \
	RB_METHOD(graphics##Set##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		int value; \
		rb_get_args(argc, argv, "i", &value RB_ARG_END); \
		GFX_LOCK; \
		shState->graphics().set##PropName(value); \
		GFX_UNLOCK; \
		return rb_fix_new(value); \
	}

#define DEF_GRA_PROP_B(PropName) \
	RB_METHOD(graphics##Get##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_bool_new(shState->graphics().get##PropName()); \
	} \
	RB_METHOD(graphics##Set##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		bool value; \
		rb_get_args(argc, argv, "b", &value RB_ARG_END); \
		GFX_LOCK; \
		shState->graphics().set##PropName(value); \
		GFX_UNLOCK; \
		return rb_bool_new(value); \
	}

#define DEF_GRA_PROP_F(PropName) \
	RB_METHOD(graphics##Get##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_float_new(shState->graphics().get##PropName()); \
	} \
	RB_METHOD(graphics##Set##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		double value; \
		rb_get_args(argc, argv, "f", &value RB_ARG_END); \
		GFX_LOCK; \
		shState->graphics().set##PropName(value); \
		GFX_UNLOCK; \
		return rb_float_new(value); \
	}

RB_METHOD(graphicsWidth)
{
	RB_UNUSED_PARAM;

	return rb_fix_new(shState->graphics().width());
}

RB_METHOD(graphicsHeight)
{
	RB_UNUSED_PARAM;

	return rb_fix_new(shState->graphics().height());
}

RB_METHOD(graphicsDisplayWidth)
{
    RB_UNUSED_PARAM;
    
    return rb_fix_new(shState->graphics().displayWidth());
}

RB_METHOD(graphicsDisplayHeight)
{
    RB_UNUSED_PARAM;
    
    return rb_fix_new(shState->graphics().displayHeight());
}

RB_METHOD(graphicsWait)
{
	RB_UNUSED_PARAM;

	int duration;
	rb_get_args(argc, argv, "i", &duration RB_ARG_END);

#if RAPI_MAJOR >= 2
	rb_thread_call_without_gvl([](void* d) -> void* {
		GFX_LOCK;
		shState->graphics().wait(*(int*)d);
		GFX_UNLOCK;
		return 0;
	}, (int*)&duration, 0, 0);
#else
	shState->graphics().wait(duration);
#endif

	return Qnil;
}

RB_METHOD(graphicsFadeout)
{
	RB_UNUSED_PARAM;

	int duration;
	rb_get_args(argc, argv, "i", &duration RB_ARG_END);

	GFX_LOCK;
	shState->graphics().fadeout(duration);
	GFX_UNLOCK;

	return Qnil;
}

RB_METHOD(graphicsFadein)
{
	RB_UNUSED_PARAM;

	int duration;
	rb_get_args(argc, argv, "i", &duration RB_ARG_END);

	GFX_LOCK;
	shState->graphics().fadein(duration);
	GFX_UNLOCK;

	return Qnil;
}

void bitmapInitProps(Bitmap *b, VALUE self);

RB_METHOD(graphicsSnapToBitmap)
{
	RB_UNUSED_PARAM;

	Bitmap *result = 0;

	GFX_GUARD_EXC( result = shState->graphics().snapToBitmap(); );

	VALUE obj = wrapObject(result, BitmapType);
	bitmapInitProps(result, obj);

	return obj;
}

RB_METHOD(graphicsResizeScreen)
{
	RB_UNUSED_PARAM;

	int width, height;
	rb_get_args(argc, argv, "ii", &width, &height RB_ARG_END);

	GFX_LOCK;
	shState->graphics().resizeScreen(width, height);
	GFX_UNLOCK;

	return Qnil;
}

RB_METHOD(graphicsResizeWindow)
{
	RB_UNUSED_PARAM;

	int width, height;
	bool center = false;
	rb_get_args(argc, argv, "ii|b", &width, &height, &center RB_ARG_END);

	GFX_LOCK;
	shState->graphics().resizeWindow(width, height, center);
	GFX_UNLOCK;

	return Qnil;
}

RB_METHOD(graphicsReset)
{
	RB_UNUSED_PARAM;

	GFX_LOCK;
	shState->graphics().reset();
	GFX_UNLOCK;

	return Qnil;
}

RB_METHOD(graphicsCenter)
{
	RB_UNUSED_PARAM;

	shState->graphics().center();

	return Qnil;
}

typedef struct
{
	const char *filename;
	int volume;
	bool skippable;
} PlayMovieArgs;

void *playMovieInternal(void *args)
{
	PlayMovieArgs *a = (PlayMovieArgs*)args;

	GFX_GUARD_EXC(
		shState->graphics().playMovie(a->filename, a->volume, a->skippable);

		// Signals for shutdown or reset only make
		// playMovie quit early, so check again.
		shState->graphics().update();
	);

	return 0;
}

RB_METHOD(graphicsPlayMovie)
{
	RB_UNUSED_PARAM;

	VALUE filename, volumeArg, skippable;
	rb_scan_args(argc, argv, "12", &filename, &volumeArg, &skippable);
	SafeStringValue(filename);

	bool skip;
	rb_bool_arg(skippable, &skip);

	// TODO: Video control inputs (e.g. skip, pause)

	PlayMovieArgs args{};
	args.filename = RSTRING_PTR(filename);
	args.volume = (volumeArg == Qnil) ? 100 : NUM2INT(volumeArg);;
	args.skippable = skip;
#if RAPI_MAJOR >= 2
	rb_thread_call_without_gvl(playMovieInternal, &args, 0, 0);
#else
	playMovieInternal(&args);
#endif

	return Qnil;
}

void graphicsScreenshotInternal(const char *filename)
{
	GFX_GUARD_EXC(shState->graphics().screenshot(filename););
}

RB_METHOD(graphicsScreenshot)
{
	RB_UNUSED_PARAM;

	VALUE filename;
	rb_scan_args(argc, argv, "1", &filename);
	SafeStringValue(filename);

#if RAPI_MAJOR >= 2
	rb_thread_call_without_gvl([](void* fn) -> void* {
		graphicsScreenshotInternal((const char*)fn);
		return 0;
	}, (void*)RSTRING_PTR(filename), 0, 0);
#else
	graphicsScreenshotInternal(RSTRING_PTR(filename));
#endif

    return Qnil;
}

DEF_GRA_PROP_I(FrameRate)
DEF_GRA_PROP_I(FrameCount)
DEF_GRA_PROP_I(Brightness)

DEF_GRA_PROP_B(Fullscreen)
DEF_GRA_PROP_B(ShowCursor)
DEF_GRA_PROP_F(Scale)
DEF_GRA_PROP_B(Frameskip)
DEF_GRA_PROP_B(FixedAspectRatio)
DEF_GRA_PROP_B(SmoothScaling)
DEF_GRA_PROP_B(IntegerScaling)
DEF_GRA_PROP_B(LastMileScaling)
DEF_GRA_PROP_B(Threadsafe)

#define INIT_GRA_PROP_BIND(PropName, prop_name_s) \
{ \
	_rb_define_module_function(module, prop_name_s, graphics##Get##PropName); \
	_rb_define_module_function(module, prop_name_s "=", graphics##Set##PropName); \
}

void graphicsBindingInit()
{
	VALUE module = rb_define_module("Graphics");

	_rb_define_module_function(module, "delta", graphicsDelta);
	_rb_define_module_function(module, "update", graphicsUpdate);
	_rb_define_module_function(module, "freeze", graphicsFreeze);
	_rb_define_module_function(module, "transition", graphicsTransition);
	_rb_define_module_function(module, "frame_reset", graphicsFrameReset);
	_rb_define_module_function(module, "screenshot", graphicsScreenshot);

	_rb_define_module_function(module, "__reset__", graphicsReset);

	INIT_GRA_PROP_BIND( FrameRate,  "frame_rate"  );
	INIT_GRA_PROP_BIND( FrameCount, "frame_count" );
	_rb_define_module_function(module, "average_frame_rate", graphicsAverageFrameRate);

	_rb_define_module_function(module, "width", graphicsWidth);
	_rb_define_module_function(module, "height", graphicsHeight);
	_rb_define_module_function(module, "display_width", graphicsDisplayWidth);
	_rb_define_module_function(module, "display_height", graphicsDisplayHeight);
	_rb_define_module_function(module, "wait", graphicsWait);
	_rb_define_module_function(module, "fadeout", graphicsFadeout);
	_rb_define_module_function(module, "fadein", graphicsFadein);
	_rb_define_module_function(module, "snap_to_bitmap", graphicsSnapToBitmap);
	_rb_define_module_function(module, "resize_screen", graphicsResizeScreen);
	_rb_define_module_function(module, "resize_window", graphicsResizeWindow);
	_rb_define_module_function(module, "center", graphicsCenter);
	_rb_define_module_function(module, "play_movie", graphicsPlayMovie);

	INIT_GRA_PROP_BIND( Brightness,       "brightness"         );
	INIT_GRA_PROP_BIND( Fullscreen,       "fullscreen"         );
	INIT_GRA_PROP_BIND( ShowCursor,       "show_cursor"        );
	INIT_GRA_PROP_BIND( Scale,            "scale"              );
	INIT_GRA_PROP_BIND( Frameskip,        "frameskip"          );
	INIT_GRA_PROP_BIND( FixedAspectRatio, "fixed_aspect_ratio" );
	INIT_GRA_PROP_BIND( SmoothScaling,    "smooth"             );
	INIT_GRA_PROP_BIND( IntegerScaling,   "integer_scaling"    );
	INIT_GRA_PROP_BIND( LastMileScaling,  "last_mile_scaling"  );
	INIT_GRA_PROP_BIND( Threadsafe,       "thread_safe"        );
}
