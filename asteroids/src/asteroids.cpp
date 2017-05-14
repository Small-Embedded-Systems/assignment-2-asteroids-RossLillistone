/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	asteroids.cpp
*/
	
/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/* hardware platform libraries */
#include <display.h>
#include <mbed.h>

/* Main game elements */
#include "model.h"
#include "view.h"
#include "controller.h"

/* Game state */
float 	elapsed_time; 
int   	score;
int   	lives;
int 		shield;
int 		shieldfix;
struct 	ship player;
struct 	missile *shots; 
struct 	asteroid *asteroids;

float Dt = 0.01f;

Ticker model, view, controller;

bool mainmenu = false;
bool gameover = false;
bool paused = true;

int main()
{		
	//Initialises the double buffer
    init_DBuffer();
	
	  //Initialise heap
		initialiseMissileHeap();
	  initialiseAsteroidHeap();
    
	  //Attaches timers to the different files
    view.attach( draw, 0.025);
    model.attach( physics, Dt);
    controller.attach( controls, 0.05);
	
	  //game start variables
		player.p.x = 240;  /* player position is center horizontally on x axis */
	  player.p.y = 140;	 /* player position is center vertically on y axis */
		lives = 3;				 /* player start lives */
	  shield = 5;				 /* initial shield strength is max */
	  shieldfix = 0;		 /* set shield fix to zero - 1 bar of shield is replenished when this = 1000 */
	  mainmenu = true;   /* start game with main menu screen */
		
		 while(true) {
				if(lives == 0 && gameover == true) {
						//view.detach();
						model.detach();
						controller.detach();
				}
	}
    
}



