#ifndef __DEF_H
#define __DEF_H

//a point with three float numbers indicating its location in 3d world
struct point {
	float x, y, z;
};

//color structure with three float numbers indicating values
struct color {
    float red, green, blue;
};

//a 3d vector with three float numbers indicating its values in three dimentions
struct vector3d {
	float x, y, z;
};

point operator + (const point&p, const vector3d &v);

point operator - (const point&p, const vector3d &v);

vector3d operator + (const vector3d&v1, const vector3d &v2);

vector3d operator - (const point&p1, const point &p2);

vector3d operator * (float c, const vector3d &v);

vector3d operator - (const vector3d&v1, const vector3d &v2);

float operator * (const vector3d&v1, const vector3d &v2 );

vector3d operator ^ (const vector3d& v1, const vector3d& v2);

color operator * (const color&c1, const color &c2 );

color operator + (const color&c1, const color &c2 );

color operator * (float coef, const color &c );

#endif 
