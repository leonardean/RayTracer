RayTracer
=========

A simple ray tracer that renders geometries from an input text scene file
##Framework
Camera mode was achieved with the ability to transform the scene including moving up, down, left, right and rotation. This is achieved by changing starting point and direction of the ray.
![frame](http://www.mftp.info/20140202/1392086128x1927178161.png)

The application is well structured and was coded very object oriented to facilitate adding additional primitives. Structures were created to represent objects such as point, vector, color, light, ray, material, sphere, and plane. The application is able to read a text file containing scene description (the format of the description file can be seen in the sample file) and output an image file.

##Simple Primitives
Sphere, plane and triangle can be produced by the application. The hit functions were adapted from the recommended book (ray tracing from the ground up). With the hit functions the process to produce such images can be described with such pseudo code:
```
for each pixel of the scene{
  Final color = 0;
  Ray = { starting point, direction }; 
  for each object in the scene {
    determine closest object using hit functions; 
  }
  Final color = light intensity * object color;
  put the Final color into the image file; 
}
```
![primitives](http://www.mftp.info/20140202/1392086218x1927178161.png)
##Simple materials

Lambert and phong materials with reflection can be assigned to primitives as shown below:

![material](http://www.mftp.info/20140202/1392086286x1927178161.png)

Lambert material pseudo code:
```
for each pixel of the scene{
  Final color = 0;
  Final normal = {0, 0, 0};
  Ray = { starting point, direction }; 
  for each object in the scene {
    determine closest object using hit functions; 
  }
  Final color = light intensity * object color;
  Final normal = normal of the closest object;
  calculate lambert coefficient (Ray.direction, Final normal); 
  Final color *= lambert coefficient;
  put the Final color into the image file;
}
```
Phone material pseudo code:
```
for each pixel of the scene{
  Final color = 0;
  Final normal = {0, 0, 0};
  Reflection factor = 1; 
  Repeat {
    Ray = { starting point, direction }; 
    for each object in the scene {
      determine closest object using hit functions; 
    }
    Final color = light intensity * object color;
    Final normal = normal of the closest object;
    calculate lambert coefficient (Ray.direction, Final normal);
    Final color *= lambert coefficient;
    calculate phong coefficient (Ray.direction, Final normal, specular);￼￼
    Final color += phong coefficient * light intensity; 
    reflection factor *= material reflection coefficient; 
    reassign Ray;
    increment depth;
  } until reflection factor is 0 or maximum depth is reached;
  put the Final color into the image file; 
}
```
##Complex Materials
The application enables adding multiple point lights and setting their positions and intensities of colors in the scene data file.

Shadows can be generated once a light source is blocked between it and the surface being illuminated. Picture bellows shows a resulting image with the use of multiple lights and shadows.

![shadow](http://www.mftp.info/20140202/1392086509x1927178161.png)

When dealing with point lights instead of ambient light, only the color value that is at point of illumination needs to be taken into consideration of all kinds of calculations in previous pseudo code. To do that, rays that represent point lights need to be casted. By using hit functions, whether a position in the scene is illuminated can be obtained. The rest positions with no color assigning naturally become shadow area.

##Compilation and Running
To compile:
```
g++ *.cpp
```
To run:
```
./a.out arg0 arg1 arg2 arg3 arg4 arg5 arg6 arg7 
where arg0 is scene data file name
      arg1 is output image file name arg2 is move up scale
      arg3 is move right scale
      arg4 is move close scale
      arg5 is rotate horizontal scale
      arg6 is rotate vertical scale
      arg7 indicate if the user want primitives to be lambert or phong
          (1 means phong and 0 means lambert)
An example would be: ./a.out scene.txt output.jpeg 100 100 0 -0.2 0.3 0
```
