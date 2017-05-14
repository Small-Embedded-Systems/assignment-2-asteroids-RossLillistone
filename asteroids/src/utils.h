/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	utils.h
*/

/* utility functions */

/* some numerical helper functions */
float norm(float value, float min, float max);
float lerp(float min, float max, float value);
float map(float value, float lower, float upper, float min, float max);

/* make a 16bit colour */
typedef uint16_t colour_t;

#define rgb(r,g,b) ((uint16_t)(31 * r /255)<<11 | (uint16_t)(63 * g /255)<<5 | (uint16_t)(31 * b /255))

int randrange(int from, int to);

float dist(coordinate_t p1, coordinate_t p2);

extern const float pi;
float radians(float degrees);
