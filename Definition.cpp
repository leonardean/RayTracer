#include "Definition.h"

point operator + (const point&p, const vector3d &v){
	point p2={p.x + v.x, p.y + v.y, p.z + v.z };
	return p2;
}

point operator - (const point&p, const vector3d &v){
	point p2={p.x - v.x, p.y - v.y, p.z - v.z };
	return p2;
}

vector3d operator + (const vector3d&v1, const vector3d &v2){
	vector3d v={v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	return v;
}

vector3d operator - (const point&p1, const point &p2){
	vector3d v={p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
	return v;
}

vector3d operator * (float c, const vector3d &v)
{
	vector3d v2={v.x *c, v.y * c, v.z * c };
	return v2;
}

vector3d operator - (const vector3d&v1, const vector3d &v2){
	vector3d v={v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	return v;
}

float operator * (const vector3d&v1, const vector3d &v2 ) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vector3d operator ^ (const vector3d& v1, const vector3d& v2) {
    vector3d v = {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    return v;
}

color operator * (const color&c1, const color &c2 ) {
	color c = {c1.red * c2.red, c1.green * c2.green, c1.blue * c2.blue};
	return c;
}

color operator + (const color&c1, const color &c2 ) {
	color c = {c1.red + c2.red, c1.green + c2.green, c1.blue + c2.blue};
	return c;
}

color operator * (float coef, const color &c ) {
	color c2 = {c.red * coef, c.green * coef, c.blue * coef};
	return c2;
}