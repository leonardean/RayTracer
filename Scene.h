#ifndef __SCENE_H
#define __SCENE_H
#include <vector>
#include "raytrace.h"

struct camera{
	float horizontal;
	float vertical;
	float depth;
	vector3d rotate;
	};

struct scene {
    //vectors to store multiple primitives, materials and lights
    std::vector<material> materialContainer;
    std::vector<sphere>   sphereContainer;
    std::vector<plane>    planeContainer;
		std::vector<light>    lightContainer;
    std::vector<triangle> triangleContainer;
    std::vector<disc>			discContainer;	
    int sizex, sizey;
    camera cam;
};

bool init(char* inputName, scene &myScene);

#endif
