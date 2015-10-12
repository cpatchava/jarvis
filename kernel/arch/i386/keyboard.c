#include "kernel/keyboard.h"
#include "kernel/i8259.h"
#include "kernel/types.h"
#include "kernel/lib.h"

unsigned char k_buffer[80];//its one line
unsigned int k_loc = 0;
unsigned int shift =0;
void keyboard_data(){
	uint32_t in = inb(0x60);
	
	character_decode(in);

	//clear();
	send_eoi(0x01); //end of interrupt
	asm volatile("leave");
	asm volatile("ret");
}

void character_decode(uint32_t input){
	char value;
	switch(input){
		
		case 0x01 ://escape
		break;
		
		case 0x02 :
		if(shift == 1){
			value = '!';
		}
		else{
			value = '1';
		}
		break;

		case 0x03 :
		if(shift == 1){
			value = '@';
		}
		else{
			value = '2';
		}
		break;

		case 0x04 :
		if(shift == 1){
			value = '#';
		}
		else{
			value = '3';
		}
		break;
		
		case 0x05 :
		if(shift == 1){
			value = '$';
		}
		else{
			value = '4';
		}
		break;

		case 0x06 :
		if(shift == 1){
			value = '%';
		}
		else{
			value = '5';
		}
		break;

		case 0x07 :
		if(shift == 1){
			value = '^';
		}
		else{
			value = '6';
		}
		break;
		
		case 0x08 :
		if(shift == 1){
			value = '&';
		}
		else{
			value = '7';
		}
		break;

		case 0x09 :
		if(shift == 1){
			value = '*';
		}
		else{
			value = '8';
		}
		break;

		case 0x0A :
		if(shift == 1){
			value = '(';
		}
		else{
			value = '9';
		}
		break;

		case 0x0B :
		if(shift == 1){
			value = ')';
		}
		else{
			value = '0';
		}
		break;
		
		case 0x0C :
		if(shift == 1){
			value = '_';
		}
		else{
			value = '-';
		}
		break;

		case 0x0D :
		if(shift == 1){
			value = '+';
		}
		else{
			value = '=';
		}
		break;

		case 0x0E :
		if(shift == 1){
			value = '\b';
		}
		else{
			value = '\b';
		}
		break;
		
		case 0x0F :
		if(shift == 1){
			value = '$';
		}
		else{
			value = '4';
		}
		break;

		case 0x10 :
		if(shift == 1){
			value = '%';
		}
		else{
			value = '5';
		}
		break;

		case 0x11 :
		if(shift == 1){
			value = '^';
		}
		else{
			value = '6';
		}
		break;
		
		case 0x12 :
		if(shift == 1){
			value = '&';
		}
		else{
			value = '7';
		}
		break;

		case 0x13 :
		if(shift == 1){
			value = '*';
		}
		else{
			value = '8';
		}
		break;

		case 0x14 :
		if(shift == 1){
			value = '(';
		}
		else{
			value = '9';
		}
		break;

		case 0x15 :
		if(shift == 1){
			value = ')';
		}
		else{
			value = '0';
		}
		break;



	}
	putc(value);

}

