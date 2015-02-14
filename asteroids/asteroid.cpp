#ifdef _M_IX86
#include <windows.h>
#else
#include <stream.h>
#endif

#include <string.h>
#include <glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#ifndef PI
  #define PI 3.14159265358979323846
#endif
 
#define RAD2DEG 180.0/PI
#define DEG2RAD PI/180.0
#define SHOT_MAX 4
#define SHOT_SPEED 2.25
#define ASTEROID_MAX 8
#define START_ASTEROIDS 3
#define ASTEROID_RADIUS 1
#define SHOT_MAX_TIME 40
#define MAX_EXPLOSION_POINTS 20
#define ALIEN_TIME_TILL_SPAWN 400
#define SMALL_ALIEN_SCORE_SPAWN 5000
#define ALIEN_SHOT_MAX 1
#define ALIEN_SHOT_SPEED 1
#define SMALL_ALIEN_SHOT_SPEED 2
#define ALIEN_PROB_ASTEROID 40
#define ALIEN_PROB_PLAYER 20
#define ALIEN_PROB_RANDOM 40
#define SMALL_ALIEN_PROB_ASTEROID 20
#define SMALL_ALIEN_PROB_PLAYER 75
#define SMALL_ALIEN_PROB_RANDOM 5
#define ALIEN_SPEED 0.2
#define SMALL_ALIEN_SPEED 0.35
#define ALIEN_UPDATE 30
#define EXPLOSION_UPDATE 0.1
#define START_LIVES 5
#define SCORE_LARGE_ASTEROID 20
#define SCORE_MED_ASTEROID 50
#define SCORE_SMALL_ASTEROID 100
#define SCORE_LARGE_ALIEN 200
#define SCORE_SMALL_ALIEN 1000
#define BONUS_SHIP_MULTIPLE 3000

/*--------------------------------------------------------------------------*/
/*  Asteroids.cpp														    */
/*		This is my version of the game Asteroids. The player controls		*/
/*		the ship using the following keys: 'x' to accelerate, 'z' to		*/
/*		shoot, left arrow key to turn left, and right arrow key to turn		*/
/*		right.  The player starts out with 3 lives (including the one		*/
/*		they are currently using), and get a new life every					*/
/*		BONUS_SHIP_MULTIPLE	points.											*/
/*																			*/
/*		The objective of the game is to get as many points as				*/
/*		possible by shooting asteroids and aliens, with the point			*/
/*		allocation defined above.  The large alien (the easier of the		*/
/*		two) spawns at a specific time in each screen.  The small alien		*/
/*		(the faster and more difficult one) spawns whenever the player's	*/
/*		score reaches the SMALL_ALIEN_SCORE_SPAWN number.					*/
/*																			*/
/*		To start the game, simply run this program and right-click on the	*/
/*		screen to select to start a new game.								*/
/*																		    */
/*--Modification History:---------------------------------------------------*/
/*  When:			Who:					Comments:					    */
/*  27-November-14	Kevin D. Ashley			Complete Game					*/
/*--------------------------------------------------------------------------*/

  //  Module global variables

typedef enum {
  TRANSLATE,
  ROTATE_X,
  ROTATE_Y,
  ROTATE_Z
} mode;
typedef struct {
  int active, shot;
  float x, y, velX, velY, theta;
  int age;
  int asteroidsShot, aliensShot, shotsFired;
  double accuracy;
  float explosionScale;
} Player;
typedef struct {
  int active, done;
  float x, y, velX, velY, theta, speedY, dirY;
  int shot, age;
  float explosionScale;
} Alien;
typedef struct {
  int active, done;
  float x, y, velX, velY, theta, speedY, dirY;
  int shot, age;
  float explosionScale;
} SmallAlien;
typedef struct {
  int isOnScreen;
  float x, y, velX, velY;
  int time;
} Shot;
typedef struct {
  int active;
  float x, y, velX, velY;
  int age;
} AlienShot;
typedef struct {
  int active;
  float x, y, velX, velY;
  int theta[ 3 ];
  float vertices[ 18 ][ 3 ] ;
  int shot;
  float explosionScale;
} Asteroid;
typedef struct {
  int active, age;
  float x, y, velX, velY;
} Explosion;

//  Module global prototypes
void	randomize_asteroid( void );
void	display_large_asteroid( Asteroid *a );
void	display_med_asteroid( Asteroid *a );
void	display_small_asteroid( Asteroid *a );
void	display_player( Player *p );
void	display_shot( Shot *s );
void	display_alien( Alien *a );
void	display_small_alien( SmallAlien *a );
void	display_alien_shot( Shot *s );
void	display_explosion( int, int, int );
void	display_score( void );
void	display_statistics( Player *p );
void	display_lives( void );
void	display_game_over( void );
void	scale_game_over( void );
void	display_all( void );
float*	compute_normal( float[], float[], float[] );
void	handle_menu( int );
void	keyboard_special( int, int, int );
void	keyboard_special_release( int, int, int );
void	keyboard( unsigned char, int, int );
void	keyboard_release( unsigned char, int, int );
void	player_angles( void );
void	alien_angles( void );
void	scale_explosion( void );
void	asteroid_angles( void );
void	spawn_asteroids( void );
void	player_movement( void );
void	alien_movement( void );
void	small_alien_movement( void );
void	alien_shot_movement( void );
void	small_alien_shot_movenet( void );
float	alien_shot_direction( void );
float	small_alien_shot_direction( void );
void	shot_movemnet( void );
void	asteroid_movement( void );
void	wrap_player( void );
void	wrap_alien( void );
void	wrap_small_alien( void );
void	wrap_asteroids( void );
void	wrap_shots( void );
void	display_player_boundary( Player *p );
void	display_asteroid_boundary( Asteroid *a );
void	shot_asteroid_collision( void );
void	player_asteroid_collision( void );
void	player_alien_shot_collision( void );
void	shot_alien_collision( void );
void	shot_small_alien_collision( void );
void	player_alien_collision( void );
void	reset_player( void );
void	reset_alien( void );
void	reset_small_alien( void );
void	start_screen( void );
void	reset_game( void );
void	new_screen( void );
void	increment_score( int );
void	movement( void );
void	wrap_screen( void );
void	collisions( void );

int    btn[ 3 ] = { 0 };			// Current button state
mode   cur_mode = TRANSLATE;		// Current mouse mode
int    mouse_x, mouse_y;			// Current mouse position

float angle, player_angle, alien_angle, asteroid_angle = 0.0;	// Current angles to rotate at
int player_rot = 8;												// How fast the player's ship rotates
boolean rotateLeft, rotateRight, accelerating = false;
Player player;
Alien alien;
SmallAlien smallAlien;
//float explosion_scale = 0.0;
int windowX = 800;
int windowY = 800;
int boundX = 25;
int boundY = 25;
static int shooting = 0;
static int alienShooting = 0;
static int smallAlienShooting = 0;
static float rayLength = 0.5;
static Shot shots[ SHOT_MAX ];
static AlienShot alienShots[ ALIEN_SHOT_MAX ];
static AlienShot smallAlienShots[ ALIEN_SHOT_MAX ];
static int startAsteroids = 5; // Less than ASTEROID_MAX
static int currentAsteroids = 5;
static Asteroid asteroids[ ASTEROID_MAX ];
static Asteroid med_asteroids[ ASTEROID_MAX * 2 ];
static Asteroid small_asteroids[ ASTEROID_MAX * 4 ];
float largeAsteroidScale = 2.5;
float medAsteroidScale = 2;
float smallAsteroidScale = 1.5;

static int screenNum = 1;
static int score = 0;
static int lives = START_LIVES;
static float gameOverScale = 0;
static int gameOverStats = 0;

static int startGame = 0;

char str[6];
void * font = GLUT_BITMAP_9_BY_15;

float  v[ 18 ][ 3 ] = {		// Asteroid vertex positions
    { 0,  1,  0 },			//0
	{ -0.5,  0.5,  0.5 },	//1
	{ 0.5,  0.5,  0.5 },	//2
	{ 0.5,  0.5,  -0.5 },	//3
	{ -0.5,  0.5,  -0.5 },	//4
	{ -1,  0,  -1 },		//5
	{ -1,  0,  0 },			//6
	{ -1,  0,  1 },			//7
	{ 0,  0,  1 },			//8
	{ 1,  0,  1 },			//9
	{ 1,  0,  0 },			//10
	{ 1,  0,  -1 },			//11
	{ 0,  0,  -1 },			//12
	{ -0.5,  -0.5,  -0.5 },	//13
	{ -0.5,  -0.5,  0.5 },	//14
	{ 0.5,  -0.5,  0.5 },	//15
	{ 0.5,  -0.5,  -0.5 },	//16
	{ 0,  -1,  0 }			//17
};

float playerOutlineVerticies[ 17 ][ 3 ] = {		// Vertex positions
		{ 0.4,  0,  0 },			//0
		{ 0,  1.5,  0 },			//1
		{ -0.4,  0,  0 },			//2
		{ -0.5,  1,  0 },			//3
		{ -0.75,  0,  0 },			//4
		{ -1.5,  -1,  0 },			//5
		{ -0.75,  -0.75,  0 },		//6
		{ -0.4,  -1,  0 },			//7
		{ 0,  -0.5,  0 },			//8
		{ 0.4,  -1,  0 },			//9
		{ 0.75,  0,  0 },			//10
		{ 0.75,  -0.75,  0 },		//11
		{ 1.5,  -1,  0 },			//12
		{ 0.5,  1,  0 },			//13
		{ 0,  0.4,  0.5 },			//14
		{ -0.6,  -0.25,  0.25 },	//15
		{ 0.6,  -0.25,  0.25 }		//16
};

int playerOutlineFace[ 14 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 10, 13, 0 };

float alienOutlineVerticies[ 12 ][ 3 ] = {
		{ 2, 0, 0 },
		{ 2, 1, 0 },
		{ 1, 1, 0 },
		{ 0, 2, 0 },
		{ -1, 1, 0 },
		{ -2, 1, 0 },
		{ -2, 0, 0 },
		{ -2, -1, 0 },
		{ -1, -1, 0 },
		{ 0, -2, 0 },
		{ 1, -1, 0 },
		{ 2, -1, 0 }
};

float smallAlienOutlineVerticies[ 12 ][ 3 ] = {
		{ 1, 0, 0 },
		{ 1, 0.5, 0 },
		{ 0.5, 0.5, 0 },
		{ 0, 1, 0 },
		{ -0.5, 0.5, 0 },
		{ -1, 0.5, 0 },
		{ -1, 0, 0 },
		{ -1, -0.5, 0 },
		{ -0.5, -0.5, 0 },
		{ 0, -1, 0 },
		{ 0.5, -0.5, 0 },
		{ 1, -0.5, 0 }
};

int alienOutlineFace[ 12 ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

void randomize_asteroid()
{
	// Normalize all the points with respect to the center
	float dx, dy, dz = 0;
	float r = 1;
	srand( (unsigned)time( NULL ) );
	for ( int i = 0; i < 17; i++ )
	{
		dx = v[i][0];
		dy = v[i][1];
		dz = v[i][2];
		dx = dx * (r / sqrt( powf( dx, 2.0 ) + powf( dy, 2.0 ) + powf( dz, 2.0 ) ));
		dy = dy * (r / sqrt( powf( dx, 2.0 ) + powf( dy, 2.0 ) + powf( dz, 2.0 ) ));
		dz = dz * (r / sqrt( powf( dx, 2.0 ) + powf( dy, 2.0 ) + powf( dz, 2.0 ) ));
		v[i][0] = dx;
		v[i][1] = dy;
		v[i][2] = dz;

		// Randomize the spacing of the points from the center
		v[i][0] += 0.1*( rand() % 4 );
		v[i][1] += 0.1*( rand() % 4 );
		v[i][2] += 0.1*( rand() % 4 );
	}
}

void display_large_asteroid( Asteroid *a )
//  This routine displays an asteroid
{
	int  f[ 32 ][ 3 ] = {  // Asteroid face vertex indices
		{ 0, 1, 2 },
		{ 0, 2, 3 },
		{ 0, 3, 4 },
		{ 0, 4, 1 },
		{ 1, 7, 8 },
		{ 1, 8, 2 },
		{ 2, 8, 9 },
		{ 2, 9, 10 },
		{ 2, 10, 3 },
		{ 3, 10, 11 },
		{ 3, 11, 12 },
		{ 3, 12, 4 },
		{ 4, 12, 5 },
		{ 4, 5, 6 },
		{ 4, 6, 1 },
		{ 1, 6, 7 },
		{ 7, 14, 8 }, // Start of lower half
		{ 8, 14, 15 },
		{ 8, 15, 9 },
		{ 9, 15, 10 },
		{ 10, 15, 16 },
		{ 10, 16, 11 },
		{ 11, 16, 12 },
		{ 12, 16, 13 },
		{ 12, 13, 5 },
		{ 5, 13, 6 },
		{ 6, 13, 14 },
		{ 7, 6, 14 },
		{ 17, 15, 14 },
		{ 17, 16, 15 },
		{ 17, 13, 16 },
		{ 17, 14, 13 }
	};
	float v1[ 18 ][ 3 ];
	for ( int i = 0; i < 18; i++ ) {
		v1[ i ][ 0 ] = v[ i ][ 0 ] * largeAsteroidScale;
		v1[ i ][ 1 ] = v[ i ][ 1 ] * largeAsteroidScale;
		v1[ i ][ 2 ] = v[ i ][ 2 ] * largeAsteroidScale;
	}
	float n[ 32 ][ 3 ];	// Face normals
	for ( int i = 0; i < 32; i++ ) {
		n[ i ][ 0 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 0 ];
		n[ i ][ 1 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 1 ];
		n[ i ][ 2 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 2 ];
	}
	float mat[ 1 ][ 4 ] = { 0, 0, 1, 0 }; // Polygon material

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
	glPushMatrix();

	glTranslatef( a->x, a->y, 0 );

	glRotatef( asteroid_angle, a->theta[ 0 ], a->theta[ 1 ], 1 );

	for ( int i = 0; i < 32; i++ ) {		// For all faces
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
		glBegin( GL_POLYGON );
		// Specify face normal
		glNormal3f( n[ i ][ 0], n[ i ][ 1 ], n[ i ][ 2 ] );
		for( int j = 0; j < 3; j++ ) {		// For all face vertices
			glVertex3fv( v1[ f[ i ][ j ] ] );
		}
		glEnd();
	}

	glPopMatrix();
	glFlush();				// Flush OpenGL queue
}

void display_med_asteroid( Asteroid *a )
//  This routine displays an asteroid
{
	int  f[ 32 ][ 3 ] = {  // Asteroid face vertex indices
		{ 0, 1, 2 },
		{ 0, 2, 3 },
		{ 0, 3, 4 },
		{ 0, 4, 1 },
		{ 1, 7, 8 },
		{ 1, 8, 2 },
		{ 2, 8, 9 },
		{ 2, 9, 10 },
		{ 2, 10, 3 },
		{ 3, 10, 11 },
		{ 3, 11, 12 },
		{ 3, 12, 4 },
		{ 4, 12, 5 },
		{ 4, 5, 6 },
		{ 4, 6, 1 },
		{ 1, 6, 7 },
		{ 7, 14, 8 }, // Start of lower half
		{ 8, 14, 15 },
		{ 8, 15, 9 },
		{ 9, 15, 10 },
		{ 10, 15, 16 },
		{ 10, 16, 11 },
		{ 11, 16, 12 },
		{ 12, 16, 13 },
		{ 12, 13, 5 },
		{ 5, 13, 6 },
		{ 6, 13, 14 },
		{ 7, 6, 14 },
		{ 17, 15, 14 },
		{ 17, 16, 15 },
		{ 17, 13, 16 },
		{ 17, 14, 13 }
	};
	float v1[ 18 ][ 3 ];
	for ( int i = 0; i < 18; i++ ) {
		v1[ i ][ 0 ] = v[ i ][ 0 ] * medAsteroidScale;
		v1[ i ][ 1 ] = v[ i ][ 1 ] * medAsteroidScale;
		v1[ i ][ 2 ] = v[ i ][ 2 ] * medAsteroidScale;
	}
	float n[ 32 ][ 3 ];	// Face normals
	for ( int i = 0; i < 32; i++ ) {
		n[ i ][ 0 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 0 ];
		n[ i ][ 1 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 1 ];
		n[ i ][ 2 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 2 ];
	}
	float mat[ 1 ][ 4 ] = { 0, 0, 1, 0 }; // Polygon material

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
	glPushMatrix();

	glTranslatef( a->x, a->y, 0 );

	glRotatef( asteroid_angle, a->theta[ 0 ], a->theta[ 1 ], 1 );

	for ( int i = 0; i < 32; i++ ) {		// For all faces
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
		glBegin( GL_POLYGON );
		// Specify face normal
		glNormal3f( n[ i ][ 0], n[ i ][ 1 ], n[ i ][ 2 ] );
		for( int j = 0; j < 3; j++ ) {		// For all face vertices
			glVertex3fv( v1[ f[ i ][ j ] ] );
		}
		glEnd();
	}

	glPopMatrix();
	glFlush();				// Flush OpenGL queue
}

void display_small_asteroid( Asteroid *a )
//  This routine displays an asteroid
{
	int  f[ 32 ][ 3 ] = {  // Asteroid face vertex indices
		{ 0, 1, 2 },
		{ 0, 2, 3 },
		{ 0, 3, 4 },
		{ 0, 4, 1 },
		{ 1, 7, 8 },
		{ 1, 8, 2 },
		{ 2, 8, 9 },
		{ 2, 9, 10 },
		{ 2, 10, 3 },
		{ 3, 10, 11 },
		{ 3, 11, 12 },
		{ 3, 12, 4 },
		{ 4, 12, 5 },
		{ 4, 5, 6 },
		{ 4, 6, 1 },
		{ 1, 6, 7 },
		{ 7, 14, 8 }, // Start of lower half
		{ 8, 14, 15 },
		{ 8, 15, 9 },
		{ 9, 15, 10 },
		{ 10, 15, 16 },
		{ 10, 16, 11 },
		{ 11, 16, 12 },
		{ 12, 16, 13 },
		{ 12, 13, 5 },
		{ 5, 13, 6 },
		{ 6, 13, 14 },
		{ 7, 6, 14 },
		{ 17, 15, 14 },
		{ 17, 16, 15 },
		{ 17, 13, 16 },
		{ 17, 14, 13 }
	};
	float v1[ 18 ][ 3 ];
	for ( int i = 0; i < 18; i++ ) {
		v1[ i ][ 0 ] = v[ i ][ 0 ] * smallAsteroidScale;
		v1[ i ][ 1 ] = v[ i ][ 1 ] * smallAsteroidScale;
		v1[ i ][ 2 ] = v[ i ][ 2 ] * smallAsteroidScale;
	}
	float n[ 32 ][ 3 ];	// Face normals
	for ( int i = 0; i < 32; i++ ) {
		n[ i ][ 0 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 0 ];
		n[ i ][ 1 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 1 ];
		n[ i ][ 2 ] = compute_normal ( v1[ f[ i ][ 0 ] ], v1[ f[ i ][ 1 ] ], v1[ f[ i ][ 2 ] ] )[ 2 ];
	}
	float mat[ 1 ][ 4 ] = { 0, 0, 1, 0 }; // Polygon material

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
	glPushMatrix();

	glTranslatef( a->x, a->y, 0 );

	glRotatef( asteroid_angle, a->theta[ 0 ], a->theta[ 1 ], 1 );

	for ( int i = 0; i < 32; i++ ) {		// For all faces
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
		glBegin( GL_POLYGON );
		// Specify face normal
		glNormal3f( n[ i ][ 0], n[ i ][ 1 ], n[ i ][ 2 ] );
		for( int j = 0; j < 3; j++ ) {		// For all face vertices
			glVertex3fv( v1[ f[ i ][ j ] ] );
		}
		glEnd();
	}

	glPopMatrix();
	glFlush();				// Flush OpenGL queue
}

void display_player( Player *p )
//  This routine displays the player ship
{
	int    f[ 30 ][ 3 ] = {		// Face vertex indices
		{ 5, 15, 4 },
		{ 6, 15, 5 },
		{ 15, 3, 4 },
		{ 6, 7, 15 },
		{ 7, 3, 15 },
		{ 2, 3, 7 },
		{ 14, 1, 2 },
		{ 2, 7, 14 },
		{ 7, 8, 14 },
		{ 9, 14, 8 },
		{ 0, 14, 9 },
		{ 1, 14, 0 },
		{ 0, 9, 13 },
		{ 9, 16, 13 },
		{ 11, 16, 9 },
		{ 12, 16, 11 },
		{ 10, 16, 12 },
		{ 13, 16, 10 },
		{ 10, 12, 11 },
		{ 0, 13, 10 },
		{ 10, 11, 0 },
		{ 11, 9, 8 },
		{ 8, 0, 11 },
		{ 2, 0, 8 },
		{ 1, 0, 2 },
		{ 2, 8, 6 },
		{ 8, 7, 6 },
		{ 6, 5, 4 },
		{ 4, 2, 6 },
		{ 3, 2, 4 }
	};	
	float  v[ 17 ][ 3 ] = {		// Vertex positions
		{ 0.4,  0,  0 },			//0
		{ 0,  1.5,  0 },			//1
		{ -0.4,  0,  0 },			//2
		{ -0.5,  1,  0 },			//3
		{ -0.75,  0,  0 },			//4
		{ -1.5,  -1,  0 },			//5
		{ -0.75,  -0.75,  0 },		//6
		{ -0.4,  -1,  0.7 },		//7
		{ 0,  -0.5,  0.45 },		//8
		{ 0.4,  -1,  0.7 },			//9
		{ 0.75,  0,  0 },			//10
		{ 0.75,  -0.75,  0 },		//11
		{ 1.5,  -1,  0 },			//12
		{ 0.5,  1,  0 },			//13
		{ 0,  0.4,  0.5 },			//14
		{ -0.6,  -0.25,  0.25 },	//15
		{ 0.6,  -0.25,  0.25 }		//16
	};	
	float  n[ 30 ][ 3 ];			// Face normals
	for ( int i = 0; i < 30; i++ ) {
		n[ i ][ 0 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 0 ];
		n[ i ][ 1 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 1 ];
		n[ i ][ 2 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 2 ];
	}
	float mat[ 1 ][ 4 ] = { 0, 1, 0, 0 };	// Face material properties

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
  
	glPushMatrix();

	glTranslatef( p->x, p->y, 0 );

	glRotatef ( RAD2DEG*p->theta, 0, 0, 1 );

	for ( int i = 0; i < 30; i++ ) {		// For all faces
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
		glBegin( GL_POLYGON );
		// Specify face normal
		glNormal3f( n[ i ][ 0], n[ i ][ 1 ], n[ i ][ 2 ] );
		for( int j = 0; j < 3; j++ ) {		// For all face vertices
			glVertex3fv( v[ f[ i ][ j ] ] );
		}
		glEnd();
	}

	glPopMatrix();
	glFlush();				// Flush OpenGL queue
}

void display_shot( Shot *s ) {
	float mat[ 1 ][ 4 ] = { 0, 10, 0, 0 };	// Face material properties
	glMatrixMode( GL_MODELVIEW );		// Stup model transformations

	glPushMatrix();

	glTranslatef( s->x, s->y, 0 );
  
	glPointSize( 10 );
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	glBegin( GL_POINTS );
	glVertex2f( 0, 0 );
	glEnd();

	glPopMatrix();
	glFlush();
}

void display_alien( Alien *a )
//  This routine displays an alien ship
{
	float mat[ 1 ][ 4 ] = { 1, 0, 0, 0 };	// Face material properties

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations

	glPushMatrix();

	glTranslatef( a->x, a->y, 0 );

	glRotatef( alien_angle, 0, 1, 0 );	// Rotate constantly

	glRotatef( 90, 1, 0, 0 );				// Turn sideways

	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	glutSolidTorus( 0.5, 2, 20, 20 );
	glRotatef( 135, 1, 1, 0 );
	glutSolidCube( 2 );

	glPopMatrix();
	glFlush();				    // Flush OpenGL queue
}

void display_small_alien( SmallAlien *a )
//  This routine displays an alien ship
{
	float mat[ 1 ][ 4 ] = { 1, 0, 0, 0 };	// Face material properties

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations

	glPushMatrix();

	glTranslatef( a->x, a->y, 0 );

	glRotatef( alien_angle, 0, 1, 0 );	// Rotate constantly

	glRotatef( 90, 1, 0, 0 );				// Turn sideways

	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	glutSolidTorus( 0.25, 1, 20, 20 );
	glRotatef( 135, 1, 1, 0 );
	glutSolidCube( 1 );

	glPopMatrix();
	glFlush();				    // Flush OpenGL queue
}

void display_alien_shot( AlienShot *s )
{
	float mat[ 1 ][ 4 ] = { 10, 0, 0, 0 };	// Face material properties
	glMatrixMode( GL_MODELVIEW );		// Stup model transformations

	glPushMatrix();

	glTranslatef( s->x, s->y, 0 );
  
	glPointSize( 10 );
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	glBegin( GL_POINTS );
	glVertex2f( 0, 0 );
	glEnd();

	glPopMatrix();
	glFlush();
}

void display_explosion( int x, int y, float explosionScale ) {
	float mat[ 1 ][ 4 ] = { 10, 10, 10, 0 };	// Face material properties
	glMatrixMode( GL_MODELVIEW );		// Stup model transformations
	glPushMatrix();

	glTranslatef( x, y, 0 );
  
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	glScalef( explosionScale, explosionScale, 0 );
	if ( explosionScale < 5 )
		glutSolidTorus( 0.05, 0.5, 10, 10 );
	glPopMatrix();
	glFlush();
}

void display_score()
{
	glPushMatrix();

	float mat[ 1 ][ 4 ] = { 10, 10, 10, 0 };	// Face material properties

	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	char c[50];
	sprintf_s( c, "Score: %d\0", score );
	glRasterPos3f( boundX-15, boundY-3, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, c[i] );
	}
	glPopMatrix();
	glFlush();
	
}

void display_statistics( Player *p )
{
	glPushMatrix();

	float mat[ 1 ][ 4 ] = { 10, 10, 10, 0 };	// Face material properties

	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	glPushMatrix();
	char c[50];
	sprintf( c, "Shots fired: %d  \0", p->shotsFired );
	glRasterPos3f( 1, 4, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, c[i] );
		c[i] = '\0';
	}
	sprintf( c, "Number of times you \"misfired%\": %d  \0", p->shotsFired-(p->asteroidsShot + p->aliensShot) );
	glRasterPos3f( 1, 2, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, c[i] );
		c[i] = '\0';
	}
	sprintf( c, "Potential Xenomorph Hives Destroyed: %d\0", p->asteroidsShot );
	glRasterPos3f( 1, 0, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, c[i] );
		c[i] = '\0';
	}
	sprintf( c, "Potential Alien Friends Killed: %d\0", p->aliensShot );
	glRasterPos3f( 1, -2, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, c[i] );
		c[i] = '\0';
	}
	if ( p->shotsFired != 0 )
		p->accuracy = 100*( ((float)p->asteroidsShot + (float)p->aliensShot)/(float)p->shotsFired );
	sprintf( c, "Accuracy: %.f %% \0", p->accuracy );
	glRasterPos3f( 1, -4, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, c[i] );
		c[i] = '\0';
	}
	glPopMatrix();
	glFlush();
}

void display_lives()
// This routine displays the player's lives as small ships
// in the left corner of the screen.
{
	int    f[ 30 ][ 3 ] = {		// Face vertex indices
		{ 5, 15, 4 },
		{ 6, 15, 5 },
		{ 15, 3, 4 },
		{ 6, 7, 15 },
		{ 7, 3, 15 },
		{ 2, 3, 7 },
		{ 14, 1, 2 },
		{ 2, 7, 14 },
		{ 7, 8, 14 },
		{ 9, 14, 8 },
		{ 0, 14, 9 },
		{ 1, 14, 0 },
		{ 0, 9, 13 },
		{ 9, 16, 13 },
		{ 11, 16, 9 },
		{ 12, 16, 11 },
		{ 10, 16, 12 },
		{ 13, 16, 10 },
		{ 10, 12, 11 },
		{ 0, 13, 10 },
		{ 10, 11, 0 },
		{ 11, 9, 8 },
		{ 8, 0, 11 },
		{ 2, 0, 8 },
		{ 1, 0, 2 },
		{ 2, 8, 6 },
		{ 8, 7, 6 },
		{ 6, 5, 4 },
		{ 4, 2, 6 },
		{ 3, 2, 4 }
	};	
	float  v[ 17 ][ 3 ] = {		// Vertex positions
		{ 0.4,  0,  0 },			//0
		{ 0,  1.5,  0 },			//1
		{ -0.4,  0,  0 },			//2
		{ -0.5,  1,  0 },			//3
		{ -0.75,  0,  0 },			//4
		{ -1.5,  -1,  0 },			//5
		{ -0.75,  -0.75,  0 },		//6
		{ -0.4,  -1,  0.7 },		//7
		{ 0,  -0.5,  0.45 },		//8
		{ 0.4,  -1,  0.7 },			//9
		{ 0.75,  0,  0 },			//10
		{ 0.75,  -0.75,  0 },		//11
		{ 1.5,  -1,  0 },			//12
		{ 0.5,  1,  0 },			//13
		{ 0,  0.4,  0.5 },			//14
		{ -0.6,  -0.25,  0.25 },	//15
		{ 0.6,  -0.25,  0.25 }		//16
	};	
	float  n[ 30 ][ 3 ];			// Face normals
	for ( int i = 0; i < 30; i++ ) {
		n[ i ][ 0 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 0 ];
		n[ i ][ 1 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 1 ];
		n[ i ][ 2 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 2 ];
	}
	float mat[ 1 ][ 4 ] = { 0, 1, 0, 0 };	// Face material properties

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
  
	//glPushMatrix();

	for ( int j = 0; j < lives-1; j++) {
		glPushMatrix();
		glTranslatef( -boundX + 3 + (3*j), boundY - 2, -3 );
		glRotatef( alien_angle, 0, 1, 0 );	// Rotate constantly (using alien's rotation)
		for ( int i = 0; i < 30; i++ ) {		// For all faces
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
			glBegin( GL_POLYGON );
			// Specify face normal
			glNormal3f( n[ i ][ 0], n[ i ][ 1 ], n[ i ][ 2 ] );
			for( int j = 0; j < 3; j++ ) {		// For all face vertices
				glVertex3fv( v[ f[ i ][ j ] ] );
			}
			glEnd();
		}
		glPopMatrix();
	}
	//glPopMatrix();
	glFlush();				// Flush OpenGL queue
}

void display_game_over()
{
	int    f[ 30 ][ 3 ] = {		// Face vertex indices
		{ 5, 15, 4 },
		{ 6, 15, 5 },
		{ 15, 3, 4 },
		{ 6, 7, 15 },
		{ 7, 3, 15 },
		{ 2, 3, 7 },
		{ 14, 1, 2 },
		{ 2, 7, 14 },
		{ 7, 8, 14 },
		{ 9, 14, 8 },
		{ 0, 14, 9 },
		{ 1, 14, 0 },
		{ 0, 9, 13 },
		{ 9, 16, 13 },
		{ 11, 16, 9 },
		{ 12, 16, 11 },
		{ 10, 16, 12 },
		{ 13, 16, 10 },
		{ 10, 12, 11 },
		{ 0, 13, 10 },
		{ 10, 11, 0 },
		{ 11, 9, 8 },
		{ 8, 0, 11 },
		{ 2, 0, 8 },
		{ 1, 0, 2 },
		{ 2, 8, 6 },
		{ 8, 7, 6 },
		{ 6, 5, 4 },
		{ 4, 2, 6 },
		{ 3, 2, 4 }
	};	
	float  v[ 17 ][ 3 ] = {		// Vertex positions
		{ 0.4,  0,  0 },			//0
		{ 0,  1.5,  0 },			//1
		{ -0.4,  0,  0 },			//2
		{ -0.5,  1,  0 },			//3
		{ -0.75,  0,  0 },			//4
		{ -1.5,  -1,  0 },			//5
		{ -0.75,  -0.75,  0 },		//6
		{ -0.4,  -1,  0.7 },		//7
		{ 0,  -0.5,  0.45 },		//8
		{ 0.4,  -1,  0.7 },			//9
		{ 0.75,  0,  0 },			//10
		{ 0.75,  -0.75,  0 },		//11
		{ 1.5,  -1,  0 },			//12
		{ 0.5,  1,  0 },			//13
		{ 0,  0.4,  0.5 },			//14
		{ -0.6,  -0.25,  0.25 },	//15
		{ 0.6,  -0.25,  0.25 }		//16
	};	
	float  n[ 30 ][ 3 ];			// Face normals
	for ( int i = 0; i < 30; i++ ) {
		n[ i ][ 0 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 0 ];
		n[ i ][ 1 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 1 ];
		n[ i ][ 2 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 2 ];
	}
	float mat[ 1 ][ 4 ] = { 0, 1, 0, 0 };	// Face material properties

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
  
	glPushMatrix();

	glTranslatef( -3, 0, 25 );
	glRotatef( alien_angle, 1, 1, 1 );	// Rotate constantly (using alien's rotation)
	for ( int i = 0; i < 30; i++ ) {		// For all faces
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
		glBegin( GL_POLYGON );
		// Specify face normal
		glNormal3f( n[ i ][ 0], n[ i ][ 1 ], n[ i ][ 2 ] );
		for( int j = 0; j < 3; j++ ) {		// For all face vertices
			glVertex3fv( v[ f[ i ][ j ] ] );
		}
		glEnd();
	}
	glPopMatrix();

	glPushMatrix();
	if ( gameOverStats == 1 )
		display_statistics( &player );
	glPopMatrix();

	char c[50];
	glPushMatrix();
	sprintf( c, "GAME OVER" );
	glTranslatef( -boundX/2 - 8, boundY/2, 0 );
	glScalef( gameOverScale, gameOverScale, gameOverScale );
	for ( int i = 0; i < 50; i++ ) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c[i]);
		c[i] = '\0';
	}
	glPopMatrix();

	glPushMatrix();
	if ( score <= 9000 )
		sprintf( c, "Score: %d", score );
	else
		sprintf( c, "Score: OVER 9000!" );
	glTranslatef( -boundX/2 - 2, -boundY/2, 0 );
	glScalef( gameOverScale/2, gameOverScale/2, gameOverScale/2 );
	for ( int i = 0; i < 50; i++ )
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c[i]);
	glPopMatrix();

	glFlush();				// Flush OpenGL queu
}

void scale_game_over()
{
	if ( gameOverScale >= 0 && gameOverScale <= 0.05 ) {
		gameOverScale += 0.001;
		gameOverStats = 0;
	}
	if ( gameOverScale >= 0.05 )
		gameOverStats = 1;
}
	
void display_all()
//  This routine displays the player's ship, an alien ship, 
//  and an asteroid
{
	glMatrixMode( GL_MODELVIEW );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
  
	if ( startGame == 1 ) {
		glPushMatrix();
		start_screen();
		glPopMatrix();
	} else if ( lives > 0 ) {
		glPushMatrix();				// Draw player ship
		if ( player.active == 1 ) {
			//display_player_boundary( &player );
			display_player( &player );
		}
		if ( player.shot == 1 ) 
			display_explosion( player.x, player.y, player.explosionScale );
		glPopMatrix();
	
		glPushMatrix();				// Draw shots
		for ( int i = 0; i < SHOT_MAX; i++ ) {
			if ( shots[ i ].isOnScreen == 1 )
				display_shot( &shots[ i ] );
		}
		glPopMatrix();
	
		glPushMatrix();				// Draw alien shots
		for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
			if ( alienShots[ i ].active == 1 )
				display_alien_shot( &alienShots[ i ] );
		}
		glPopMatrix();

		glPushMatrix();				// Draw alien shots
		for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
			if ( smallAlienShots[ i ].active == 1 )
				display_alien_shot( &smallAlienShots[ i ] );
		}
		glPopMatrix();
	
		glPushMatrix();				// Draw large asteroids
		for ( int i = 0; i < ASTEROID_MAX; i++ ) {
			if ( asteroids[ i ].active == 1 ) {
				display_large_asteroid( &asteroids[ i ] );
				//display_asteroid_boundary( &asteroids[ i ] );
			}
			if ( asteroids[ i ].shot == 1 ) {
				display_explosion( asteroids[ i ].x, asteroids[ i ].y, asteroids[ i ].explosionScale );
			}
		}
		glPopMatrix();
			
		glPushMatrix();				// Draw medium asteroids
		for ( int i = 0; i < ASTEROID_MAX * 2; i++ ) {
			if ( med_asteroids[ i ].active == 1 ) {
				display_med_asteroid( &med_asteroids[ i ] );
				//display_asteroid_boundary( &med_asteroids[ i ] );
			}
			if ( med_asteroids[ i ].shot == 1 ) {
				display_explosion( med_asteroids[ i ].x, med_asteroids[ i ].y, med_asteroids[ i ].explosionScale );
			}
		}
		glPopMatrix();
			
		glPushMatrix();				// Draw small asteroids
		for ( int i = 0; i < ASTEROID_MAX * 4; i++ ) {
			if ( small_asteroids[ i ].active == 1 ) {
				display_small_asteroid( &small_asteroids[ i ] );
				//display_asteroid_boundary( &small_asteroids[ i ] );
			}
			if ( small_asteroids[ i ].shot == 1 ) {
				display_explosion( small_asteroids[ i ].x, small_asteroids[ i ].y, small_asteroids[ i ].explosionScale );
			}
		}
		glPopMatrix();

		glPushMatrix();				// Draw alien ship
		if ( alien.active == 1 ) {
			display_alien( &alien );
		}
		if ( alien.shot == 1 ) {
			display_explosion( alien.x, alien.y, alien.explosionScale );
		}
		glPopMatrix();

		glPushMatrix();
		if ( smallAlien.active == 1 ) {
			display_small_alien( &smallAlien );
		}
		if ( smallAlien.shot == 1 ) {
			display_explosion( smallAlien.x, smallAlien.y, smallAlien.explosionScale );
		}
		glPopMatrix();
		
		glPushMatrix();				// Draw score
		display_score();
		glPopMatrix();
	
		glPushMatrix();				// Draw player lives
		display_lives();
		glPopMatrix();

	} else if ( lives <= 0 ) {
		
		glPushMatrix();
		display_game_over();
		glPopMatrix();
	
	}
	glutSwapBuffers();			// Display back buffer
}

float* compute_normal ( float v0[3], float v1[3], float v2[3] )
// This routine computes the normal of a polygon with 3 vertices
{
	float d0[3], d1[3], n[3];
	d0[0] = v1[0] - v0[0];
	d0[1] = v1[1] - v0[1];
	d0[2] = v1[2] - v0[2];

	d1[0] = v2[0] - v1[0];
	d1[1] = v2[1] - v1[1];
	d1[2] = v2[2] - v1[2];
	
	n[0] = (d0[1]*d1[2]) - (d0[2]*d1[1]);
	n[1] = (d0[2]*d1[0]) - (d0[0]*d1[2]);
	n[2] = (d0[0]*d1[1]) - (d0[1]*d1[0]);
	
	return n;
}

void handle_menu( int ID )
//  This routine handles popup menu selections
//
//  ID:  Menu entry ID
{
	switch( ID ) {
	case 0:				// New Game
		reset_game();
		break;
	case 1:				// Quit
		exit( 0 );
		break;
	}
}					// End routine handle_menu

void keyboard_special( int key, int x, int y )
//  This routine handles the pressing of the left and right
//  arrow keys.
{
	switch( key ) {
	case GLUT_KEY_LEFT:
		rotateLeft = true;
		break;
	case GLUT_KEY_RIGHT:
		rotateRight = true;
		break;
	}
}

void keyboard_special_release( int key, int x, int y )
//  This routine handles the release of the left and right
//  arrow keys.
{
	switch( key ) {
	case GLUT_KEY_LEFT:
		rotateLeft = false;
		break;
	case GLUT_KEY_RIGHT:
		rotateRight = false;
		break;
	}
}

void keyboard( unsigned char key, int x, int y )
//  This routine handles the pressing of 'x', 'z' and 
//  the escape key.
{
	switch( key ) {
	case 'x':
		accelerating = true;
		break;
	case 'z':
		if ( player.active == 1 )
			shooting = 1;
		break;
	case 27:
		exit( 0 );
		break;
	}
}

void keyboard_release( unsigned char key, int x, int y )
//  This routine handles the release of 'x', 'z' and 
//  the escape key.
{
	switch( key ) {
	case 'x':
		accelerating = false;
		break;
	}
}

void player_angles()
//  This routine handles the player ship's rotation.
{
	if (rotateLeft)
		player.theta += DEG2RAD * player_rot;
	else if (rotateRight)
		player.theta -= DEG2RAD * player_rot;
}

void alien_angles()
//  This routine handles the alien ship's rotation.
{
	alien_angle += 2.0;
	if ( alien_angle > 360 )
		alien_angle -= 360;
}

void scale_explosion()
{
	//explosion_scale += 0.1;
}

void asteroid_angles()
//  This routine handles the alien ship's rotation.
{
	asteroid_angle += 2.0;
	if ( asteroid_angle > 360 )
		asteroid_angle -= 360;
}

void player_movement()
//  This routine handles the player ship's movement.
{
	if ( player.shot == 1 ) {
		reset_player();
		player.explosionScale = 2;
	}
	player_angles();
	if ( accelerating ) {
		player.velX = player.velX - 0.02 * sin( player.theta );
		player.velY = player.velY + 0.02 * cos( player.theta );
	} else if ( !accelerating ) {
		double vel_angle = atan2( player.velY, player.velX ) - (PI/2);
		player.velX = player.velX + 0.01 * sin( vel_angle );
		player.velY = player.velY - 0.01 * cos( vel_angle );

		if ( abs( player.velX ) <= 0.001 && abs( player.velY ) <= 0.001 ) {
			player.velX = 0;
			player.velY = 0;
		}
	}

	// Limit the player ship's velocity magnitude
	float temp = player.velX * player.velX + player.velY * player.velY;
	if( accelerating && temp > 0.5 ) {
		temp = 0.5 / sqrt( temp );
		player.velX *= temp;
		player.velY *= temp;
	}

	player.x = player.x + player.velX;
	player.y = player.y + player.velY;
	player.age++;
}

void shot_movement ()
//  This routine handles shot movement
{
	if ( shooting == 1 ) {
		for ( int i = 0; i < SHOT_MAX; i++ ) {
			if ( shots[ i ].isOnScreen == 0 ) {
				player.shotsFired++;
				shots[ i ].isOnScreen = 1;
				shots[ i ].x = player.x;
				shots[ i ].y = player.y;
				shots[ i ].velX = -SHOT_SPEED * sin( player.theta ) + player.velX;
				shots[ i ].velY = SHOT_SPEED * cos( player.theta ) + player.velY;
				break;
			}
		}
		shooting = 0;
	}
	for ( int i = 0; i < SHOT_MAX; i++ ) {
		if ( shots[ i ].isOnScreen == 1 ) {
			shots[ i ].x = shots[ i ].x + shots[ i ].velX;
			shots[ i ].y = shots[ i ].y + shots[ i ].velY;
			shots[ i ].time++;
			if ( shots[ i ].time >= SHOT_MAX_TIME ) {
				shots[ i ].isOnScreen = 0;
				shots[ i ].time = 0;
			}
		}
	}
}

void asteroid_movement ()
//  This routine handles asteroid movement
{
	if ( screenNum == 1 )
		currentAsteroids = START_ASTEROIDS;
	else if ( screenNum == 2 )
		currentAsteroids = START_ASTEROIDS + 1;
	else if ( screenNum == 3 )
		currentAsteroids = START_ASTEROIDS + 2;
	else if ( screenNum == 4 )
		currentAsteroids = START_ASTEROIDS + 3;
	else if ( screenNum == 5 )
		currentAsteroids = START_ASTEROIDS + 4;
	else if ( screenNum == 6 )
		currentAsteroids = START_ASTEROIDS + 5;
	for ( int i = 0; i < currentAsteroids * 2; i++ ) {
		if ( med_asteroids[ i ].active == 0 && med_asteroids[ i ].shot == 0 && i % 2 != 1 ) { //EVEN
			med_asteroids[ i ].x = asteroids[ i / 2 ].x;
			med_asteroids[ i ].y = asteroids[ i / 2 ].y;
		} else if ( med_asteroids[ i ].active == 0 && med_asteroids[ i ].shot == 0 && i % 2 == 1 ) { //ODD
			med_asteroids[ i ].x = asteroids[ ( i - 1 ) / 2 ].x;
			med_asteroids[ i ].y = asteroids[ ( i - 1 ) / 2 ].y;
		}
	}
	for ( int i = 0; i < currentAsteroids * 4; i++ ) {
		if ( small_asteroids[ i ].active == 0 && small_asteroids[ i ].shot == 0 && i % 2 != 1 ) { //EVEN
			small_asteroids[ i ].x = med_asteroids[ i / 2 ].x;
			small_asteroids[ i ].y = med_asteroids[ i / 2 ].y;
		} else if ( small_asteroids[ i ].active == 0 && small_asteroids[ i ].shot == 0 && i % 2 == 1 ) { //ODD
			small_asteroids[ i ].x = med_asteroids[ ( i - 1 ) / 2 ].x;
			small_asteroids[ i ].y = med_asteroids[ ( i - 1 ) / 2 ].y;
		}
	}
	for ( int i = 0; i < currentAsteroids; i++ ) {
		if ( asteroids[ i ].active == 0 && asteroids[ i ].shot == 0 ) {
			asteroids[ i ].x = -boundX;
			asteroids[ i ].y = boundY;
			int side = rand() % 5 + 1;					// Randomly choose a side for the asteroid to appear from
			if ( side == 1 ) {							// Top
				asteroids[ i ].x = rand() % 37 - 18;
				asteroids[ i ].y = boundY;
			} else if ( side == 2 ) {					// Right
				asteroids[ i ].x = boundX;
				asteroids[ i ].y = rand() % 37 - 18;
			} else if ( side == 3 ) {					// Bottom
				asteroids[ i ].x = rand() % 37 - 18;
				asteroids[ i ].y = -boundY;
			} else if ( side == 4 ) {					// Left
				asteroids[ i ].x = -boundX;
				asteroids[ i ].y = rand() % 37 - 18;
			}
			asteroids[ i ].velX = -0.25 * sin( (float)rand() );
			asteroids[ i ].velY = 0.25 * cos( (float)rand() );
			int randRot[ 3 ];
			for ( int j = 0; j < 3; j++ ) {
				randRot[ j ] = rand() % 2;
			}
			asteroids[ i ].theta[ 0 ] = randRot[ 0 ];
			asteroids[ i ].theta[ 1 ] = randRot[ 1 ];
			asteroids[ i ].theta[ 2 ] = randRot[ 2 ];
			asteroids[ i ].active = 1;
			asteroids[ i ].shot = 0;
		} else if ( asteroids[ i ].shot == 1 ) {
			asteroids[ i ].explosionScale += EXPLOSION_UPDATE;
			if (med_asteroids[ i * 2 ].active == 0 && med_asteroids[ i * 2 ].shot == 0 ) {
				med_asteroids[ i * 2 ].velX = -0.25 * sin( (float)rand() );
				med_asteroids[ i * 2 ].velY = 0.25 * cos( (float)rand() );
				int randRot[ 3 ];
				for ( int j = 0; j < 3; j++ ) {
					randRot[ j ] = rand() % 2;
				}
				med_asteroids[ i * 2 ].theta[ 0 ] = randRot[ 0 ];
				med_asteroids[ i * 2 ].theta[ 1 ] = randRot[ 1 ];
				med_asteroids[ i * 2 ].theta[ 2 ] = randRot[ 2 ];
				med_asteroids[ i * 2 ].active = 1;
				med_asteroids[ i * 2 ].shot = 0;
			} else if (med_asteroids[ (i * 2) + 1 ].active == 0 && med_asteroids[ (i * 2) + 1 ].shot == 0 ) {
				med_asteroids[ (i * 2) + 1 ].velX = -0.25 * sin( (float)rand() );
				med_asteroids[ (i * 2) + 1 ].velY = 0.25 * cos( (float)rand() );
				int randRot[ 3 ];
				for ( int j = 0; j < 3; j++ ) {
					randRot[ j ] = rand() % 2;
				}
				med_asteroids[ (i * 2) + 1 ].theta[ 0 ] = randRot[ 0 ];
				med_asteroids[ (i * 2) + 1 ].theta[ 1 ] = randRot[ 1 ];
				med_asteroids[ (i * 2) + 1 ].theta[ 2 ] = randRot[ 2 ];
				med_asteroids[ (i * 2) + 1 ].active = 1;
				med_asteroids[ (i * 2) + 1 ].shot = 0;
			}
		}
	}
	for ( int i = 0; i < currentAsteroids * 2; i++ ) {
		if ( med_asteroids[ i ].shot == 1 ) {
			med_asteroids[ i ].explosionScale += EXPLOSION_UPDATE;
			if (small_asteroids[ i * 2 ].active == 0 && small_asteroids[ i * 2 ].shot == 0 ) {
				small_asteroids[ i * 2 ].velX = -0.25 * sin( (float)rand() );
				small_asteroids[ i * 2 ].velY = 0.25 * cos( (float)rand() );
				int randRot[ 3 ];
				for ( int j = 0; j < 3; j++ ) {
					randRot[ j ] = rand() % 2;
				}
				small_asteroids[ i * 2 ].theta[ 0 ] = randRot[ 0 ];
				small_asteroids[ i * 2 ].theta[ 1 ] = randRot[ 1 ];
				small_asteroids[ i * 2 ].theta[ 2 ] = randRot[ 2 ];
				small_asteroids[ i * 2 ].active = 1;
				small_asteroids[ i * 2 ].shot = 0;
			} else if (small_asteroids[ (i * 2) + 1 ].active == 0 && small_asteroids[ (i * 2) + 1 ].shot == 0 ) {
				small_asteroids[ (i * 2) + 1 ].velX = -0.25 * sin( (float)rand() );
				small_asteroids[ (i * 2) + 1 ].velY = 0.25 * cos( (float)rand() );
				int randRot[ 3 ];
				for ( int j = 0; j < 3; j++ ) {
					randRot[ j ] = rand() % 2;
				}
				small_asteroids[ (i * 2) + 1 ].theta[ 0 ] = randRot[ 0 ];
				small_asteroids[ (i * 2) + 1 ].theta[ 1 ] = randRot[ 1 ];
				small_asteroids[ (i * 2) + 1 ].theta[ 2 ] = randRot[ 2 ];
				small_asteroids[ (i * 2) + 1 ].active = 1;
				small_asteroids[ (i * 2) + 1 ].shot = 0;
			}
		}
	}
	for ( int i = 0; i < currentAsteroids * 4; i++ ) {
		if ( small_asteroids[ i ].shot == 1 )
			small_asteroids[ i ].explosionScale += EXPLOSION_UPDATE;
	}
	for ( int i = 0; i < currentAsteroids; i++ ) {
		if ( asteroids[ i ].active == 1 ) {
			asteroids[ i ].x = asteroids[ i ].x + asteroids[ i ].velX;
			asteroids[ i ].y = asteroids[ i ].y + asteroids[ i ].velY;
		}
	}
	for ( int i = 0; i < currentAsteroids * 2; i++ ) {
		if ( med_asteroids[ i ].active == 1 ) {
			med_asteroids[ i ].x = med_asteroids[ i ].x + med_asteroids[ i ].velX;
			med_asteroids[ i ].y = med_asteroids[ i ].y + med_asteroids[ i ].velY;
		}
	}
	for ( int i = 0; i < currentAsteroids * 4; i++ ) {
		if ( small_asteroids[ i ].active == 1 ) {
			small_asteroids[ i ].x = small_asteroids[ i ].x + small_asteroids[ i ].velX;
			small_asteroids[ i ].y = small_asteroids[ i ].y + small_asteroids[ i ].velY;
		}
	}
}

void alien_movement()
//  This routine handles the alien ship's movement.
{
	alien_angles();
	if ( alien.active == 0 && alien.shot == 0 && alien.done == 0 ) {
		alien.speedY = 0.2;
		alien.x = -boundX;
		alien.y = boundY;
		int side = rand() % 2 + 1;			// Randomly choose a side for the alien to appear from
		if ( side == 1 ) {					// Right
			alien.x = boundX;
			alien.y = rand() % 37 - 18;
			alien.velX = -ALIEN_SPEED;
		} else if ( side == 2 ) {			// Left
			alien.x = -boundX;
			alien.y = rand() % 37 - 18;
			alien.velX = ALIEN_SPEED;
		}
		alien.dirY = (float) rand();
		alien.velY = alien.speedY * cos( alien.dirY );
		alien.active = 1;
	}
	alien.age++;
	if ( alien.age >= ALIEN_UPDATE && alien.active == 1 && alien.shot == 0 ) {
		alienShooting = 1;
		alien.speedY = rand() % 3 - 1;
		alien.speedY *= 0.5;
		alien.velY = alien.speedY * cos( alien.dirY );
		alien.age = 0;
	}
	if ( alien.active == 1 && alien.shot == 0 ) {
		alien.x = alien.x + alien.velX;
		alien.y = alien.y + alien.velY;
	}
	if ( alien.shot == 1 ) {
		alien.explosionScale += EXPLOSION_UPDATE;
	}
}

void small_alien_movement()
{
	alien_angles();
	if ( smallAlien.active == 0 && smallAlien.shot == 0 && smallAlien.done == 0 ) {
		smallAlien.speedY = 0.6;
		smallAlien.x = -boundX;
		smallAlien.y = boundY;
		int side = rand() % 2 + 1;			// Randomly choose a side for the alien to appear from
		if ( side == 1 ) {					// Right
			smallAlien.x = boundX;
			smallAlien.y = rand() % 37 - 18;
			smallAlien.velX = -SMALL_ALIEN_SPEED;
		} else if ( side == 2 ) {			// Left
			smallAlien.x = -boundX;
			smallAlien.y = rand() % 37 - 18;
			smallAlien.velX = SMALL_ALIEN_SPEED;
		}
		smallAlien.dirY = (float) rand();
		smallAlien.velY = smallAlien.speedY * cos( smallAlien.dirY );
		smallAlien.active = 1;
	}
	smallAlien.age++;
	if ( smallAlien.age >= ALIEN_UPDATE && smallAlien.active == 1 && smallAlien.shot == 0 ) {
		smallAlienShooting = 1;
		smallAlien.speedY = rand() % 3 - 1;
		smallAlien.speedY *= 0.5;
		smallAlien.velY = smallAlien.speedY * cos( smallAlien.dirY );
		smallAlien.age = 0;
	}
	if ( smallAlien.active == 1 && smallAlien.shot == 0 ) {
		smallAlien.x = smallAlien.x + smallAlien.velX;
		smallAlien.y = smallAlien.y + smallAlien.velY;
	}
	if ( smallAlien.shot == 1 ) {
		smallAlien.explosionScale += EXPLOSION_UPDATE;
	}
}

void alien_shot_movement()
//	This routine handles the alien shot movement.
{
	if ( alienShooting == 1 ) {
		for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
			if ( alienShots[ i ].active == 0 ) {
				alienShots[ i ].active = 1;
				alienShots[ i ].x = alien.x;
				alienShots[ i ].y = alien.y;
				float newDirection = alien_shot_direction();
				alienShots[ i ].velX = -ALIEN_SHOT_SPEED * sin( newDirection );
				alienShots[ i ].velY = ALIEN_SHOT_SPEED * cos( newDirection );
				break;
			}
		}
		alienShooting = 0;
	}
	for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
		if ( alienShots[ i ].active == 1 ) {
			alienShots[ i ].x = alienShots[ i ].x + alienShots[ i ].velX;
			alienShots[ i ].y = alienShots[ i ].y + alienShots[ i ].velY;
		}
	}
}

void small_alien_shot_movement()
//	This routine handles the alien shot movement.
{
	if ( smallAlienShooting == 1 ) {
		for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
			if ( smallAlienShots[ i ].active == 0 ) {
				smallAlienShots[ i ].active = 1;
				smallAlienShots[ i ].x = smallAlien.x;
				smallAlienShots[ i ].y = smallAlien.y;
				float newDirection = small_alien_shot_direction();
				smallAlienShots[ i ].velX = -SMALL_ALIEN_SHOT_SPEED * sin( newDirection );
				smallAlienShots[ i ].velY = SMALL_ALIEN_SHOT_SPEED * cos( newDirection );
				break;
			}
		}
		smallAlienShooting = 0;
	}
	for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
		if ( smallAlienShots[ i ].active == 1 ) {
			smallAlienShots[ i ].x = smallAlienShots[ i ].x + smallAlienShots[ i ].velX;
			smallAlienShots[ i ].y = smallAlienShots[ i ].y + smallAlienShots[ i ].velY;
		}
	}
}

float alien_shot_direction()
{
	float newDirection = 0;
	int probabilityIndex = rand() % 99 + 1;
	if ( probabilityIndex < ALIEN_PROB_PLAYER ) {
		newDirection = atan2( (player.y - alien.y), (player.x - alien.x) ) - (PI/2);
	} else if ( probabilityIndex > ALIEN_PROB_PLAYER && 
				probabilityIndex < ALIEN_PROB_PLAYER + ALIEN_PROB_ASTEROID ) {
		int asteroidType = rand() % 3 + 1;
		if ( asteroidType == 1 ) {				// Large asteroids
			int index = rand() % ASTEROID_MAX;
			newDirection = atan2( (asteroids[index].y - alien.y), (asteroids[index].x - alien.x) ) - (PI/2);
		} else if ( asteroidType == 2 ) {		// Medium asteroids
			int index = rand() % (2 * ASTEROID_MAX);
			newDirection = atan2( (med_asteroids[index].y - alien.y), (med_asteroids[index].x - alien.x) ) - (PI/2);
		} else if ( asteroidType == 3 ) {		// Small asteroids
			int index = rand() % (3 * ASTEROID_MAX);
				newDirection = atan2( (small_asteroids[index].y - alien.y), (small_asteroids[index].x - alien.x) ) - (PI/2);
		}
	} else if ( probabilityIndex > ALIEN_PROB_PLAYER + ALIEN_PROB_ASTEROID && 
				probabilityIndex < ALIEN_PROB_PLAYER + ALIEN_PROB_ASTEROID + ALIEN_PROB_RANDOM ) {
		float x = rand();
		float y = rand();
		newDirection = atan2( y, x );
	}
	return newDirection;
}

float small_alien_shot_direction( void )
{
	float newDirection = 0;
	int probabilityIndex = rand() % 99 + 1;
	if ( probabilityIndex < SMALL_ALIEN_PROB_PLAYER ) {
		newDirection = atan2( (player.y - smallAlien.y), (player.x - smallAlien.x) ) - (PI/2);
	} else if ( probabilityIndex > SMALL_ALIEN_PROB_PLAYER && 
				probabilityIndex < SMALL_ALIEN_PROB_PLAYER + SMALL_ALIEN_PROB_ASTEROID ) {
		int asteroidType = rand() % 3 + 1;
		if ( asteroidType == 1 ) {				// Large asteroids
			int index = rand() % ASTEROID_MAX;
			newDirection = atan2( (asteroids[index].y - smallAlien.y), (asteroids[index].x - smallAlien.x) ) - (PI/2);
		} else if ( asteroidType == 2 ) {		// Medium asteroids
			int index = rand() % (2 * ASTEROID_MAX);
			newDirection = atan2( (med_asteroids[index].y - smallAlien.y), (med_asteroids[index].x - smallAlien.x) ) - (PI/2);
		} else if ( asteroidType == 3 ) {		// Small asteroids
			int index = rand() % (3 * ASTEROID_MAX);
				newDirection = atan2( (small_asteroids[index].y - smallAlien.y), (small_asteroids[index].x - smallAlien.x) ) - (PI/2);
		}
	} else if ( probabilityIndex > SMALL_ALIEN_PROB_PLAYER + SMALL_ALIEN_PROB_ASTEROID && 
				probabilityIndex < SMALL_ALIEN_PROB_PLAYER + SMALL_ALIEN_PROB_ASTEROID + SMALL_ALIEN_PROB_RANDOM ) {
		float x = rand();
		float y = rand();
		newDirection = atan2( y, x );
	}
	return newDirection;
}

void wrap_player ()
//  This routine handles wrapping the player's ship 
//  around the screen.
{
	if ( player.x > boundX )
		player.x = -boundX;
	if ( player.x < -boundX )
		player.x = boundX;
	if ( player.y > boundY )
		player.y = -boundY;
	if ( player.y < -boundY )
		player.y = boundY;
}

void wrap_alien ()
//	This routine handles wrapping the alien ship
//	around the screen.
{
	if ( alien.x > boundX +10 ) {
		alien.active = 0;
		alien.shot = 1;
		alien.done = 1;
	}
	if ( alien.x < -boundX -10 ) {
		alien.active = 0;
		alien.shot = 1;
		alien.done = 1;
	}
	if ( alien.y > boundY )
		alien.y = -boundY;
	if ( alien.y < -boundY )
		alien.y = boundY;
}

void wrap_small_alien()
{
	if ( smallAlien.x > boundX +10 ) {
		smallAlien.active = 0;
		smallAlien.shot = 1;
		smallAlien.done = 1;
	}
	if ( smallAlien.x < -boundX -10 ) {
		smallAlien.active = 0;
		smallAlien.shot = 1;
		smallAlien.done = 1;
	}
	if ( smallAlien.y > boundY )
		smallAlien.y = -boundY;
	if ( smallAlien.y < -boundY )
		smallAlien.y = boundY;
}

void wrap_asteroids ( void )
//  This routine handles wrapping the player's ship 
//  around the screen.
{
	for ( int i = 0; i < ASTEROID_MAX; i++ ) {
		if ( asteroids[ i ].x > boundX )
			asteroids[ i ].x = -boundX;
		if ( asteroids[ i ].x < -boundX )
			asteroids[ i ].x = boundX;
		if ( asteroids[ i ].y > boundY )
			asteroids[ i ].y = -boundY;
		if ( asteroids[ i ].y < -boundY )
			asteroids[ i ].y = boundY;
	}
	for ( int i = 0; i < ASTEROID_MAX * 2; i++ ) {
		if ( med_asteroids[ i ].x > boundX )
			med_asteroids[ i ].x = -boundX;
		if ( med_asteroids[ i ].x < -boundX )
			med_asteroids[ i ].x = boundX;
		if ( med_asteroids[ i ].y > boundY )
			med_asteroids[ i ].y = -boundY;
		if ( med_asteroids[ i ].y < -boundY )
			med_asteroids[ i ].y = boundY;
	}
	for ( int i = 0; i < ASTEROID_MAX * 4; i++ ) {
		if ( small_asteroids[ i ].x > boundX )
			small_asteroids[ i ].x = -boundX;
		if ( small_asteroids[ i ].x < -boundX )
			small_asteroids[ i ].x = boundX;
		if ( small_asteroids[ i ].y > boundY )
			small_asteroids[ i ].y = -boundY;
		if ( small_asteroids[ i ].y < -boundY )
			small_asteroids[ i ].y = boundY;
	}
}

void wrap_shots ( void )
//  This routine handles wrapping the player's shots
//  around the screen
{
	for ( int i = 0; i < SHOT_MAX; i++ ) {
		if ( shots[ i ].x > boundX )
			shots[ i ].x = -boundX;
		if ( shots[ i ].x < -boundX )
			shots[ i ].x = boundX;
		if ( shots[ i ].y > boundY )
			shots[ i ].y = -boundY;
		if ( shots[ i ].y < -boundY )
			shots[ i ].y = boundY;
	}
	for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
		if ( alienShots[ i ].x > boundX )
			alienShots[ i ].active = 0;
		if ( alienShots[ i ].x < -boundX )
			alienShots[ i ].active = 0;
		if ( alienShots[ i ].y > boundY )
			alienShots[ i ].active = 0;
		if ( alienShots[ i ].y < -boundY )
			alienShots[ i ].active = 0;
	}
	for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
		if ( smallAlienShots[ i ].x > boundX )
			smallAlienShots[ i ].active = 0;
		if ( smallAlienShots[ i ].x < -boundX )
			smallAlienShots[ i ].active = 0;
		if ( smallAlienShots[ i ].y > boundY )
			smallAlienShots[ i ].active = 0;
		if ( smallAlienShots[ i ].y < -boundY )
			smallAlienShots[ i ].active = 0;
	}
}

void display_player_boundary( Player *p )
//  This routine displays the player ship boundaries
{
	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
  
	glPushMatrix();

	glTranslatef( p->x, p->y, 0 );

	glRotatef ( RAD2DEG*p->theta, 0, 0, 1 );

	glBegin( GL_POLYGON );
	// Specify face normal
	for( int i = 0; i < 14; i++ ) {		// For all face vertices
		glVertex3fv( playerOutlineVerticies[ playerOutlineFace[ i ] ] );
	}
	glEnd();

	glPopMatrix();
	glFlush();				// Flush OpenGL queue
}

void display_asteroid_boundary( Asteroid *a )
//  This routine displays the boundary of an asteroid
{
	float mat[ 1 ][ 4 ] = { 1, 1, 1, 0 };	// Face material properties
	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
	glPushMatrix();

	glTranslatef( a->x, a->y, 0 );

	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
	glBegin( GL_POLYGON );
	int num_segments = 30;
	for ( int i = 0; i < num_segments; i++ ) {
		float theta = 2.0f * PI * float( i ) / float ( num_segments );
		float x = ASTEROID_RADIUS * cosf(theta);
		float y = ASTEROID_RADIUS * sinf(theta);

		glVertex2f( x, y );
	}
	glEnd();

	glPopMatrix();
	glFlush();				// Flush OpenGL queue
}

void shot_asteroid_collision()
//  This routine determines if there is a collision between an asteroid 
//  and a shot.
{
	for ( int i = 0; i < SHOT_MAX; i++ ) {
		for ( int j = 0; j < ASTEROID_MAX; j++ ) {
			float d = powf( ( asteroids[ j ].x - shots[ i ].x ), 2 ) +
				powf( ( asteroids[ j ].y - shots[ i ].y ), 2 );
			if ( d < powf( ASTEROID_RADIUS * largeAsteroidScale, 2 ) && shots[ i ].isOnScreen && asteroids[ j ].active ) {
				asteroids[ j ].shot = 1;
				asteroids[ j ].active = 0;
				shots[ i ].isOnScreen = 0;
				increment_score( SCORE_LARGE_ASTEROID );
				player.asteroidsShot++;
			}
		}
		for ( int j = 0; j < ASTEROID_MAX * 2; j++ ) {
			float d = powf( ( med_asteroids[ j ].x - shots[ i ].x ), 2 ) +
				powf( ( med_asteroids[ j ].y - shots[ i ].y ), 2 );
			if ( d < powf( ASTEROID_RADIUS * medAsteroidScale, 2 ) && shots[ i ].isOnScreen && med_asteroids[ j ].active ) {
				med_asteroids[ j ].shot = 1;
				med_asteroids[ j ].active = 0;
				shots[ i ].isOnScreen = 0;
				increment_score( SCORE_MED_ASTEROID );
				player.asteroidsShot++;
			}
		}
		for ( int j = 0; j < ASTEROID_MAX * 4; j++ ) {
			float d = powf( ( small_asteroids[ j ].x - shots[ i ].x ), 2 ) +
				powf( ( small_asteroids[ j ].y - shots[ i ].y ), 2 );
			if ( d < powf( ASTEROID_RADIUS * smallAsteroidScale, 2 ) && shots[ i ].isOnScreen && small_asteroids[ j ].active ) {
				small_asteroids[ j ].shot = 1;
				small_asteroids[ j ].active = 0;
				shots[ i ].isOnScreen = 0;
				increment_score( SCORE_SMALL_ASTEROID );
				player.asteroidsShot++;
			}
		}
	}
	for ( int i = 0; i < ALIEN_SHOT_MAX; i++ ) {
		for ( int j = 0; j < ASTEROID_MAX; j++ ) {
			float d = powf( ( asteroids[ j ].x - alienShots[ i ].x ), 2 ) +
				powf( ( asteroids[ j ].y - alienShots[ i ].y ), 2 );
			if ( d < powf( ASTEROID_RADIUS * largeAsteroidScale, 2 ) && alienShots[ i ].active && asteroids[ j ].active ) {
				asteroids[ j ].shot = 1;
				asteroids[ j ].active = 0;
				alienShots[ i ].active = 0;
			}
		}
		for ( int j = 0; j < ASTEROID_MAX * 2; j++ ) {
			float d = powf( ( med_asteroids[ j ].x - alienShots[ i ].x ), 2 ) +
				powf( ( med_asteroids[ j ].y - alienShots[ i ].y ), 2 );
			if ( d < powf( ASTEROID_RADIUS * medAsteroidScale, 2 ) && alienShots[ i ].active && med_asteroids[ j ].active ) {
				med_asteroids[ j ].shot = 1;
				med_asteroids[ j ].active = 0;
				alienShots[ i ].active = 0;
			}
		}
		for ( int j = 0; j < ASTEROID_MAX * 4; j++ ) {
			float d = powf( ( small_asteroids[ j ].x - alienShots[ i ].x ), 2 ) +
				powf( ( small_asteroids[ j ].y - alienShots[ i ].y ), 2 );
			if ( d < powf( ASTEROID_RADIUS * smallAsteroidScale, 2 ) && alienShots[ i ].active && small_asteroids[ j ].active ) {
				small_asteroids[ j ].shot = 1;
				small_asteroids[ j ].active = 0;
				alienShots[ i ].active = 0;
			}
		}
	}
}

void player_asteroid_collision()
//  This routine determines if there is a collision between an asteroid 
//  and a shot.
{
	for ( int i = 0; i < 14; i++ ) {
		float x2, x1, y2, y1, dx, dy;
		if (i != 13) {
			x2 = playerOutlineVerticies[ playerOutlineFace[ i + 1 ] ][ 0 ];
			x1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 0 ];
			y2 = playerOutlineVerticies[ playerOutlineFace[ i + 1 ] ][ 1 ];
			y1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 1 ];
		} else if ( i == 13 ) {
			x2 = playerOutlineVerticies[ playerOutlineFace[ 0 ] ][ 0 ];
			x1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 0 ];
			y2 = playerOutlineVerticies[ playerOutlineFace[ 0 ] ][ 1 ];
			y1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 1 ];
		}
		x2 += player.x;
		x1 += player.x;
		y2 += player.y;
		y1 += player.y;
		dx = x2 - x1;
		dy = y2 - y1;
		float d = powf( dx, 2 ) + powf( dy, 2 );
		for ( int j = 0; j < ASTEROID_MAX; j++ ) {
			float sx = boundX;//asteroids[ j ].x;
			float sy = 0;//asteroids[ j ].y;
			float d2 = powf( ( asteroids[ j ].x - x2 ), 2 ) +
				powf( ( asteroids[ j ].y - y2 ), 2 );
			if ( d2 < powf( ASTEROID_RADIUS * largeAsteroidScale, 2 ) && player.active && asteroids[ j ].active ) {
				asteroids[ j ].shot = 1;
				asteroids[ j ].active = 0;
				player.active = 0;
				player.shot = 1;
				lives--;
			}
			float D = (( x1 - sx ) * ( y2 - sy )) -
				(( x2 - sx ) * ( y1 - sy ));
			float delta = powf( 1, 2 ) * d - powf( D, 2 );
			/*if ( delta >= 0 && player.active && asteroids[ j ].active ) {
				asteroids[ j ].shot = 1;
				asteroids[ j ].active = 0;
				player.active = 0;
			}*/
		}
		for ( int j = 0; j < ASTEROID_MAX * 2; j++ ) {
			float sx = boundX;//med_asteroids[ j ].x;
			float sy = 0;//med_asteroids[ j ].y;
			float d2 = powf( ( med_asteroids[ j ].x - x2 ), 2 ) +
				powf( ( med_asteroids[ j ].y - y2 ), 2 );
			if ( d2 < powf( ASTEROID_RADIUS * medAsteroidScale, 2 ) && player.active && med_asteroids[ j ].active ) {
				med_asteroids[ j ].shot = 1;
				med_asteroids[ j ].active = 0;
				player.active = 0;
				player.shot = 1;
				lives--;
			}
			float D = (( x1 - sx ) * ( y2 - sy )) -
				(( x2 - sx ) * ( y1 - sy ));
			float delta = powf( 1, 2 ) * d - powf( D, 2 );
			/*if ( delta >= 0 && player.active && med_asteroids[ j ].active ) {
				med_asteroids[ j ].shot = 1;
				med_asteroids[ j ].active = 0;
				player.active = 0;
			}*/
		}
		for ( int j = 0; j < ASTEROID_MAX * 4; j++ ) {
			float sx = boundX;//small_asteroids[ j ].x;
			float sy = 0;//small_asteroids[ j ].y;
			float d2 = powf( ( small_asteroids[ j ].x - x2 ), 2 ) +
				powf( ( small_asteroids[ j ].y - y2 ), 2 );
			if ( d2 < powf( ASTEROID_RADIUS * smallAsteroidScale, 2 ) && player.active && small_asteroids[ j ].active ) {
				small_asteroids[ j ].shot = 1;
				small_asteroids[ j ].active = 0;
				player.active = 0;
				player.shot = 1;
				lives--;
			}
			float D = (( x1 - sx ) * ( y2 - sy )) -
				(( x2 - sx ) * ( y1 - sy ));
			float delta = powf( 1, 2 ) * d - powf( D, 2 );
			/*if ( delta >= 0 && player.active && small_asteroids[ j ].active ) {
				small_asteroids[ j ].shot = 1;
				small_asteroids[ j ].active = 0;
				player.active = 0;
			}*/
		}
	}
}

void player_alien_shot_collision() {
	for ( int i = 0; i < 14; i++ ) {
		float x2, x1, y2, y1;
		if (i != 13) {
			x2 = playerOutlineVerticies[ playerOutlineFace[ i + 1 ] ][ 0 ];
			x1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 0 ];
			y2 = playerOutlineVerticies[ playerOutlineFace[ i + 1 ] ][ 1 ];
			y1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 1 ];
		} else if ( i == 13 ) {
			x2 = playerOutlineVerticies[ playerOutlineFace[ 0 ] ][ 0 ];
			x1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 0 ];
			y2 = playerOutlineVerticies[ playerOutlineFace[ 0 ] ][ 1 ];
			y1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 1 ];
		}
		x2 += player.x;
		x1 += player.x;
		y2 += player.y;
		y1 += player.y;
		for ( int j = 0; j < ALIEN_SHOT_MAX; j++ ) {
			for ( int k = 0; k < 8; k++ ) {
				float x4, y4;
				if ( k == 0 ) {
					x4 = alienShots[j].x;
					y4 = alienShots[j].y + rayLength;
				} else if ( k == 1 ) {
					x4 = alienShots[j].x + sqrt(rayLength);
					y4 = alienShots[j].y + sqrt(rayLength);
				} else if ( k == 2 ) {
					x4 = alienShots[j].x + rayLength;
					y4 = alienShots[j].y;
				} else if ( k == 3 ) {
					x4 = alienShots[j].x + sqrt(rayLength);
					y4 = alienShots[j].y - sqrt(rayLength);
				} else if ( k == 4 ) {
					x4 = alienShots[j].x;
					y4 = alienShots[j].y - rayLength;
				} else if ( k == 5 ) {
					x4 = alienShots[j].x - sqrt(rayLength);
					y4 = alienShots[j].y - sqrt(rayLength);
				} else if ( k == 6 ) {
					x4 = alienShots[j].x - rayLength;
					y4 = alienShots[j].y;
				} else if ( k == 7 ) {
					x4 = alienShots[j].x - sqrt(rayLength);
					y4 = alienShots[j].y + sqrt(rayLength);
				}
				float x3 = alienShots[j].x;
				float y3 = alienShots[j].y;
				float d = ( (y4 - y3) * (x2 - x1) ) -
							( (x4 - x3) * (y2 - y1) );
				float t1 = (( (x4 - x3) * (y1 - y3) ) -
							( (y4 - y3) * (x1 - x3) ))/d;
				float t2 = (( (x2 - x1) * (y1 - y3) ) -
							( (y2 - y1) * (x1 - x3) ))/d;
				if ( d != 0 && t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1 && alienShots[j].active == 1 ) {
					alienShots[j].active = 0;
					player.active = 0;
					player.shot = 1;
					lives--;
				}
			}
		}
		for ( int j = 0; j < ALIEN_SHOT_MAX; j++ ) {
			for ( int k = 0; k < 8; k++ ) {
				float x4, y4;
				if ( k == 0 ) {
					x4 = smallAlienShots[j].x;
					y4 = smallAlienShots[j].y + rayLength;
				} else if ( k == 1 ) {
					x4 = smallAlienShots[j].x + sqrt(rayLength);
					y4 = smallAlienShots[j].y + sqrt(rayLength);
				} else if ( k == 2 ) {
					x4 = smallAlienShots[j].x + rayLength;
					y4 = smallAlienShots[j].y;
				} else if ( k == 3 ) {
					x4 = smallAlienShots[j].x + sqrt(rayLength);
					y4 = smallAlienShots[j].y - sqrt(rayLength);
				} else if ( k == 4 ) {
					x4 = smallAlienShots[j].x;
					y4 = smallAlienShots[j].y - rayLength;
				} else if ( k == 5 ) {
					x4 = smallAlienShots[j].x - sqrt(rayLength);
					y4 = smallAlienShots[j].y - sqrt(rayLength);
				} else if ( k == 6 ) {
					x4 = smallAlienShots[j].x - rayLength;
					y4 = smallAlienShots[j].y;
				} else if ( k == 7 ) {
					x4 = smallAlienShots[j].x - sqrt(rayLength);
					y4 = smallAlienShots[j].y + sqrt(rayLength);
				}
				float x3 = smallAlienShots[j].x;
				float y3 = smallAlienShots[j].y;
				float d = ( (y4 - y3) * (x2 - x1) ) -
							( (x4 - x3) * (y2 - y1) );
				float t1 = (( (x4 - x3) * (y1 - y3) ) -
							( (y4 - y3) * (x1 - x3) ))/d;
				float t2 = (( (x2 - x1) * (y1 - y3) ) -
							( (y2 - y1) * (x1 - x3) ))/d;
				if ( d != 0 && t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1 && smallAlienShots[j].active == 1 ) {
					smallAlienShots[j].active = 0;
					player.active = 0;
					player.shot = 1;
					lives--;
				}
			}
		}
	}
}

void shot_alien_collision() {
	for ( int i = 0; i < 12; i++ ) {
		float x2, x1, y2, y1;
		if (i != 11) {
			x2 = alienOutlineVerticies[ alienOutlineFace[ i + 1 ] ][ 0 ];
			x1 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 0 ];
			y2 = alienOutlineVerticies[ alienOutlineFace[ i + 1 ] ][ 1 ];
			y1 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 1 ];
		} else if ( i == 11 ) {
			x2 = alienOutlineVerticies[ alienOutlineFace[ 0 ] ][ 0 ];
			x1 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 0 ];
			y2 = alienOutlineVerticies[ alienOutlineFace[ 0 ] ][ 1 ];
			y1 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 1 ];
		}
		x2 += alien.x;
		x1 += alien.x;
		y2 += alien.y;
		y1 += alien.y;
		for ( int j = 0; j < SHOT_MAX; j++ ) {
			for ( int k = 0; k < 8; k++ ) {
				float x4, y4;
				if ( k == 0 ) {
					x4 = shots[j].x;
					y4 = shots[j].y + rayLength;
				} else if ( k == 1 ) {
					x4 = shots[j].x + sqrt(rayLength);
					y4 = shots[j].y + sqrt(rayLength);
				} else if ( k == 2 ) {
					x4 = shots[j].x + rayLength;
					y4 = shots[j].y;
				} else if ( k == 3 ) {
					x4 = shots[j].x + sqrt(rayLength);
					y4 = shots[j].y - sqrt(rayLength);
				} else if ( k == 4 ) {
					x4 = shots[j].x;
					y4 = shots[j].y - rayLength;
				} else if ( k == 5 ) {
					x4 = shots[j].x - sqrt(rayLength);
					y4 = shots[j].y - sqrt(rayLength);
				} else if ( k == 6 ) {
					x4 = shots[j].x - rayLength;
					y4 = shots[j].y;
				} else if ( k == 7 ) {
					x4 = shots[j].x - sqrt(rayLength);
					y4 = shots[j].y + sqrt(rayLength);
				}
				float x3 = shots[j].x;
				float y3 = shots[j].y;
				float d = ( (y4 - y3) * (x2 - x1) ) -
							( (x4 - x3) * (y2 - y1) );
				float t1 = (( (x4 - x3) * (y1 - y3) ) -
							( (y4 - y3) * (x1 - x3) ))/d;
				float t2 = (( (x2 - x1) * (y1 - y3) ) -
							( (y2 - y1) * (x1 - x3) ))/d;
				if ( d != 0 && t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1 && shots[j].isOnScreen == 1 && alien.active == 1) {
					shots[j].isOnScreen = 0;
					alien.active = 0;
					alien.shot = 1;
					increment_score( SCORE_LARGE_ALIEN );
					player.aliensShot++;
				}
			}
		}
	}
}

void shot_small_alien_collision() {
	for ( int i = 0; i < 12; i++ ) {
		float x2, x1, y2, y1;
		if (i != 11) {
			x2 = smallAlienOutlineVerticies[ alienOutlineFace[ i + 1 ] ][ 0 ];
			x1 = smallAlienOutlineVerticies[ alienOutlineFace[ i ] ][ 0 ];
			y2 = smallAlienOutlineVerticies[ alienOutlineFace[ i + 1 ] ][ 1 ];
			y1 = smallAlienOutlineVerticies[ alienOutlineFace[ i ] ][ 1 ];
		} else if ( i == 11 ) {
			x2 = smallAlienOutlineVerticies[ alienOutlineFace[ 0 ] ][ 0 ];
			x1 = smallAlienOutlineVerticies[ alienOutlineFace[ i ] ][ 0 ];
			y2 = smallAlienOutlineVerticies[ alienOutlineFace[ 0 ] ][ 1 ];
			y1 = smallAlienOutlineVerticies[ alienOutlineFace[ i ] ][ 1 ];
		}
		x2 += smallAlien.x;
		x1 += smallAlien.x;
		y2 += smallAlien.y;
		y1 += smallAlien.y;
		for ( int j = 0; j < SHOT_MAX; j++ ) {
			for ( int k = 0; k < 8; k++ ) {
				float x4, y4;
				if ( k == 0 ) {
					x4 = shots[j].x;
					y4 = shots[j].y + rayLength;
				} else if ( k == 1 ) {
					x4 = shots[j].x + sqrt(rayLength);
					y4 = shots[j].y + sqrt(rayLength);
				} else if ( k == 2 ) {
					x4 = shots[j].x + rayLength;
					y4 = shots[j].y;
				} else if ( k == 3 ) {
					x4 = shots[j].x + sqrt(rayLength);
					y4 = shots[j].y - sqrt(rayLength);
				} else if ( k == 4 ) {
					x4 = shots[j].x;
					y4 = shots[j].y - rayLength;
				} else if ( k == 5 ) {
					x4 = shots[j].x - sqrt(rayLength);
					y4 = shots[j].y - sqrt(rayLength);
				} else if ( k == 6 ) {
					x4 = shots[j].x - rayLength;
					y4 = shots[j].y;
				} else if ( k == 7 ) {
					x4 = shots[j].x - sqrt(rayLength);
					y4 = shots[j].y + sqrt(rayLength);
				}
				float x3 = shots[j].x;
				float y3 = shots[j].y;
				float d = ( (y4 - y3) * (x2 - x1) ) -
							( (x4 - x3) * (y2 - y1) );
				float t1 = (( (x4 - x3) * (y1 - y3) ) -
							( (y4 - y3) * (x1 - x3) ))/d;
				float t2 = (( (x2 - x1) * (y1 - y3) ) -
							( (y2 - y1) * (x1 - x3) ))/d;
				if ( d != 0 && t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1 && shots[j].isOnScreen == 1 && smallAlien.active == 1) {
					shots[j].isOnScreen = 0;
					smallAlien.active = 0;
					smallAlien.shot = 1;
					increment_score( SCORE_SMALL_ALIEN );
					player.aliensShot++;
				}
			}
		}
	}
}

void player_alien_collision() {
	for ( int i = 0; i < 14; i++ ) {
		float x2, x1, y2, y1;
		if (i != 13) {
			x2 = playerOutlineVerticies[ playerOutlineFace[ i + 1 ] ][ 0 ];
			x1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 0 ];
			y2 = playerOutlineVerticies[ playerOutlineFace[ i + 1 ] ][ 1 ];
			y1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 1 ];
		} else if ( i == 13 ) {
			x2 = playerOutlineVerticies[ playerOutlineFace[ 0 ] ][ 0 ];
			x1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 0 ];
			y2 = playerOutlineVerticies[ playerOutlineFace[ 0 ] ][ 1 ];
			y1 = playerOutlineVerticies[ playerOutlineFace[ i ] ][ 1 ];
		}
		x2 += player.x;
		x1 += player.x;
		y2 += player.y;
		y1 += player.y;
		for ( int i = 0; i < 12; i++ ) {
			float x3, x4, y3, y4;
			if (i != 11) {
				x4 = alienOutlineVerticies[ alienOutlineFace[ i + 1 ] ][ 0 ];
				x3 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 0 ];
				y4 = alienOutlineVerticies[ alienOutlineFace[ i + 1 ] ][ 1 ];
				y3 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 1 ];
			} else if ( i == 11 ) {
				x4 = alienOutlineVerticies[ alienOutlineFace[ 0 ] ][ 0 ];
				x3 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 0 ];
				y4 = alienOutlineVerticies[ alienOutlineFace[ 0 ] ][ 1 ];
				y3 = alienOutlineVerticies[ alienOutlineFace[ i ] ][ 1 ];
			}
			x4 += alien.x;
			x3 += alien.x;
			y4 += alien.y;
			y3 += alien.y;
			float d = ( (y4 - y3) * (x2 - x1) ) -
						( (x4 - x3) * (y2 - y1) );
			float t1 = (( (x4 - x3) * (y1 - y3) ) -
						( (y4 - y3) * (x1 - x3) ))/d;
			float t2 = (( (x2 - x1) * (y1 - y3) ) -
						( (y2 - y1) * (x1 - x3) ))/d;
			if ( d != 0 && t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1 && alien.active == 1 ) {
				alien.active = 0;
				alien.shot = 1;
				player.active = 0;
				player.shot = 1;
				lives--;
			}
		}
	}
}

void reset_player()
{
	player.x = 0;
	player.y = 0;
	player.velX = 0;
	player.velY = 0;
	player.theta = 0;
	player.active = 1;
	player.shot = 0;
	player.explosionScale = 0;
}

void reset_alien()
{
	alien.x = 0;
	alien.y = 0;
	alien.active = 0;
	alien.shot = 0;
	alien.done = 1;
	alien.explosionScale = 0;
}

void reset_small_alien()
{
	smallAlien.x = 0;
	smallAlien.y = 0;
	smallAlien.active = 0;
	smallAlien.shot = 0;
	smallAlien.done = 1;
}

void reset_game() {
	startGame = 0;
	currentAsteroids = START_ASTEROIDS;
	reset_player();
	reset_alien();
	reset_small_alien();
	for ( int j = 0; j < SHOT_MAX; j++ )
		shots[ j ].isOnScreen = 0;
	for ( int j = 0; j < ALIEN_SHOT_MAX; j++ ) {
		alienShots[ j ].active = 0;
		smallAlienShots[ j ].active = 0;
	}
	for ( int j = 0; j < ASTEROID_MAX; j++) {
		asteroids[ j ].shot = 0;
		asteroids[ j ].active = 0;
		asteroids[ j ].explosionScale = 0;
	}
	for ( int j = 0; j < ASTEROID_MAX * 2; j++ ) {
		med_asteroids[ j ].shot = 0;
		med_asteroids[ j ].active = 0;
		med_asteroids[ j ].explosionScale = 0;
	}
	for ( int j = 0; j < ASTEROID_MAX * 4; j++ ) {
		small_asteroids[ j ].shot = 0;
		small_asteroids[ j ].active = 0;
		small_asteroids[ j ].explosionScale = 0;
	}
	lives = START_LIVES;
	score = 0;
	gameOverScale = 0;
	player.age = 0;
	player.accuracy = 0;
	player.aliensShot = 0;
	player.asteroidsShot = 0;
	player.shotsFired = 0;
	screenNum = 1;
}

void new_screen()
// This routine is called whenever a new screen is started (i.e. a new game or the
// player has destroyed all asteroids)
{
	int allDestroyed = 1;
	for ( int i = 0; i < currentAsteroids; i++) {
		if ( asteroids[ i ].shot == 0 )
			allDestroyed = 0;
	}
	for ( int i = 0; i < currentAsteroids * 2; i++ ) {
		if ( med_asteroids[ i ].shot == 0 )
			allDestroyed = 0;
	}
	for ( int i = 0; i < currentAsteroids * 4; i++ ) {
		if ( small_asteroids[ i ].shot == 0 )
			allDestroyed = 0;
	}
	if ( allDestroyed == 1 ) {
		reset_player();
		reset_alien();
		for ( int j = 0; j < SHOT_MAX; j++ )
			shots[ j ].isOnScreen = 0;
		for ( int j = 0; j < ALIEN_SHOT_MAX; j++ )
			alienShots[ j ].active = 0;
		for ( int j = 0; j < ASTEROID_MAX; j++) {
			asteroids[ j ].shot = 0;
			asteroids[ j ].explosionScale = 0;
		}
		for ( int j = 0; j < ASTEROID_MAX * 2; j++ ) {
			med_asteroids[ j ].shot = 0;
			med_asteroids[ j ].explosionScale = 0;
		}
		for ( int j = 0; j < ASTEROID_MAX * 4; j++ ) {
			small_asteroids[ j ].shot = 0;
			small_asteroids[ j ].explosionScale = 0;
		}
		if ( screenNum != 4 )
			screenNum++;
		player.age = 0;
	}
}

void increment_score( int amount )
{
	for (int i = 0; i < amount; i++ ) {
		score++;
		if ( score % BONUS_SHIP_MULTIPLE == 0 )
			lives++;
	}
}

void start_screen()
{
	int    f[ 30 ][ 3 ] = {		// Face vertex indices
		{ 5, 15, 4 },
		{ 6, 15, 5 },
		{ 15, 3, 4 },
		{ 6, 7, 15 },
		{ 7, 3, 15 },
		{ 2, 3, 7 },
		{ 14, 1, 2 },
		{ 2, 7, 14 },
		{ 7, 8, 14 },
		{ 9, 14, 8 },
		{ 0, 14, 9 },
		{ 1, 14, 0 },
		{ 0, 9, 13 },
		{ 9, 16, 13 },
		{ 11, 16, 9 },
		{ 12, 16, 11 },
		{ 10, 16, 12 },
		{ 13, 16, 10 },
		{ 10, 12, 11 },
		{ 0, 13, 10 },
		{ 10, 11, 0 },
		{ 11, 9, 8 },
		{ 8, 0, 11 },
		{ 2, 0, 8 },
		{ 1, 0, 2 },
		{ 2, 8, 6 },
		{ 8, 7, 6 },
		{ 6, 5, 4 },
		{ 4, 2, 6 },
		{ 3, 2, 4 }
	};	
	float  v[ 17 ][ 3 ] = {		// Vertex positions
		{ 0.4,  0,  0 },			//0
		{ 0,  1.5,  0 },			//1
		{ -0.4,  0,  0 },			//2
		{ -0.5,  1,  0 },			//3
		{ -0.75,  0,  0 },			//4
		{ -1.5,  -1,  0 },			//5
		{ -0.75,  -0.75,  0 },		//6
		{ -0.4,  -1,  0.7 },		//7
		{ 0,  -0.5,  0.45 },		//8
		{ 0.4,  -1,  0.7 },			//9
		{ 0.75,  0,  0 },			//10
		{ 0.75,  -0.75,  0 },		//11
		{ 1.5,  -1,  0 },			//12
		{ 0.5,  1,  0 },			//13
		{ 0,  0.4,  0.5 },			//14
		{ -0.6,  -0.25,  0.25 },	//15
		{ 0.6,  -0.25,  0.25 }		//16
	};	
	float  n[ 30 ][ 3 ];			// Face normals
	for ( int i = 0; i < 30; i++ ) {
		n[ i ][ 0 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 0 ];
		n[ i ][ 1 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 1 ];
		n[ i ][ 2 ] = compute_normal ( v[ f[ i ][ 0 ] ], v[ f[ i ][ 1 ] ], v[ f[ i ][ 2 ] ] )[ 2 ];
	}
	float mat[ 1 ][ 4 ] = { 0, 1, 0, 0 };	// Face material properties

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
  
	glPushMatrix();

	glTranslatef( 0, 0, 25 );
	glRotatef( alien_angle, 0, 1, 0 );	// Rotate constantly (using alien's rotation)
	for ( int i = 0; i < 30; i++ ) {		// For all faces
		glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat[0] );
		glBegin( GL_POLYGON );
		// Specify face normal
		glNormal3f( n[ i ][ 0], n[ i ][ 1 ], n[ i ][ 2 ] );
		for( int j = 0; j < 3; j++ ) {		// For all face vertices
			glVertex3fv( v[ f[ i ][ j ] ] );
		}
		glEnd();
	}
	glPopMatrix();

	char c[50];
	glPushMatrix();
	sprintf( c, "ASTEROIDS" );
	glTranslatef( -boundX/2 - 6, boundY/2, 0 );
	glScalef( 0.05, 0.05, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c[i]);
		c[i] = '\0';
	}
	glPopMatrix();

	glPushMatrix();
	sprintf( c, "(Right-click to start)" );
	glTranslatef( -boundX/2 - 2, -boundY/2, 0 );
	glScalef( 0.02, 0.02, 0 );
	for ( int i = 0; i < 50; i++ ) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c[i]);
		c[i] = '\0';
	}
	glPopMatrix();

	glFlush();
}

void movement()
{
	player_movement();
	alien_movement();
	small_alien_movement();
	alien_shot_movement();
	small_alien_shot_movement();
	shot_movement();
	asteroid_movement();
}

void wrap_screen()
{
	wrap_player();
	wrap_alien();
	wrap_small_alien();
	wrap_shots();
	wrap_asteroids();
}

void collisions()
{
	shot_asteroid_collision();
	shot_alien_collision();
	shot_small_alien_collision();
	player_asteroid_collision();
	player_alien_shot_collision();
	player_alien_collision();
}

void update( int value )
//  This routine updates every frame, ensuring that input is
//  shown on screen.
{
	if ( player.age >= ALIEN_TIME_TILL_SPAWN && alien.shot == 0 )
		alien.done = 0;
	if ( score > SMALL_ALIEN_SCORE_SPAWN && smallAlien.shot == 0 && alien.shot == 1 )
		smallAlien.done = 0;
	alien_angles();
	//scale_explosion();
	asteroid_angles();
	movement();
	wrap_screen();
	collisions();
	new_screen();
	if ( lives <= 0 )
		scale_game_over();
	glutPostRedisplay();
	glutTimerFunc( 25, update, 0 );
}

void main( int argc, char *argv[] )
{
	float  amb[] = { 0, 0, 0, 1 };	// Ambient material property
	float  lt_amb[] = { .2, .2, .2, 1 };	// Ambient light property
	float  lt_dif[] = { .8, .8, .8, 1 };	// Ambient light property
	float  lt_pos[] = { 0, .39392, .91914, 0 };	// Light position
	float  lt_spc[] = { 0, 0, 0, 1 };	// Specular light property

	player.velX = 0;
	player.velY = 0;
	player.active = 1;
	randomize_asteroid();				// Randomize points to create an asteroid

	glutInit( &argc, argv );		// Initialize GLUT
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( windowX, windowY );
	glutCreateWindow( "ASTEROID" );

	glutDisplayFunc( display_all );	// Setup GLUT callbacks
	glutKeyboardFunc( keyboard );
	glutKeyboardUpFunc( keyboard_release );
	glutSpecialFunc( keyboard_special );
	glutSpecialUpFunc( keyboard_special_release );

	glutCreateMenu( handle_menu );	// Setup GLUT popup menu
	glutAddMenuEntry( "New Game", 0 );
	glutAddMenuEntry( "Quit", 1 );
	glutAttachMenu( GLUT_RIGHT_BUTTON );
  
	glMatrixMode( GL_PROJECTION );	// Setup perspective projection
	glLoadIdentity();
	gluPerspective( 70, 1, 1, 40 );

	glMatrixMode( GL_MODELVIEW );		// Setup model transformations
  
	glLoadIdentity();
	gluLookAt( 0, 0, 35, 0, 0, -1, 0, 1, 0 );

	//  Set default ambient light in scene

	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb );

	//  Set Light 0 position, ambient, diffuse, specular intensities
	
	glLightfv( GL_LIGHT0, GL_POSITION, lt_pos );
	glLightfv( GL_LIGHT0, GL_AMBIENT, lt_amb );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lt_dif );
	glLightfv( GL_LIGHT0, GL_SPECULAR, lt_spc );

	//  Enable Light 0 and GL lighting

	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHTING );
	
	glShadeModel( GL_FLAT );		// Flat shading
	glEnable( GL_NORMALIZE );		// Normalize normals

	glClearDepth( 1.0 );			// Setup background colour
	glClearColor( 0, 0, 0, 0 );
	glEnable( GL_DEPTH_TEST );

	for ( int i = startAsteroids; i < ASTEROID_MAX; i++ ) {
		asteroids[i].active = 0;
	}

	startGame = 1;
  
	glutTimerFunc( 25, update, 0 );

	glutMainLoop();			// Enter GLUT main loop
}					// End mainline
