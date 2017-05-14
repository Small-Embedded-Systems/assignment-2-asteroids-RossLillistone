/*
	Asteroids Assignment - Small Embedded Systems
  Ross Lillistone 
	W15021733
	Model.h
*/

struct point {
    float x,y;
};
typedef struct point coordinate_t;
typedef struct point vector_t;

/* Some initial struct types if you want to use them */
struct ship {
	  float heading;
    coordinate_t p;
    vector_t     v;
};

/* initial struts for building linked lists */
typedef struct asteroid {
  float heading;  
	coordinate_t p;
	vector_t     v;
	bool alive;
	int type;
  struct asteroid *next;
}asteroid_t;

typedef struct missile {
	  float heading;
    coordinate_t p;
	  vector_t     v;
	  bool alive;
    struct missile *next;
}missile_t;


extern struct missile *missileActive;
extern struct asteroid *asteroidActive;

/*Methods contained in model.cpp called by other files */
void initialiseMissileHeap(void);
void initialiseAsteroidHeap(void);
missile_t *missileAllocnode(void);
asteroid_t *asteroidAllocnode(void);
void missileFreenode(missile_t *n);
void asteroidFreenode(asteroid_t *n);
void missileSystem(void);
void asteroidSystem(void);
void incThrust(void);
void decThrust(void);
void physics(void);

