/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	view.h
*/

/* Asteroids View */

/* support double buffering */
void init_DBuffer(void);
void swap_DBuffer(void);

void draw(void);
void drawmissiles(struct missile *lst);
void drawasteroids(struct asteroid *lst);
void bitmap(void);
