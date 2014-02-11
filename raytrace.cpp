#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
using namespace std;

#include "Raytrace.h"
#include "Scene.h"

bool hitSphere(const ray &r, const sphere &s, float &t);
bool draw(char* outputName, scene &myScene) ;
bool hitPlane(const ray& r, const plane& p, float& t);
bool hitTriangle(const ray& r, const triangle& tr, float& t);
bool hitDisc(const ray& r, const disc& d, float& t);

//whether disc is hit by ray
bool hitDisc(const ray& r, const disc& d, float& t) {
		bool retvalue = false;
		
		// whether the plane hit by ray or not
		float dist = (d.pos - r.start) * d.normal / (r.dir * d.normal);
		point hitpoint = r.start + dist * r.dir;
		
		// whether the hitpoint is in the disc
		float r2 = (hitpoint - d.pos) * (hitpoint - d.pos);
		if ((dist > 0.1f) && (dist <= t)) {
        if (r2 <= d.size * d.size)
        {
             t = dist;
             retvalue = true;
        }
    }
    return retvalue; 		
}

//whether plane is hit by ray
bool hitPlane(const ray& r, const plane& p, float& t) {
    bool retvalue = false;
    
    // The implementation is based on the following equation:
    // t = -(N.P + d) / N .D 
    
    float dist = (p.pos - r.start) * p.normal / (r.dir * p.normal);
    if ((dist > 0.1f) && (dist <= t)) {
        t = dist;
        retvalue = true;
    }
    return retvalue;  
}

//whether sphere is hit by ray
bool hitSphere(const ray &r, const sphere &s, float &t) 
 { 
    vector3d dist = s.pos - r.start; 
    float B = r.dir * dist;
    float D = B*B - dist * dist + s.size * s.size; 
    
    // The implementation is based on the following equations:
    // a*t2 + b*t + c = 0
    // a = D.D = 1, b = 2 * D.P, c = P.P - R * R    
    
    if (D < 0.0f) 
        return false; 
    float t0 = B - sqrtf(D); 
    float t1 = B + sqrtf(D);
    bool retvalue = false;  
    if ((t0 > 0.1f) && (t0 <= t)) 
    {
        
        t = t0;
        retvalue = true; 
    } 
    if ((t1 > 0.1f) && (t1 <= t)) 
    {
    
        t = t1; 
        retvalue = true; 
    }
    return retvalue; 
 }

//where triangle is hit by ray
bool hitTriangle(const ray& ray, const triangle& tr, float& tmin)
{
		// the implementation is based on the book "raytracing from the ground up" pp.479
    float a = tr.v0.x - tr.v1.x, b = tr.v0.x - tr.v2.x, c = ray.dir.x, d = tr.v0.x - ray.start.x;
    float e = tr.v0.y - tr.v1.y, f = tr.v0.y - tr.v2.y, g = ray.dir.y, h = tr.v0.y - ray.start.y;
    float i = tr.v0.z - tr.v1.z, j = tr.v0.z - tr.v2.z, k = ray.dir.z, l = tr.v0.z - ray.start.z;
    
    float m = f * k - g * j, n = h * k - g * l, p = f * l - h *j;
    float q = g * i - e * k, s = e * j - f * i;
    
    float inv_denom = 1.0 / (a * m + b * q + c * s);
    
    float e1 = d * m - b * n - c * p;
    float beta = e1 * inv_denom;
    
    if (beta < 0.0) {
        return false;
    }
    
    float r = e * l - h * i;
    float e2 = a * n + d * q + c * r;
    float gamma = e2 * inv_denom;
    
    if (gamma < 0.0) {
        return false;
    }
    if (beta + gamma > 1.0) {
        return false;
    }
    float e3 = a * p - b * r + d * s;
    float t = e3 * inv_denom;
    
    if (t < 0.1) {
        return false;
    }
    
    
    tmin = t;
    return true;
}

ray camera_model(float fragmentx, float fragmenty, float depth, vector3d direction, scene &myScene)
{
	//move the camera
	fragmentx = fragmentx + myScene.cam.vertical;
	fragmenty = fragmenty + myScene.cam.horizontal;
	depth = depth + myScene.cam.depth;
	vector3d rotate = myScene.cam.rotate; 
	vector3d new_direction = rotate + direction;
	ray viewray = {{fragmentx, fragmenty, depth}, new_direction};
	return viewray;
}

color addRay(ray viewRay, scene &myScene)
{
	color output = {0.0f, 0.0f, 0.0f}; 
	float coef = 1.0f;
	int level = 0;
			do 
            { 
                float t = 2000.0f;      //the farthest place I concern
                int currentSphere= -1;
                int currentPlane = -1;
                int currentTriangle = -1;
                int currentDisc = -1;
                
                // which object is hit and choose the closet one//
                for (unsigned int i = 0; i < myScene.triangleContainer.size(); ++i) {
                    if (hitTriangle(viewRay, myScene.triangleContainer[i], t)) {
                        currentTriangle = i;
                    }
                }
                for (unsigned int i = 0; i < myScene.sphereContainer.size(); ++i) 
                { 
                    if (hitSphere(viewRay, myScene.sphereContainer[i], t)) 
                    {
                        currentSphere = i;
                        currentTriangle = -1;
                    }
                }
                for (unsigned int i = 0; i < myScene.planeContainer.size(); ++i) {
                    if (hitPlane(viewRay, myScene.planeContainer[i], t)) {
                        currentPlane = i;
                        currentSphere = -1;
                        currentTriangle = -1;
                    }
                }
                for (unsigned int i = 0; i < myScene.discContainer.size(); ++i) {
                    if (hitDisc(viewRay, myScene.discContainer[i], t)) {
                        currentDisc = i;
                        currentPlane = -1;
                        currentSphere = -1;
                        currentTriangle = -1;
                    }
                }
                
                
                
                point newStart = viewRay.start + t * viewRay.dir; 
                //initialize final normal
                vector3d n = {0.0, 0.0, 0.0};
                material currentMat = {0.0, 0.0, 0.0, 0.0};
                
                
                //Calculate different vNormal depends on different objects
                // Get the material
                if (currentSphere != -1) {          
                    n = newStart - myScene.sphereContainer[currentSphere].pos;
                    float temp = n * n;
                    if (temp == 0.0f) 
                        break; 
                
                    temp = 1.0f / sqrtf(temp); 
                    n = temp * n; 
                    currentMat = myScene.materialContainer[myScene.sphereContainer[currentSphere].materialId];
                }                
                else if (currentPlane != -1){
                    n = myScene.planeContainer[currentPlane].normal;
                    currentMat = myScene.materialContainer[myScene.planeContainer[currentPlane].materialId];
                    }
                else if (currentDisc != -1){
                		n = myScene.discContainer[currentDisc].normal;
                		currentMat = myScene.materialContainer[myScene.discContainer[currentDisc].materialId];
                }
                else if (currentTriangle != -1) {
                    n = (myScene.triangleContainer[currentTriangle].v1 - myScene.triangleContainer[currentTriangle].v0) ^ (myScene.triangleContainer[currentTriangle].v2 - myScene.triangleContainer[currentTriangle].v0);
                    n.x = -n.x;
                    n.y = -n.y;
                    n.z = -n.z;
                    float length = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
                    n.x /= length;
                    n.y /= length;
                    n.z /= length;
                    currentMat = myScene.materialContainer[myScene.triangleContainer[currentTriangle].materialId];
                }
                
               
               
                //calculate the value at the point of illumination
                for (unsigned int j = 0; j < myScene.lightContainer.size(); ++j) {
                    
                    light currentLight = myScene.lightContainer[j];
                    vector3d dist = currentLight.pos - newStart;
                    
                    if (n * dist <= 0.0f)
                        continue;
                   
                    float t = sqrtf(dist * dist);
                    if ( t <= 0.0f )
                        continue;
                    ray lightRay;
                    lightRay.start = newStart;
                    lightRay.dir = (1/t) * dist;
                    
                    // computation of the shadows
                    bool inShadow = false; 
                    
                    for (unsigned int i = 0; i < myScene.sphereContainer.size(); ++i) {
                        if (hitSphere(lightRay, myScene.sphereContainer[i], t)) {
                            inShadow = true;
                            break;
                        }
                    }
                    for (unsigned int i = 0; i < myScene.planeContainer.size(); ++i) {
                        if (hitPlane(lightRay, myScene.planeContainer[i], t)) {
                            inShadow = true;
                            break;
                        }
                    }
                    
                    for (unsigned int i = 0; i < myScene.triangleContainer.size(); ++i) {
                        if (hitTriangle(lightRay, myScene.triangleContainer[i], t)) {                            
                            inShadow = true;
                            break;
                        }
                    }
                    
                    for (unsigned int i = 0; i < myScene.discContainer.size(); ++i) {
                        if (hitDisc(lightRay, myScene.discContainer[i], t)) {                            
                            inShadow = true;
                            break;
                        }
                    }
                    if (!inShadow) {
                        //lambert
                        float lambert = (lightRay.dir * n) * coef;
                        output.red += lambert * currentLight.intensity.red * currentMat.red;
                        output.green += lambert * currentLight.intensity.green * currentMat.green;
                        output.blue += lambert * currentLight.intensity.blue * currentMat.blue;
                        
                        if (currentMat.mode == 2)
                        {                            
                            //Phong
                            float reflet = 2.0f * (lightRay.dir * n);
                            vector3d phongDir = lightRay.dir - reflet * n;
                            float phongTerm = max(phongDir * viewRay.dir, 0.0f) ;
                            phongTerm = currentMat.specular * powf(phongTerm, 60) * coef;
                            output.red += phongTerm * currentLight.intensity.red;
                            output.green += phongTerm * currentLight.intensity.green;
                            output.blue += phongTerm * currentLight.intensity.blue;
                        }
                        else if (currentMat.mode == 1){
                            //Blinn
                            float fViewProjection = viewRay.dir * n;
														vector3d blinnDir = lightRay.dir - viewRay.dir;
														float fLightProjection = lightRay.dir * n;
														float temp = blinnDir * blinnDir;
														if (temp != 0.0f )
														{
																float blinn = invsqrtf(temp) * max(fLightProjection - fViewProjection , 0.0f);
																blinn = coef * powf(blinn, currentMat.power);
																output.red += blinn *currentMat.specular * currentLight.intensity.red;
                                output.green += blinn *currentMat.specular * currentLight.intensity.green;
                                output.blue += blinn *currentMat.specular * currentLight.intensity.blue;
														}
                        }
                    }
                }
                
                //compute the reflection
                coef *= currentMat.reflection;
                float reflet = 2.0f * (viewRay.dir * n);
                viewRay.start = newStart;
                viewRay.dir = viewRay.dir - reflet * n;
                
                level++;
            }
            while ((coef > 0.0f) && (level < 15));   
            return output;
}

bool draw(char* outputName, scene &myScene) 
{
    ofstream imageFile(outputName,ios_base::binary);
    if (!imageFile)
        return false; 
    
    imageFile.put(0).put(0);
    imageFile.put(2);       
    
    imageFile.put(0).put(0);
    imageFile.put(0).put(0);
    imageFile.put(0);
    
    imageFile.put(0).put(0); 
    imageFile.put(0).put(0); 
    
    imageFile.put((unsigned char)(myScene.sizex & 0x00FF)).put((unsigned char)((myScene.sizex & 0xFF00) / 256));
    imageFile.put((unsigned char)(myScene.sizey & 0x00FF)).put((unsigned char)((myScene.sizey & 0xFF00) / 256));
    imageFile.put(24);       
    imageFile.put(0); 
    
    
    for (int y = 0; y < myScene.sizey; ++y) { 
        for (int x = 0; x < myScene.sizex; ++x) {
            //color values
            float red = 0, green = 0, blue = 0;
            float coef = 1.0f;
            //depth of reflection
            int level = 0; 
            float depth = -500.0f;
				    vector3d direction = {0.0f, 0.0f, 1.0f};
            ray viewRay =  camera_model((float)x, (float)y, depth, direction, myScene);
            color temp = addRay (viewRay, myScene);
            
            
            imageFile.put((unsigned char)min(temp.blue*255.0f,255.0f))
            .put((unsigned char)min(temp.green*255.0f, 255.0f))
            .put((unsigned char)min(temp.red*255.0f, 255.0f));
        }
    }
    return true;
}



int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << "Usage : Raytrace.exe Scene.txt Output.tga" << endl;
        return -1;
    }
    scene myScene;
    if (!init(argv[1], myScene))
    {
        cout << "Failure when reading the Scene file." << endl;
        return -1;
    }
    if (!draw(argv[2], myScene))
    {
        cout << "Failure when creating the image file." << endl;
        return -1;
    }
    return 0;
}
