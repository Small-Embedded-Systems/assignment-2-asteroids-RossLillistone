/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	Model.cpp
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "model.h"
#include "utils.h"
#include "asteroids.h"

/* Static Variables */
static const int missileHeapsize = 4; // max number of missiles that can be fired at a time
static const int asteroidHeapsize = 10; // max number of asteroids at any one time
static missile_t missileheap[missileHeapsize];
static asteroid_t asteroidheap[asteroidHeapsize];
static missile_t *missilefreenodes;
static asteroid_t *asteroidfreenodes;

/* Static Methods */
static void missilevalues(struct missile *r);
static void asteroidvalues(struct asteroid *r);
static void shipDie(void); 
static void checkShipAsteroids(struct asteroid *c);
static void checkMissilesAsteroids(struct missile *a, struct asteroid *b);
static void update_missiles(struct missile *l);
static void update_asteroids(struct asteroid *l);
static void move_ship(void);
static void applyFriction();
static void shieldCharge(void);
static void time(void);

/* Static Method Variables */
static int 		timeAlive;
static float 	angle;
static int 		shieldfix;
static float 	thrustunit;
static float 	newVX;
static float 	newVY;

/*initially no active asteroids or missiles*/
struct missile *missileActive = NULL;
struct asteroid *asteroidActive = NULL;

/*creates a node heap for the created missiles.*/
void initialiseMissileHeap(void){
    int n;
    for( n=0 ; n<(missileHeapsize-1) ; n++) {
        missileheap[n].next = &missileheap[n+1];
    }
    missileheap[n].next = NULL;
    missilefreenodes = &missileheap[0];
}
/*creates a node heap for the created asteroids.*/
void initialiseAsteroidHeap(void){
    int n;
    for( n=0 ; n<(asteroidHeapsize-1) ; n++) {
        asteroidheap[n].next = &asteroidheap[n+1];
    }
    asteroidheap[n].next = NULL;
    asteroidfreenodes = &asteroidheap[0];
}

/*allocate a node to a created missile object*/
missile_t *missileAllocnode(void){
    missile_t *node = NULL;
    if( missilefreenodes ) {
        node = missilefreenodes;
        missilefreenodes = missilefreenodes->next;
    }
    return node;
}

/*allocate a node to a created asteroid object*/
asteroid_t *asteroidAllocnode(void){
    asteroid_t *node = NULL;
    if( asteroidfreenodes ) {
        node = asteroidfreenodes;
        asteroidfreenodes = asteroidfreenodes->next;
    }
    return node;
}

/*assign an available node for a missile*/
void missileFreenode(missile_t *n){
    n->next = missilefreenodes;
    missilefreenodes = n;
}

/*assign an available node for an asteroid*/
void asteroidFreenode(asteroid_t *n){
    n->next = asteroidfreenodes;
    asteroidfreenodes = n;
}

/*spawn created missiles from the location of the player ship*/
void missilevalues(struct missile *r){
	float angle = radians(player.heading);
    r->p.x = player.p.x;
    r->p.y = player.p.y;
    r->v.x = 200*(cos(angle));
    r->v.y = 200*(sin(angle));
    r->alive = true;
}

/*spawn created asteroids on the screen*/
void asteroidvalues(struct asteroid *r){
   	r->heading = randrange(1,350);
	  r->p.x = randrange(5,475)* cos(radians(r->heading));
    r->p.y = randrange(5,255)* -sin(radians(r->heading));
    r->v.x = randrange(-50,50); 
    r->v.y= randrange(-50,50);
	  r->type= randrange(1,6);  // this is used to randomise the type of asteroid spawned -- large green or small red
    r->alive = true;
}

/*spawn missiles if node available*/
void missileSystem(void){
        struct missile *missilepark = missileAllocnode();
        if(missilepark) {
            missilepark->next = missileActive;
            missileActive = missilepark;
            missilevalues(missilepark);
        }
}

/*spawn asteroid if node available*/
void asteroidSystem(void){
        struct asteroid *asteroidpark = asteroidAllocnode();
        if(asteroidpark) {
            asteroidpark->next = asteroidActive;
            asteroidActive = asteroidpark;
            asteroidvalues(asteroidpark);
        }
}
/* decrease number of player lives and replenish shields on asteroid hit with 0 shield strength*/
void shipDie(void) 
{
	if (shield == 0) {
		lives = lives-1;
		shield = 5;}
	if (lives == 0) {
		gameover = true;		
	}
}

/*Detect collision between ship and asteroid, modify asteroid->alive 
variable to false on objects that have confirmed collisions
*/
void checkShipAsteroids(struct asteroid *c){
    for( ; c ; c = c->next ) {
        if(c->p.x<(player.p.x+10) && c->p.x>(player.p.x-10)){
          if(c->p.y<(player.p.y+10) && c->p.y>(player.p.y-10)){
						c->alive = false;
						shipDie();
						shield = shield -1;
						shieldfix = 0;
				}
			}
		}
}

/*Detect collision between missile and asteroid, modify asteroid->alive 
and missile-alive variables to false on objects that have confirmed collisions
*/
void checkMissilesAsteroids(struct missile *a, struct asteroid *b) {
  for(; a ; a = a->next){
    for(; b ; b = b->next){
      if(a->p.x > (b->p.x-10) && a->p.x < (b->p.x+10)){
        if(a->p.y > (b->p.y-10) && a->p.y < (b->p.y+10)){
          a->alive=false;
					b->alive=false;
       }
     }

   }
 } 
}

/*check list for any missiles with ->alive set as false and discard
  while travering list set alive status to false on any out of bounds objects
*/
 void update_missiles(struct missile *l){
    for( ; l ; l = l->next ) {
        l->p.x += l->v.x * Dt;
        l->p.y += l->v.y * Dt;
        if( l->p.x<0 || 480<l->p.x ) l->alive=false;
        if( l->p.y<10 || 260<l->p.y ) l->alive=false;
        if( !l->next->alive) {
            struct missile *discard = l->next;
            l->next = l->next->next;
            missileFreenode(discard);
        }
    }
}

/*check list for any asteroids with ->alive set as false and discard
  while travering list set alive status to false on any out of bounds objects
*/
void update_asteroids(struct asteroid *l){
    for( ; l ; l = l->next ) {
        l->p.x += l->v.x * Dt;
        l->p.y += l->v.y * Dt;
        if( l->p.x<-20 || 500<l->p.x ) l->alive=false;;
        if( l->p.y<25 || 280<l->p.y ) l->alive=false;
        if( !l->next->alive) {
            struct asteroid *discard = l->next;
            l->next = l->next->next;
            asteroidFreenode(discard);
        }
    }
}

/*ship movement*/
void move_ship(void){
	float angle = radians(player.heading);
	player.p.x += player.v.x;
	player.p.y += player.v.y;

//wrap the ship movement around the playable are of the screen.	
	if (player.p.x<0) {player.p.x=480;} 
	else if (player.p.x>480) {player.p.x=0;}
	if (player.p.y<37) {player.p.y=240;} 
	else if (player.p.y>240) {player.p.y=37;}
}

//generate directional thrust
void incThrust(void)
{
    float thrustUnit = thrustUnit + 0.2;
    float newVX, newVY = 0.0; 
	
    newVX = thrustUnit * cos(radians(player.heading));
    newVY = thrustUnit * sin(radians(player.heading)); 

    player.v.x += newVX;
    player.v.y += newVY;
}

//generate braking
void decThrust(void)
{
    float thrustUnit = thrustUnit - 0.1;
    float newVX, newVY = 0.0; 
	
    newVX = thrustUnit * cos(radians(player.heading));
    newVY = thrustUnit * sin(radians(player.heading)); 

    player.v.x += newVX;
    player.v.y += newVY;
}

//cause ship to automatically slow down when coasting -- under no active thrust
void applyFriction()
{
	player.v.x *= 0.99;
	player.v.y *= 0.99;
}

//function to recharge shields, everytime shieldfix = 1000 increase shield level by 1
void shieldCharge(void){
	if (shield >= 0 && shield <=4 && shieldfix <= 1000) {
	shieldfix = shieldfix + 1;
	}else if (shield >= 0 && shield <=4 && shieldfix >= 1000) {
		shieldfix = 0;
		shield = shield + 1;
	}
}

/*time spent playing the game; the greater it is, the higher the user scores*/
void time(void){
	timeAlive = (int)((score/100)+0.5);
}

/*physics invoked by all.cpp*/
void physics(void){
	checkShipAsteroids(asteroidActive);
	checkMissilesAsteroids(missileActive, asteroidActive);
	update_missiles(missileActive);
	update_asteroids(asteroidActive);
	asteroidSystem();
	move_ship();
	applyFriction();
	shieldCharge();
	time();
	score++;
}

