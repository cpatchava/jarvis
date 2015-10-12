/* debug.h - Useful macros for debugging
 * vim:ts=4 noexpandtab
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include "types.h"

extern void keyboard_data();
void character_decode(uint32_t input);
/*unsigned char keys[128]=
{
	0,'\e','1','2','3','4','5','6','7','8','9',
	'0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't',
	'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ''', '`', 
	'\s', '\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	'\s'	, 0 ,
	0, 
	' ',, 40  
		

};*/


#endif /* _DEBUG_H */
