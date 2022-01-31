/*
 * exceptions.cpp
 *
 *  Created on: 23.05.2020
 *      Author: herbert
 */

#include "exceptions.h"

using namespace Ocurses;
using namespace std;


OcursesException::OcursesException(const std::string& message) : Fehler(message) {}


CannotCreatePointer::CannotCreatePointer(const std::string& message) : OcursesException(message) {}


BadDesign::BadDesign(const std::string& message) : OcursesException(message) {}


//std::string _badMess(const char* funcname, const std::string& message) {
//	std::stringstream buf;
//	buf << funcname << ": " << message;
//	return buf.str();
//}

BadDesign::BadDesign(const char* funcname, const std::string& message) :
		OcursesException(OException::envNameMess(funcname, message)) {}


ColorException::ColorException(const std::string& message) : OcursesException(message) {}




