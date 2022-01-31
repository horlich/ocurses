/*
 * exceptions.h
 *
 *  Created on: 23.05.2020
 *      Author: herbert
 */

#ifndef OCURSES_EXCEPTIONS_H_
#define OCURSES_EXCEPTIONS_H_

#include <string>
#include <sstream>

#include "oexception.h"


namespace Ocurses {




class OcursesException : public OException::Fehler {
public:
	OcursesException(const std::string& message);

	virtual ~OcursesException() = default;
};


class CannotCreatePointer : public OcursesException {
public:
	CannotCreatePointer(const std::string& message);

	virtual ~CannotCreatePointer() = default;
};


class ColorException : public OcursesException {
public:
	ColorException(const std::string& message);

	virtual ~ColorException() = default;
};


class BadDesign : public OcursesException {
public:
	BadDesign(const std::string& message);

	BadDesign(const char* funcname, const std::string& message);

	virtual ~BadDesign() = default;
};














} // Ende Namespace Ocurses



#endif /* OCURSES_EXCEPTIONS_H_ */
