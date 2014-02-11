#ifndef __RAYTRACE_H
#define __RAYTRACE_H

#include "Definition.h"

//material with reflection coefficient and color
struct material {
    float reflection;
	float red, green, blue;
	int mode;
	float power;
	float specular;
};

//sphere with origin, radius and material
struct sphere {
	point pos;
	float size;
	int materialId;
};

//plane with a point, normal and material
struct plane {
    point pos;
    vector3d normal;
    int materialId;
};

struct disc {
		point pos;
		float size;
		vector3d normal;
		int materialId;
};

//triangle with three points, normal and material
struct triangle{
    point v0,v1,v2;
    int materialId;
};

//light with a position and color intensity
struct light {
	point pos;
	color intensity;
};

//ray with a position and direction
struct ray {
	point start;
	vector3d dir;
};

#define invsqrtf(x) ((1.0f) / sqrtf(x))
#endif
