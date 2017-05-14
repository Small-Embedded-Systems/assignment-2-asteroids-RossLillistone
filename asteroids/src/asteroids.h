/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	asteroids.h
*/

/* Game state */
extern float elapsed_time; 					/* time this ship has been active */
extern int   score;        					/* total score so far */
extern int   lives;        					/* lives remaining */
extern int	 shield;			 					/* shield level remaining 5(MAX) - 1(MIN)  0 = no shield */
extern bool  paused;				 				/*todo -- set button on back of board to use as pause*/
extern bool  mainmenu;			 				/* true/false value used to detect whether to show main menu screen*/
extern bool  gameover;			 				/* true/false value used to detect whether to show game over screen */

extern struct ship player; 					/* create player using ship defenition */
extern struct asteroids *asteroids; /* array of rocks / pointer to linked-list */
extern struct missile *shots;  			/* array of missiles / pointer to linked-list */

extern const float Dt; 							/* Time step for physics, needed for consistent motion */

