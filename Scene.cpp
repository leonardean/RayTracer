//this file contains functions to initialize the scene with primitives, light, and material information 
#include "Scene.h"
#include "Config.h"
#include "Raytrace.h"
#include <iostream>
using namespace std;

const vector3d NullVector = { 0.0f,0.0f,0.0f };
const point Origin = { 0.0f,0.0f,0.0f };
const SimpleString emptyString("");
const SimpleString diffuseString ("Diffuse");
const SimpleString intensityString ("Intensity");

//get material data from input file
void GetMaterial(const Config &sceneFile, material &currentMat);
//get sphere data from input file
void GetSphere(const Config &sceneFile, sphere &currentSph);
//get plane data from input file
void GetPlane(const Config& sceneFile, plane& currentPlane);
//get disc data from input file
void GetDisc(const Config& sceneFile, disc& currentDisc);
//get triangle data from input file
void GetTriangle(const Config& sceneFile, triangle& currentTriangle);
//get light data from input file
void GetLight(const Config &sceneFile, light &currentLight);
//get camera data from input file
void GetCamera(const Config& sceneFile, camera & currentCam);



void GetMaterial(const Config &sceneFile, material &currentMat)
{    
    //set material color
    {
        float fScalar;
        fScalar =  float(sceneFile.GetByNameAsFloat(diffuseString, 0.0f)); 
        vector3d vColor = {fScalar, fScalar, fScalar};
        vColor = sceneFile.GetByNameAsVector(diffuseString, vColor);
        currentMat.red   = vColor.x;
        currentMat.green = vColor.y;
        currentMat.blue  = vColor.z;
    }

    //set material reflection
    {
        float fScalar;
        fScalar =  float(sceneFile.GetByNameAsFloat("Reflection", 0.0f)); 
        currentMat.reflection = fScalar;
    }
    
    //set mode whether use lambert or phone or blinn
    {
    		int mode;
    		mode = sceneFile.GetByNameAsInteger("Mode", 0);
    		currentMat.mode = mode;
    }
    
    //set material power 
    {
    		float power;
    		power = sceneFile.GetByNameAsFloat("Power", 0.0f);
    		currentMat.power = power;
    }
    
    //set material specular
    {
    		float specular;
    		specular = sceneFile.GetByNameAsFloat("Specular", 0.0f);
    		currentMat.specular = specular;
    }
}

void GetSphere(const Config &sceneFile, sphere &currentSph)
{
    //set original point, radius and material
    currentSph.pos = sceneFile.GetByNameAsPoint("Center", Origin);
    currentSph.size =  float(sceneFile.GetByNameAsFloat("Size", 0.0f));
    currentSph.materialId = sceneFile.GetByNameAsInteger("Material.Id", 0); 
}

void GetPlane(const Config& sceneFile, plane& currentPlane)
{
    //set a point on the plane, normal and material
    vector3d defaultNormal = {0.0f, 0.0f, 1.0f};    
    currentPlane.pos = sceneFile.GetByNameAsPoint("Point", Origin);    
    currentPlane.normal = sceneFile.GetByNameAsVector("Normal", defaultNormal);    
    currentPlane.materialId = sceneFile.GetByNameAsInteger("Material.Id", 0);
}

void GetDisc(const Config& sceneFile, disc& currentDisc)
{
		currentDisc.pos = sceneFile.GetByNameAsPoint("Center", Origin);
    currentDisc.size =  float(sceneFile.GetByNameAsFloat("Size", 0.0f));
    vector3d defaultNormal = {0.0f, 0.0f, 1.0f};
    currentDisc.normal = sceneFile.GetByNameAsVector("Normal", defaultNormal); 
    currentDisc.materialId = sceneFile.GetByNameAsInteger("Material.Id", 0); 
}

void GetTriangle(const Config& sceneFile, triangle& currentTriangle)
{
    currentTriangle.v0 = sceneFile.GetByNameAsPoint("Point0", Origin);
    currentTriangle.v1 = sceneFile.GetByNameAsPoint("Point1", Origin);
    currentTriangle.v2 = sceneFile.GetByNameAsPoint("Point2", Origin);
    currentTriangle.materialId = sceneFile.GetByNameAsInteger("Material.Id", 0);
}

void GetLight(const Config &sceneFile, light &currentLight)
{
    //set position of the light
    currentLight.pos = sceneFile.GetByNameAsPoint("Position", Origin); 
    {
        //set intensity of three colors
        float fScalar;
        fScalar =  float(sceneFile.GetByNameAsFloat(intensityString, 0.0f)); 
        vector3d vColor = {fScalar, fScalar, fScalar};
        vColor = sceneFile.GetByNameAsVector(intensityString, vColor);
        currentLight.intensity.red   = vColor.x;
        currentLight.intensity.green = vColor.y;
        currentLight.intensity.blue  = vColor.z;
    }
}

void GetCamera(const Config& sceneFile, camera & currentCam)
{
    currentCam.horizontal = sceneFile.GetByNameAsFloat("Horizontal", 0.0f);
    currentCam.vertical = sceneFile.GetByNameAsFloat("Vertical", 0.0f);
    currentCam.depth = sceneFile.GetByNameAsFloat("Depth", 0.0f);
    currentCam.rotate= sceneFile.GetByNameAsVector("Rotate", NullVector);
}

bool init(char* inputName, scene &myScene)
{
    //initialize number of materials, spheres, planes and lights
	int nbMats, nbSpheres, nbPlanes, nbDiscs, nbTriangles, nbLights;
	int i;
	Config sceneFile(inputName);
    if (sceneFile.SetSection("Scene") == -1)
    {
		cout << "No Scene section." << endl;
		return false;
    }

    //set size of the scene
    myScene.sizex = sceneFile.GetByNameAsInteger("Image.Width", 640);
    myScene.sizey = sceneFile.GetByNameAsInteger("Image.Height", 480);

    //get number of primitives, lights, and materials
    nbMats = sceneFile.GetByNameAsInteger("NumberOfMaterials", 0);
    nbSpheres = sceneFile.GetByNameAsInteger("NumberOfSpheres", 0);
    nbPlanes = sceneFile.GetByNameAsInteger("NumberOfPlanes", 0);
    nbDiscs = sceneFile.GetByNameAsInteger("NumberOfDiscs", 0);
    nbLights = sceneFile.GetByNameAsInteger("NumberOfLights", 0);
    nbTriangles = sceneFile.GetByNameAsInteger("NumberOfTriangles", 0);

    //resize vector
		myScene.materialContainer.resize(nbMats);
		myScene.sphereContainer.resize(nbSpheres);
    myScene.planeContainer.resize(nbPlanes);
    myScene.discContainer.resize(nbDiscs);
		myScene.lightContainer.resize(nbLights);
    myScene.triangleContainer.resize(nbTriangles);

    //check scene input file and get materials
	for (i=0; i<nbMats; ++i)
    {   
        material &currentMat = myScene.materialContainer[i];
        SimpleString sectionName("Material");
        sectionName.append((unsigned long) i);
        if (sceneFile.SetSection( sectionName ) == -1)
        {
			cout << "Missing Material section." << endl;
		    return false;
        }
        GetMaterial(sceneFile, currentMat);
    }
    
    //check scene input file and get spheres
	for (i=0; i<nbSpheres; ++i)
    {   
        sphere &currentSphere = myScene.sphereContainer[i];
        SimpleString sectionName("Sphere");
        sectionName.append((unsigned long) i);
        if (sceneFile.SetSection( sectionName ) == -1)
        {
			cout << "Missing Sphere section." << endl;
		    return false;
        }
        GetSphere(sceneFile, currentSphere);
        if (currentSphere.materialId >= nbMats)
        {
			cout << "Material Id not valid." << endl;
		    return false;
        }
    }
    
    //check scene input file and get planes
    for (i = 0; i < nbPlanes; ++i) {
        plane& currentPlane = myScene.planeContainer[i];
        SimpleString sectionName("Plane");
        sectionName.append((unsigned long) i);
        if (sceneFile.SetSection(sectionName) == -1) {
            cout<<"Missing Plane section."<<endl;
            return false;
        }
        GetPlane(sceneFile, currentPlane);
        if (currentPlane.materialId >= nbMats) {
            cout<<"Material Id not valid."<<endl;
            return false;
        }
    }
    
    for (i = 0; i < nbDiscs; ++i) {
        disc& currentDisc = myScene.discContainer[i];
        SimpleString sectionName("Disc");
        sectionName.append((unsigned long) i);
        if (sceneFile.SetSection(sectionName) == -1) {
            cout<<"Missing Disc section."<<endl;
            return false;
        }
        GetDisc(sceneFile, currentDisc);
        if (currentDisc.materialId >= nbMats) {
            cout<<"Material Id not valid."<<endl;
            return false;
        }
    }
    
    //check scene input file and get triangles
    for (i = 0; i < nbTriangles; ++i) {
        triangle& currentTriangle = myScene.triangleContainer[i];
        SimpleString sectionName("Triangle");
        sectionName.append((unsigned long) i);
        if (sceneFile.SetSection(sectionName) == -1) {
            cout<<"Missing Triangle section."<<endl;
            return false;
        }
        GetTriangle(sceneFile, currentTriangle);
        if (currentTriangle.materialId >= nbMats) {
            cout<<"Material Id not valid."<<endl;
            return false;
        }
    }
    
    //check scene input file and get lights
	for (i=0; i<nbLights; ++i)
    {   
        light &currentLight = myScene.lightContainer[i];
        SimpleString sectionName("Light");
        sectionName.append((unsigned long) i);
        if (sceneFile.SetSection( sectionName ) == -1)
        {
			cout << "Missing Light section." << endl;
		    return false;
        }
        GetLight(sceneFile, currentLight);
        
    }

		{
		 camera &currentCam = myScene.cam;
		  SimpleString sectionName("Camera");
        if (sceneFile.SetSection( sectionName ) == -1)
        {
			cout << "Missing Camera section." << endl;
		    return false;
        }
        GetCamera(sceneFile, currentCam);
     }   
	return true;
}

