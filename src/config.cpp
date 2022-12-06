/*
** config.cpp
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

#include "config.h"
#include <SDL_filesystem.h>
#include <assert.h>

#include <stdint.h>
#include <vector>

#include "filesystem/filesystem.h"
#include "util/exception.h"
#include "util/debugwriter.h"
#include "util/sdl-util.h"
#include "util/util.h"

#include "util/json5pp.hpp"

#include "util/iniconfig.h"
#include "util/encoding.h"

#include "system/system.h"

namespace json = json5pp;

std::string prefPath(const char *org, const char *app) {
	char *path = SDL_GetPrefPath(org, app);
	if (!path)
		return std::string("");

	std::string ret(path);
	SDL_free(path);
	return ret;
}

void fillStringVec(json::value &item, std::vector<std::string> &vector) {
	if (!item.is_array()) {
		if (item.is_string())
			vector.push_back(item.as_string());

		return;
	}

	auto &array = item.as_array();
	for (size_t i = 0; i < array.size(); i++) {
		if (!array[i].is_string())
			continue;

		vector.push_back(array[i].as_string());
	}
}

bool copyObject(json::value &dest, json::value &src, const char *objectName = "") {
	assert(dest.is_object());

	if (src.is_null())
		return false;

	if (!src.is_object())
		return false;

	auto &srcVec = src.as_object();
	auto &destVec = dest.as_object();

	for (auto it : srcVec) {
		// Specifically processs this object later.
		if (it.second.is_object() && destVec[it.first].is_object())
			continue;

		if ((it.second.is_array() && destVec[it.first].is_array())	||
			(it.second.is_number() && destVec[it.first].is_number())  ||
			(it.second.is_string() && destVec[it.first].is_string())  ||
			(it.second.is_boolean() && destVec[it.first].is_boolean()) ||
			(destVec[it.first].is_null()))
		{
			destVec[it.first] = it.second;
		} else {
			Debug() << "Invalid variable in configuration:" << objectName << it.first;
		}
	}

	return true;
}

bool getEnvironmentBool(const char *env, bool defaultValue) {
	const char *e = SDL_getenv(env);
	if (!e)
		return defaultValue;

	if (!strcmp(e, "0"))
		return false;
	else if (!strcmp(e, "1"))
		return true;

	return defaultValue;
}

json::value readConfFile(const char *path) {
	json::value ret(0);

	if (!mkxp_fs::fileExists(path))
		return json::object({});

	try {
		std::string cfg = mkxp_fs::contentsOfFileAsString(path);
		ret = json::parse5(Encoding::convertString(cfg));
	} catch (const std::exception &e) {
		Debug() << "Failed to parse" << path << ":" << e.what();
	} catch (const Exception &e) {
		Debug() << "Failed to parse" << path << ":" << "Unknown encoding";
	}

	if (!ret.is_object())
		ret = json::object({});

	return ret;
}

#define CONF_FILE "modshot.json"

Config::Config() {}

void Config::read(int argc, char *argv[]) {
	auto optsJ = json::object({
		{"rgssVersion", 1},
		{"debugMode", false},
		{"printFPS", false},
		{"winResizable", false},
		{"fullscreen", false},
		{"fixedAspectRatio", true},
		{"smoothScaling", false},
		{"vsync", false},
		{"defScreenW", 0},
		{"defScreenH", 0},
		{"windowTitle", ""},
		{"fixedFramerate", 0},
		{"frameSkip", false},
		{"syncToRefreshrate", false},
		{"solidFonts", false},
#if defined(__APPLE__) && defined(__aarch64__)
		{"preferMetalRenderer", true},
#else
		{"preferMetalRenderer", false},
#endif
		{"subImageFix", false},
#ifdef __WIN32__
		{"enableBlitting", false},
#else
		{"enableBlitting", true},
#endif
		{"integerScalingActive", false},
		{"integerScalingLastMile", true},
		{"maxTextureSize", 0},
		{"gameFolder", "."},
		{"anyAltToggleFS", false},
		{"enableReset", false},
		{"enableSettings", true},
		{"allowSymlinks", false},
		{"dataPathOrg", ""},
		{"dataPathApp", "Oneshot"},
		{"iconPath", ""},
		{"execName", "modshot"},
		{"midiSoundFont", ""},
		{"midiChorus", false},
		{"midiReverb", false},
		{"SESourceCount", 6},
		{"customScript", ""},
		{"pathCache", true},
		{"useScriptNames", 1},
		{"preloadScript", json::array({})},
		{"RTP", json::array({})},
		{"fontSub", json::array({})},
		{"rubyLoadpath", json::array({"rubygems"})},
		{"JITEnable", false},
		{"JITVerboseLevel", 0},
		{"JITMaxCache", 100},
		{"JITMinCalls", 10000},
		{"bindingNames", json::object({
			{"action", "Action"},
			{"cancel", "Cancel"},
			{"menu", "Menu"},
			{"items", "Items"},
			{"run", "Run"},
			{"deactivate", "Deactivate"},
			{"l", "L"},
			{"r", "R"}
		})}
	});

	auto &opts = optsJ.as_object();

#define GUARD(exp) \
try { exp } catch (...) {}

	editor.debug = false;
	editor.battleTest = false;

	if (argc > 1) {
		if (!strcmp(argv[1], "debug") || !strcmp(argv[1], "test"))
			editor.debug = true;
		else if (!strcmp(argv[1], "btest"))
			editor.battleTest = true;
		
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "debug"))
				launchArgs.push_back(argv[i]);
		}
	}

	json::value baseConf = readConfFile(CONF_FILE);
	copyObject(optsJ, baseConf);
	copyObject(opts["bindingNames"], baseConf.as_object()["bindingNames"], "bindingNames .");

#define SET_OPT_CUSTOMKEY(var, key, type) GUARD(var = opts[#key].as_##type();)
#define SET_OPT(var, type) SET_OPT_CUSTOMKEY(var, var, type)
#define SET_STRINGOPT(var, key) GUARD(var = std::string(opts[#key].as_string());)

	SET_STRINGOPT(gameFolder, gameFolder);
	SET_STRINGOPT(dataPathOrg, dataPathOrg);
	SET_STRINGOPT(dataPathApp, dataPathApp);
	SET_STRINGOPT(iconPath, iconPath);
	SET_STRINGOPT(execName, execName);
	SET_OPT(allowSymlinks, boolean);
	SET_OPT(pathCache, boolean);
	SET_OPT_CUSTOMKEY(jit.enabled, JITEnable, boolean);
	SET_OPT_CUSTOMKEY(jit.verboseLevel, JITVerboseLevel, integer);
	SET_OPT_CUSTOMKEY(jit.maxCache, JITMaxCache, integer);
	SET_OPT_CUSTOMKEY(jit.minCalls, JITMinCalls, integer);
	SET_OPT(rgssVersion, integer);
	SET_OPT(defScreenW, integer);
	SET_OPT(defScreenH, integer);

	// Take a break real quick and witch to set game folder and read the game's ini
	if (!gameFolder.empty() && !mkxp_fs::setCurrentDirectory(gameFolder.c_str())) {
		throw Exception(Exception::MKXPError, "Unable to switch into gameFolder %s", gameFolder.c_str());
	}

	readGameINI();

	// Now check for an extra mkxp.conf in the user's save directory and merge anything else from that
	userConfPath = customDataPath + "/" CONF_FILE;
	json::value userConf = readConfFile(userConfPath.c_str());
	copyObject(optsJ, userConf);

	// now RESUME
	SET_OPT(debugMode, boolean);
	SET_OPT(printFPS, boolean);
	SET_OPT(fullscreen, boolean);
	SET_OPT(fixedAspectRatio, boolean);
	SET_OPT(smoothScaling, boolean);
	SET_OPT(winResizable, boolean);
	SET_OPT(vsync, boolean);
	SET_STRINGOPT(windowTitle, windowTitle);
	SET_OPT(fixedFramerate, integer);
	SET_OPT(frameSkip, boolean);
	SET_OPT(syncToRefreshrate, boolean);
	SET_OPT(solidFonts, boolean);
#ifdef __APPLE__
	SET_OPT(preferMetalRenderer, boolean);
#endif
	SET_OPT(subImageFix, boolean);
	SET_OPT(enableBlitting, boolean);
	SET_OPT_CUSTOMKEY(integerScaling.active, integerScalingActive, boolean);
	SET_OPT_CUSTOMKEY(integerScaling.lastMileScaling, integerScalingLastMile, boolean);
	SET_OPT(maxTextureSize, integer);
	SET_OPT(anyAltToggleFS, boolean);
	SET_OPT(enableReset, boolean);
	SET_OPT(enableSettings, boolean);
	SET_STRINGOPT(midi.soundFont, midiSoundFont);
	SET_OPT_CUSTOMKEY(midi.chorus, midiChorus, boolean);
	SET_OPT_CUSTOMKEY(midi.reverb, midiReverb, boolean);
	SET_OPT_CUSTOMKEY(SE.sourceCount, SESourceCount, integer);
	SET_STRINGOPT(customScript, customScript);
	SET_OPT(useScriptNames, boolean);

	fillStringVec(opts["preloadScript"], preloadScripts);
	fillStringVec(opts["RTP"], rtps);
	fillStringVec(opts["fontSub"], fontSubs);
	fillStringVec(opts["rubyLoadpath"], rubyLoadpaths);

	auto &bnames = opts["bindingNames"].as_object();

#define BINDING_NAME(btn) kbActionNames.btn = bnames[#btn].as_string()
	BINDING_NAME(action);
	BINDING_NAME(cancel);
	BINDING_NAME(menu);
	BINDING_NAME(items);
	BINDING_NAME(run);
	BINDING_NAME(deactivate);
	BINDING_NAME(l);
	BINDING_NAME(r);

	rgssVersion = clamp(rgssVersion, 0, 3);
	SE.sourceCount = clamp(SE.sourceCount, 1, 64);

	// Determine whether to open a console window on... Windows
	winConsole = getEnvironmentBool("MKXPZ_WINDOWS_CONSOLE", editor.debug);

#ifdef __APPLE__
	// Determine whether to use the Metal renderer on macOS
	// Environment variable takes priority over the json setting
	preferMetalRenderer = isMetalSupported() && getEnvironmentBool("MKXPZ_MACOS_METAL", preferMetalRenderer);
#endif

	// Determine whether to allow manual selection of a game folder on startup
	// Only works on macOS atm, mainly used to test games located outside of the bundle.
	// The config is re-read after the window is already created, so some entries
	// may not take effect
	manualFolderSelect = getEnvironmentBool("MKXPZ_FOLDER_SELECT", false);

#ifdef MKXPZ_STEAM
	// Override fullscreen config if enabled Steam Big Picture mode
	if (getEnvironmentBool("SteamTenfoot", false))
		fullscreen = true;
#endif

	raw = optsJ;
}

static void setupScreenSize(Config &conf) {
	if (conf.defScreenW <= 0)
		conf.defScreenW = (conf.rgssVersion == 1 ? 640 : 544);

	if (conf.defScreenH <= 0)
		conf.defScreenH = (conf.rgssVersion == 1 ? 480 : 416);
}

void Config::readGameINI() {
	/*
	std::string iniFileName(execName + ".ini");
	SDLRWStream iniFile(iniFileName.c_str(), "r");

	bool convSuccess = false;
	if (iniFile) {
		INIConfiguration ic;
		if (ic.load(iniFile.stream())) {
			GUARD(game.title = ic.getStringProperty("Game", "Title"););
			GUARD(game.scripts = ic.getStringProperty("Game", "Scripts"););

			strReplace(game.scripts, '\\', '/');

			if (game.title.empty()) 
				Debug() << iniFileName + ": Could not find Game.Title";

			if (game.scripts.empty())
				Debug() << iniFileName + ": Could not find Game.Scripts";
		}
	} else {
		Debug() << "Could not read" << iniFileName;
	}

	try {
		game.title = Encoding::convertString(game.title);
		convSuccess = true;
	} catch (const Exception &e) {
		Debug() << iniFileName + ": Could not determine encoding of Game.Title";
	}
	*/

	// Hardcode game and RGSS version settings for only OneShot game
	rgssVersion = 1;
	game.title = "OneShot";
	game.scripts = "Data/xScripts.rxdata";

	if (dataPathOrg.empty())
		dataPathOrg = ".";

	if (dataPathApp.empty())
		dataPathApp = game.title;

	customDataPath = prefPath(dataPathOrg.c_str(), dataPathApp.c_str());

	setupScreenSize(*this);
}
