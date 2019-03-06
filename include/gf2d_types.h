#ifndef __GF2D_TYPES_H__
#define __GF2D_TYPES_H__

#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define GF2D_EPSILON   1e-6f
#define GF2D_ROOT2  1.414213562
#define GF2D_2PI 6.283185308
#define GF2D_PI 3.141592654
#define GF2D_HALF_PI 1.570796327
/* conversion factor for converting from radians to degrees*/
#define GF2D_RADTODEG  57.295779513082

/* conversion factor for converting from degrees to radians*/
#define GF2D_DEGTORAD  0.017453292519943295769236907684886

typedef short unsigned int Bool;

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

/**
 * @brief random macro taken from Id Software's Quake 2 Source.
 * This macro exposes the more random bits from the rand() function
 * @return a random float between 0 and 1.0
 */
#define gf2d_random()  ((rand ()%1000)/(float)1000.0)

/**
 * @brief random macro taken from Id Software's Quake 2 Source.
 * This macro exposes the more random bits from the rand() function
 * @return a random float between -1.0 and 1.0
 */
#define gf2d_crandom() (((float)((rand()%1000)/(float)1000.0) * 2.0) - 1.0)

/**
 * basic operations
 */
#ifndef MIN
#define MIN(a,b)          (a<=b?a:b)
#endif

#ifndef MAX
#define MAX(a,b)          (a>=b?a:b)
#endif

#define gf2d_rect_set(r,a,b,c,d) (r.x=(a), r.y=(b), r.w=(c), r.h=(d))

#endif
