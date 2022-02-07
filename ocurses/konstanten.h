/*
 * konstanten.h
 *
 *  Created on: 31.05.2020
 *      Author: herbert
 */

#ifndef OCURSES_KONSTANTEN_H_
#define OCURSES_KONSTANTEN_H_


namespace Ocurses {


/*
 *   Siehe
 *   - 'man terminfo'
 *   - 'man termios' #include<termios.h>
 *
 *   Symbolische Konstanten (Keys), Sonderzeichen(VT100-Symbole ACS_*)
 *   und Makros sind in /usr/include/ncurses.h definiert.
 */




/*
 *  Verfügbare Farbkonstanten:
 *
             COLOR_BLACK
             COLOR_RED
             COLOR_GREEN
             COLOR_YELLOW
             COLOR_BLUE
             COLOR_MAGENTA
             COLOR_CYAN
             COLOR_WHITE
 *
 * */



class ColorPair;
class FullScreenWindow;


/* Definiert in nodes.cpp: */
extern ColorPair WEISS_BLAU;
extern ColorPair BLAU_WEISS;
extern ColorPair SCHWARZ_WEISS;
extern ColorPair WEISS_SCHWARZ;
extern ColorPair WEISS_ROT;
extern ColorPair ROT_WEISS;


using WindowResponse = int;

/* Diese Konstanten dürfen keine Werte von Tastatur-Keys *
 * (ASCII oder Latin1) enthalten:                        */
const WindowResponse CONTINUE_LISTENING	= 1000;
const WindowResponse QUIT_PROGRAM_KEY 	   = 1010;
const WindowResponse QUIT_WINDOW 	   	= 1013;
const WindowResponse ASK_PARENT	 		   = 1015;



} // Ende Namespace Ocurses


#endif /* OCURSES_KONSTANTEN_H_ */
