/*
** gl-debug.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 - 2021 Amaryllis Kulla <ancurio@mapleshrine.eu>
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

#include "gl-debug.h"
#include "gl-fun.h"
#include "debugwriter.h"

#include <iostream>

struct GLDebugLoggerPrivate
{
	std::ostream *logStream;

	GLDebugLoggerPrivate(const char *filename)
	{
		(void)filename;

		logStream = &std::clog;
	}

	~GLDebugLoggerPrivate()
	{
	}

	void output(std::string line)
	{
		*logStream << "[GLDEBUG] " << line << std::endl;

		logStream->flush();
	}
};

static void APIENTRY arbDebugFunc(GLenum source,
                                  GLenum type,
                                  GLuint id,
                                  GLenum severity,
                                  GLsizei length,
                                  const GLchar *message,
                                  const void *userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	GLDebugLoggerPrivate *p = static_cast<GLDebugLoggerPrivate *>(const_cast<void *>(userParam));

	std::stringstream logMessage;

	logMessage << "[";

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:
			logMessage << "GL API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			logMessage << "Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			logMessage << "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			logMessage << "Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			logMessage << "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			logMessage << "Other";
			break;
	}

	logMessage << " | ";

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:
			logMessage << "API Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			logMessage << "Deprecated Behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			logMessage << "Undefined Behavior";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			logMessage << "Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			logMessage << "Performance";
			break;
		case GL_DEBUG_TYPE_MARKER:
			logMessage << "Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			logMessage << "Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			logMessage << "Pop Group";
			break;
		case GL_DEBUG_TYPE_OTHER:
			logMessage << "Other";
			break;
	}

	logMessage << " | ";

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			logMessage << "Notice";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			logMessage << "Low";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			logMessage << "Medium";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			logMessage << "High";
			break;
	}

	logMessage << "] [" << id << "] ";
	logMessage << std::string(message, length);

	p->output(logMessage.str());
}

GLDebugLogger::GLDebugLogger(const char *filename)
{
	p = new GLDebugLoggerPrivate(filename);

	if (gl.DebugMessageCallback)
		gl.DebugMessageCallback(arbDebugFunc, p);
	else
		Debug() << "[GLDEBUG] No debug extensions found";
}

GLDebugLogger::~GLDebugLogger()
{
	delete p;
}
