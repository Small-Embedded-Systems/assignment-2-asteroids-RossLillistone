/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	view.cpp
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

#include "asteroids.h"
#include "model.h"
#include "utils.h"

/* Static Method Variables */
Display *graphics = Display::theDisplay();
const colour_t background = BLACK; 

/*variables exclusive to this file*/
#define ufo2_width 64
#define ufo2_height 27
static unsigned char ufo2_bits[] = {
   0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
   0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xc0, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff,
   0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x20, 0xff, 0xff, 0x0c, 0x00, 0x00,
   0x00, 0x00, 0x30, 0x73, 0xcf, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x63,
   0xc6, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x77, 0xee, 0x03, 0x00, 0x00,
   0x00, 0x00, 0x1e, 0xfe, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0x7f, 0x00,
   0x00, 0xff, 0x0f, 0x00, 0x00, 0xfe, 0xff, 0xbf, 0xfa, 0xff, 0x7f, 0x00,
   0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x0f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x3f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f,
   0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00,
   0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff,
   0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xbf, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
   0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x00 };
typedef uint16_t colour_t;

//define ship shape for use with Player.p.x and player.p.y co-ordinates
const coordinate_t shape[] = {
    {-5,-5}, {10,0}, {-5,5}
};
//define asteroid shape for use with asteroid.p.x and asteroid.p.y co-ordinates
const coordinate_t roid[] = {
    {20,0},{12,20},{12,20},{-8,20},{-8,20},{-20,4},{-20,4},{-12,-20},{-12,-20},{-4,-8},{-4,-8},{0,-20},{0,-20},{20,0}
};
//define smaller asteroid alternate shape for use with asteroid.p.x and asteroid.p.y co-ordinates
const coordinate_t roid2[] = {
    {10,0},{6,10},{6,10},{-4,10},{-4,10},{-10,2},{-10,2},{-6,-10},{-6,-10},{-2,-4},{-2,-4},{0,-10},{0,-10},{10,0}
};

/* Static Methods */
static void draw_overlay(int score, int elapsed_time, int lives, int shield) ;
static void draw_start(void);
static void draw_end(void);
static void drawshield(void);
static void drawmissiles(struct missile *lst);
static void drawasteroids(struct asteroid *lst);
static void drawship(ship player);	

/* double buffering functions */
void init_DBuffer(void)
{   /* initialise the LCD driver to use second frame in buffer */
    uint16_t *bufferbase = graphics->getFb();
    uint16_t *nextbuffer = bufferbase+ (480*272);
    LPC_LCD->UPBASE = (uint32_t)nextbuffer;
}

void swap_DBuffer(void)
{   /* swaps frames used by the LCD driver and the graphics object */
    uint16_t *buffer = graphics->getFb();
    graphics->setFb( (uint16_t*) LPC_LCD->UPBASE);
    LPC_LCD->UPBASE = (uint32_t)buffer;
}

//draw main game display
void draw_overlay(int score, int elapsed_time, int lives, int shield) {
	graphics->setCursor(20,10);
	graphics->setTextSize(2);
	graphics->printf("SCORE");
	graphics->setCursor(40,30);
	graphics->printf("%d", score);
	graphics->setCursor(40,220);
	graphics->printf("%d",lives);
	graphics->setCursor(20,240);
	graphics->printf("LIVES");
	graphics->setTextSize(1);
	graphics->setCursor(412,250);
	graphics->printf("SHIELD: %d",shield);
	// Hide or reveal shield power sections based on value of int shield
	if (shield == 5) {
	graphics->drawRect(430,220, 30,20, WHITE);
	graphics->fillRect(431,221, 28,18, LIGHTBLUE);
	graphics->drawRect(430,190, 30,20, WHITE);
	graphics->fillRect(431,191, 28,18, LIGHTBLUE);
	graphics->drawRect(430,160, 30,20, WHITE);
	graphics->fillRect(431,161, 28,18, LIGHTBLUE);
	graphics->drawRect(430,130, 30,20, WHITE);
	graphics->fillRect(431,131, 28,18, LIGHTBLUE);
	}else if (shield == 4) {
	graphics->drawRect(430,220, 30,20, WHITE);
	graphics->fillRect(431,221, 28,18, LIGHTBLUE);
	graphics->drawRect(430,190, 30,20, WHITE);
	graphics->fillRect(431,191, 28,18, LIGHTBLUE);
	graphics->drawRect(430,160, 30,20, WHITE);
	graphics->fillRect(431,161, 28,18, LIGHTBLUE);
	graphics->drawRect(430,130, 30,20, WHITE);
	}else if (shield == 3) {
	graphics->drawRect(430,220, 30,20, WHITE);
	graphics->fillRect(431,221, 28,18, LIGHTBLUE);
	graphics->drawRect(430,190, 30,20, WHITE);
	graphics->fillRect(431,191, 28,18, LIGHTBLUE);
	graphics->drawRect(430,160, 30,20, WHITE);
	graphics->drawRect(430,130, 30,20, WHITE);
	}else if (shield == 2) {
	graphics->drawRect(430,220, 30,20, WHITE);
	graphics->fillRect(431,221, 28,18, LIGHTBLUE);
	graphics->drawRect(430,190, 30,20, WHITE);
	graphics->drawRect(430,160, 30,20, WHITE);
	graphics->drawRect(430,130, 30,20, WHITE);
	}else if (shield == 1) {
	graphics->drawRect(430,220, 30,20, WHITE);
	graphics->drawRect(430,190, 30,20, WHITE);
	graphics->drawRect(430,160, 30,20, WHITE);
	graphics->drawRect(430,130, 30,20, WHITE);
	}else if (shield == 0) {
	}
}

//Start menu screen, visable at game load or following gameover screen.
void draw_start(void){
	graphics->setTextSize(7);
	graphics->fillScreen(background);
	graphics->setCursor(140,105);
	graphics->printf("ROIDS");
	graphics->setTextSize(2);
	graphics->setCursor(131,175);
	graphics->printf("PRESS FIRE TO BEGIN");
	graphics->setTextSize(1);
	graphics->setCursor(16,240);
	graphics->printf("AN ASTEROID CLONE --  SMALL EMBEDDED SYSTEMS -- ROSS LILLISTONE W15021733");
	graphics->drawBitmap(210, 25, ufo2_bits, 64, 27, WHITE);
}

//Game over screen, visable once all lives are lost.
void draw_end(void){
	//bitmap of crash or explosion or skull?? would be good
	graphics->setTextSize(4);
	graphics->fillScreen(background);
  graphics->setCursor(85,105);
	graphics->printf("YOU HAVE DIED");
	graphics->setTextSize(2);
	graphics->setCursor(100,145);
	graphics->printf("YOUR SCORE IS IRRELEVANT");
  graphics->setTextSize(2);
	graphics->setCursor(90,220);
	graphics->printf("PRESS RESET TO RELOAD GAME");
	graphics->drawBitmap(210, 25, ufo2_bits, 64, 27, RED);
}

//Shield circling around player ship, rings hide or reaveal based on int shield value.
void drawshield(void) {
	if(shield == 5) {
		graphics->drawCircle(player.p.x,player.p.y,10,WHITE);	
    graphics->drawCircle(player.p.x,player.p.y,11,LIGHTBLUE);
    graphics->drawCircle(player.p.x,player.p.y,12,LIGHTBLUE);
    graphics->drawCircle(player.p.x,player.p.y,13,LIGHTBLUE);	
    graphics->drawCircle(player.p.x,player.p.y,14,RED);		
	}else if (shield == 4) {
    graphics->drawCircle(player.p.x,player.p.y,10,LIGHTBLUE);
    graphics->drawCircle(player.p.x,player.p.y,11,LIGHTBLUE);
    graphics->drawCircle(player.p.x,player.p.y,12,LIGHTBLUE);	
    graphics->drawCircle(player.p.x,player.p.y,13,RED);	
	}else if (shield == 3) {
    graphics->drawCircle(player.p.x,player.p.y,10,LIGHTBLUE);
    graphics->drawCircle(player.p.x,player.p.y,11,LIGHTBLUE);	
    graphics->drawCircle(player.p.x,player.p.y,12,RED);			
	}else if (shield == 2) {
    graphics->drawCircle(player.p.x,player.p.y,10,LIGHTBLUE);	
    graphics->drawCircle(player.p.x,player.p.y,11,RED);		
	}else if (shield == 1) {
    graphics->drawCircle(player.p.x,player.p.y,10,RED);	
	}else
    graphics->drawCircle(player.p.x,player.p.y,10,BLACK);
};

//Draw missiles to screen.
void drawmissiles(struct missile *lst) {
	while(lst) {
		graphics->drawCircle(lst->p.x,lst->p.y,1,WHITE);
    lst = lst->next;		
	}
};

//Draw asteroids to screen.
void drawasteroids(struct asteroid *lst){
   while(lst) {
		 //randomly select which asteroid to draw based on the random value of asteroid->type
		 if (lst->type < 5) {
			 //large green style asteroid
   float roidAngle = radians(lst->heading);
	 int rXrotate1 = int((roid[0].x * cos(roidAngle)) - (roid[0].y * sin(roidAngle))+lst->p.x);
   int rYrotate1 = int((roid[0].x * sin(roidAngle)) + (roid[0].y * cos(roidAngle))+lst->p.y);
   int rXrotate2 = int((roid[1].x * cos(roidAngle)) - (roid[1].y * sin(roidAngle))+lst->p.x);
   int rYrotate2 = int((roid[1].x * sin(roidAngle)) + (roid[1].y * cos(roidAngle))+lst->p.y);
   int rXrotate3 = int((roid[2].x * cos(roidAngle)) - (roid[2].y * sin(roidAngle))+lst->p.x);
   int rYrotate3 = int((roid[2].x * sin(roidAngle)) + (roid[2].y * cos(roidAngle))+lst->p.y);
	 int rXrotate4 = int((roid[3].x * cos(roidAngle)) - (roid[3].y * sin(roidAngle))+lst->p.x);
   int rYrotate4 = int((roid[3].x * sin(roidAngle)) + (roid[3].y * cos(roidAngle))+lst->p.y);
   int rXrotate5 = int((roid[4].x * cos(roidAngle)) - (roid[4].y * sin(roidAngle))+lst->p.x);
   int rYrotate5 = int((roid[4].x * sin(roidAngle)) + (roid[4].y * cos(roidAngle))+lst->p.y);
   int rXrotate6 = int((roid[5].x * cos(roidAngle)) - (roid[5].y * sin(roidAngle))+lst->p.x);
   int rYrotate6 = int((roid[5].x * sin(roidAngle)) + (roid[5].y * cos(roidAngle))+lst->p.y);
	 int rXrotate7 = int((roid[6].x * cos(roidAngle)) - (roid[6].y * sin(roidAngle))+lst->p.x);
   int rYrotate7 = int((roid[6].x * sin(roidAngle)) + (roid[6].y * cos(roidAngle))+lst->p.y);
   int rXrotate8 = int((roid[7].x * cos(roidAngle)) - (roid[7].y * sin(roidAngle))+lst->p.x);
   int rYrotate8 = int((roid[7].x * sin(roidAngle)) + (roid[7].y * cos(roidAngle))+lst->p.y);
   int rXrotate9 = int((roid[8].x * cos(roidAngle)) - (roid[8].y * sin(roidAngle))+lst->p.x);
   int rYrotate9 = int((roid[8].x * sin(roidAngle)) + (roid[8].y * cos(roidAngle))+lst->p.y);
	 int rXrotate10 = int((roid[9].x * cos(roidAngle)) - (roid[9].y * sin(roidAngle))+lst->p.x);
   int rYrotate10 = int((roid[9].x * sin(roidAngle)) + (roid[9].y * cos(roidAngle))+lst->p.y);
   int rXrotate11 = int((roid[10].x * cos(roidAngle)) - (roid[10].y * sin(roidAngle))+lst->p.x);
   int rYrotate11 = int((roid[10].x * sin(roidAngle)) + (roid[10].y * cos(roidAngle))+lst->p.y);
   int rXrotate12 = int((roid[11].x * cos(roidAngle)) - (roid[11].y * sin(roidAngle))+lst->p.x);
   int rYrotate12 = int((roid[11].x * sin(roidAngle)) + (roid[11].y * cos(roidAngle))+lst->p.y);
	 int rXrotate13 = int((roid[12].x * cos(roidAngle)) - (roid[12].y * sin(roidAngle))+lst->p.x);
   int rYrotate13 = int((roid[12].x * sin(roidAngle)) + (roid[12].y * cos(roidAngle))+lst->p.y);
   int rXrotate14 = int((roid[13].x * cos(roidAngle)) - (roid[13].y * sin(roidAngle))+lst->p.x);
   int rYrotate14 = int((roid[13].x * sin(roidAngle)) + (roid[13].y * cos(roidAngle))+lst->p.y);
		
		graphics->drawLine(rXrotate1, rYrotate1, rXrotate2, rYrotate2, GREEN); // 1
		graphics->drawLine(rXrotate3, rYrotate3, rXrotate4, rYrotate4, GREEN); // 2
		graphics->drawLine(rXrotate5, rYrotate5, rXrotate6, rYrotate6, GREEN); // 3
		graphics->drawLine(rXrotate7, rYrotate7, rXrotate8, rYrotate8, GREEN); // 4
		graphics->drawLine(rXrotate9, rYrotate9, rXrotate10, rYrotate10, GREEN); // 5
		graphics->drawLine(rXrotate11, rYrotate11, rXrotate12, rYrotate12, GREEN); // 6
		graphics->drawLine(rXrotate13, rYrotate13, rXrotate14, rYrotate14, GREEN); // 7
	} else {
		//small red style asteroid
   float roidAngle = radians(lst->heading);
	 int rXrotate1 = int((roid2[0].x * cos(roidAngle)) - (roid2[0].y * sin(roidAngle))+lst->p.x);
   int rYrotate1 = int((roid2[0].x * sin(roidAngle)) + (roid2[0].y * cos(roidAngle))+lst->p.y);
   int rXrotate2 = int((roid2[1].x * cos(roidAngle)) - (roid2[1].y * sin(roidAngle))+lst->p.x);
   int rYrotate2 = int((roid2[1].x * sin(roidAngle)) + (roid2[1].y * cos(roidAngle))+lst->p.y);
   int rXrotate3 = int((roid2[2].x * cos(roidAngle)) - (roid2[2].y * sin(roidAngle))+lst->p.x);
   int rYrotate3 = int((roid2[2].x * sin(roidAngle)) + (roid2[2].y * cos(roidAngle))+lst->p.y);
	 int rXrotate4 = int((roid2[3].x * cos(roidAngle)) - (roid2[3].y * sin(roidAngle))+lst->p.x);
   int rYrotate4 = int((roid2[3].x * sin(roidAngle)) + (roid2[3].y * cos(roidAngle))+lst->p.y);
   int rXrotate5 = int((roid2[4].x * cos(roidAngle)) - (roid2[4].y * sin(roidAngle))+lst->p.x);
   int rYrotate5 = int((roid2[4].x * sin(roidAngle)) + (roid2[4].y * cos(roidAngle))+lst->p.y);
   int rXrotate6 = int((roid2[5].x * cos(roidAngle)) - (roid2[5].y * sin(roidAngle))+lst->p.x);
   int rYrotate6 = int((roid2[5].x * sin(roidAngle)) + (roid2[5].y * cos(roidAngle))+lst->p.y);
	 int rXrotate7 = int((roid2[6].x * cos(roidAngle)) - (roid2[6].y * sin(roidAngle))+lst->p.x);
   int rYrotate7 = int((roid2[6].x * sin(roidAngle)) + (roid2[6].y * cos(roidAngle))+lst->p.y);
   int rXrotate8 = int((roid2[7].x * cos(roidAngle)) - (roid2[7].y * sin(roidAngle))+lst->p.x);
   int rYrotate8 = int((roid2[7].x * sin(roidAngle)) + (roid2[7].y * cos(roidAngle))+lst->p.y);
   int rXrotate9 = int((roid2[8].x * cos(roidAngle)) - (roid2[8].y * sin(roidAngle))+lst->p.x);
   int rYrotate9 = int((roid2[8].x * sin(roidAngle)) + (roid2[8].y * cos(roidAngle))+lst->p.y);
	 int rXrotate10 = int((roid2[9].x * cos(roidAngle)) - (roid2[9].y * sin(roidAngle))+lst->p.x);
   int rYrotate10 = int((roid2[9].x * sin(roidAngle)) + (roid2[9].y * cos(roidAngle))+lst->p.y);
   int rXrotate11 = int((roid2[10].x * cos(roidAngle)) - (roid2[10].y * sin(roidAngle))+lst->p.x);
   int rYrotate11 = int((roid2[10].x * sin(roidAngle)) + (roid2[10].y * cos(roidAngle))+lst->p.y);
   int rXrotate12 = int((roid2[11].x * cos(roidAngle)) - (roid2[11].y * sin(roidAngle))+lst->p.x);
   int rYrotate12 = int((roid2[11].x * sin(roidAngle)) + (roid2[11].y * cos(roidAngle))+lst->p.y);
	 int rXrotate13 = int((roid2[12].x * cos(roidAngle)) - (roid2[12].y * sin(roidAngle))+lst->p.x);
   int rYrotate13 = int((roid2[12].x * sin(roidAngle)) + (roid2[12].y * cos(roidAngle))+lst->p.y);
   int rXrotate14 = int((roid2[13].x * cos(roidAngle)) - (roid2[13].y * sin(roidAngle))+lst->p.x);
   int rYrotate14 = int((roid2[13].x * sin(roidAngle)) + (roid2[13].y * cos(roidAngle))+lst->p.y);
		
		graphics->drawLine(rXrotate1, rYrotate1, rXrotate2, rYrotate2, RED); // 1
		graphics->drawLine(rXrotate3, rYrotate3, rXrotate4, rYrotate4, RED); // 2
		graphics->drawLine(rXrotate5, rYrotate5, rXrotate6, rYrotate6, RED); // 3
		graphics->drawLine(rXrotate7, rYrotate7, rXrotate8, rYrotate8, RED); // 4
		graphics->drawLine(rXrotate9, rYrotate9, rXrotate10, rYrotate10, RED); // 5
		graphics->drawLine(rXrotate11, rYrotate11, rXrotate12, rYrotate12, RED); // 6
		graphics->drawLine(rXrotate13, rYrotate13, rXrotate14, rYrotate14, RED); // 7
	}		
		lst = lst->next;
    }
}

//draw player ship
void drawship(ship player) {			
	 float angle = radians(player.heading);		
	 int nXrotate1 = int((shape[0].x * cos(angle)) - (shape[0].y * sin(angle))+player.p.x);
   int nYrotate1 = int((shape[0].x * sin(angle)) + (shape[0].y * cos(angle))+player.p.y);
   int nXrotate2 = int((shape[1].x * cos(angle)) - (shape[1].y * sin(angle))+player.p.x);
   int nYrotate2 = int((shape[1].x * sin(angle)) + (shape[1].y * cos(angle))+player.p.y);
   int nXrotate3 = int((shape[2].x * cos(angle)) - (shape[2].y * sin(angle))+player.p.x);
   int nYrotate3 = int((shape[2].x * sin(angle)) + (shape[2].y * cos(angle))+player.p.y);

	//ship is a basic white outline of a triangle
   graphics->drawTriangle(nXrotate1,nYrotate1,nXrotate2, nYrotate2, nXrotate3, nYrotate3, WHITE); 	
}

//draw to screen
void draw(void)
{
	if (mainmenu == true)
	{
		draw_start();
	}else if (gameover == true) {
		draw_end();
	}else if(mainmenu == false && gameover == false) {
  graphics->fillScreen(background);
	draw_overlay(score,elapsed_time,lives,shield);
	drawshield();
	drawmissiles(missileActive);
	drawasteroids(asteroidActive);
	drawship(player);
	}
	swap_DBuffer();
}
