/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	controller.cpp
*/

/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* hardware platform libraries */
#include <mbed.h>

/* asteroids */
#include "model.h"
#include "asteroids.h"

/* Joystick 5-way switch
*/
enum position { left,down,right,up,centre };
DigitalIn joystick[] = {P5_0, P5_1, P5_4, P5_2, P5_3};

/*	set up joystick controls to move ship 
		left 		-- rotate ship heading left,
		right 	-- rotate ship heading right,	
		up 			-- increase thrust 
		down 		-- decrease thrust
		center 	-- fire missile
*/
void controls(void){
if (joystick[left].read() == 0) {
player.heading += 25;
}else if (joystick[right].read() == 0) {
player.heading -= 25;
}else if (joystick[up].read() == 0) {
incThrust();
}else if (joystick[down].read() == 0) {
decThrust();
}else if (joystick[centre].read() == 0) {
missileSystem();
}
//use center joystick button to exit main menu and start game
while (mainmenu) {
if (joystick[centre].read() == 0) {
mainmenu = false;
}}
}

