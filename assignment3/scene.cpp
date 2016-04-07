//
// this provide functions to set up the scene
//
#include "sphere.h"
#include <stdio.h>

extern Point light1;
extern float light1_intensity[3];

extern float global_ambient[3];
extern Spheres *scene;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void set_up_default_scene() {
	// set background color
	background_clr.r = 0.5;
	background_clr.g = 0.05;
	background_clr.b = 0.8;

	// set up global ambient term
	global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

	// set up light 1
	light1.x = -2.0;
	light1.y = 5.0;
	light1.z = 1.0;
	light1_intensity[0] = light1_intensity[1] = light1_intensity[2] = 1.0;

	// set up decay parameters
	decay_a = 0.5;
	decay_b = 0.3;
	decay_c = 0.0;

	// sphere 1
	Point sphere1_ctr = {1.5, -0.2, -3.2};
	float sphere1_rad = 1.23;
	float sphere1_ambient[] = {0.7, 0.7, 0.7};
	float sphere1_diffuse[] = {0.1, 0.5, 0.8};
	float sphere1_specular[] = {1.0, 1.0, 1.0};
	float sphere1_shineness = 10;
	float sphere1_reflectance = 0.4;
	float sphere1_refract = 1.5;
	float sphere1_trans = 0.7;
	scene = add_sphere(scene, sphere1_ctr, sphere1_rad, sphere1_ambient,
			sphere1_diffuse, sphere1_specular, sphere1_shineness,
			sphere1_reflectance, sphere1_refract, sphere1_trans, 1);

	// sphere 2
	Point sphere2_ctr = {-1.5, 0.0, -3.5};
	float sphere2_rad = 1.5;
	float sphere2_ambient[] = {0.6, 0.6, 0.6};
	float sphere2_diffuse[] = {1.0, 0.0, 0.25};
	float sphere2_specular[] = {1.0, 1.0, 1.0};
	float sphere2_shineness = 6;
	float sphere2_reflectance = 0.3;
	float sphere2_refract = 1.5;
	float sphere2_trans = 0.7;
	scene = add_sphere(scene, sphere2_ctr, sphere2_rad, sphere2_ambient,
			sphere2_diffuse, sphere2_specular, sphere2_shineness,
			sphere2_reflectance, sphere2_refract, sphere2_trans, 2);

	// sphere 3
	Point sphere3_ctr = {-0.35, 1.75, -2.25};
	float sphere3_rad = 0.5;
	float sphere3_ambient[] = {0.2, 0.2, 0.2};
	float sphere3_diffuse[] = {0.0, 1.0, 0.25};
	float sphere3_specular[] = {0.0, 1.0, 0.0};
	float sphere3_shineness = 30;
	float sphere3_reflectance = 0.3;
	float sphere3_refract = 1.5;
	float sphere3_trans = 0.7;
	scene = add_sphere(scene, sphere3_ctr, sphere3_rad, sphere3_ambient,
			sphere3_diffuse, sphere3_specular, sphere3_shineness,
			sphere3_reflectance, sphere3_refract, sphere3_trans, 3);
}

/***************************************
 * You can create your own scene here
 ***************************************/
void set_up_user_scene() {
	// set background color
	background_clr.r = 0.2;
	background_clr.g = 0.2;
	background_clr.b = 0.4;

	// set up global ambient term
	global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.15;

	// set up light 1
	light1.x = -1.5;
	light1.y = 1.0;
	light1.z = 0.0;
	light1_intensity[0] = light1_intensity[1] = light1_intensity[2] = 1.0;

	// set up decay parameters
	decay_a = 0.5;
	decay_b = 0.2;
	decay_c = 0.0;

	// sphere 1 - blue
	Point sphere1_ctr = {-1.0, 0.75, -3.0};
	float sphere1_rad = 1.0;
	float sphere1_ambient[] = {0.3, 0.3, 0.6};
	float sphere1_diffuse[] = {0.1, 0.5, 0.8};
	float sphere1_specular[] = {1.0, 1.0, 1.0};
	float sphere1_shineness = 10;
	float sphere1_reflectance = 0.4;
	float sphere1_refract = 1.2;
	float sphere1_trans = 0.7;
	scene = add_sphere(scene, sphere1_ctr, sphere1_rad, sphere1_ambient,
			sphere1_diffuse, sphere1_specular, sphere1_shineness,
			sphere1_reflectance, sphere1_refract, sphere1_trans, 1);

	// sphere 2 - red
	Point sphere2_ctr = {0.0, 0.0, -4.0};
	float sphere2_rad = 1.0;
	float sphere2_ambient[] = {0.6, 0.3, 0.3};
	float sphere2_diffuse[] = {1.0, 0.0, 0.25};
	float sphere2_specular[] = {1.0, 0.8, 0.8};
	float sphere2_shineness = 10;
	float sphere2_reflectance = 0.3;
	float sphere2_refract = 0.0;
	float sphere2_trans = 0.7;
	scene = add_sphere(scene, sphere2_ctr, sphere2_rad, sphere2_ambient,
			sphere2_diffuse, sphere2_specular, sphere2_shineness,
			sphere2_reflectance, sphere2_refract, sphere2_trans, 2);

	// sphere 3 - green
	Point sphere3_ctr = {1.0, -0.75, -5.0};
	float sphere3_rad = 1.0;
	float sphere3_ambient[] = {0.3, 0.6, 0.3};
	float sphere3_diffuse[] = {0.0, 1.0, 0.25};
	float sphere3_specular[] = {0.0, 1.0, 0.0};
	float sphere3_shineness = 30;
	float sphere3_reflectance = 0.3;
	float sphere3_refract = 1.2;
	float sphere3_trans = 0.7;
	scene = add_sphere(scene, sphere3_ctr, sphere3_rad, sphere3_ambient,
			sphere3_diffuse, sphere3_specular, sphere3_shineness,
			sphere3_reflectance, sphere3_refract, sphere3_trans, 3);

	// sphere 4 - yellow
	Point sphere4_ctr = {1.5, 0.3, -2.6};
	float sphere4_rad = 1.0;
	float sphere4_ambient[] = {0.7, 0.7, 0.1};
	float sphere4_diffuse[] = {0.9, 0.9, 0.1};
	float sphere4_specular[] = {0.8, 0.8, 0.0};
	float sphere4_shineness = 20;
	float sphere4_reflectance = 0.7;
	float sphere4_refract = 1.2;
	float sphere4_trans = 0.9;
	scene = add_sphere(scene, sphere4_ctr, sphere4_rad,
			sphere4_ambient, sphere4_diffuse, sphere4_specular,
			sphere4_shineness, sphere4_reflectance,
			sphere4_refract, sphere4_trans, 4);

	// sphere 5 - purple
	Point sphere5_ctr = {-2.0, -1.2, -5.5};
	float sphere5_rad = 1.4;
	float sphere5_ambient[] = {0.6, 0.1, 0.6};
	float sphere5_diffuse[] = {0.6, 0.0, 0.9};
	float sphere5_specular[] = {0.6, 0.3, 0.8};
	float sphere5_shineness = 100;
	float sphere5_reflectance = 0.1;
	float sphere5_refract = 1.5;
	float sphere5_trans = 0.7;
	scene = add_sphere(scene, sphere5_ctr, sphere5_rad,
			sphere5_ambient, sphere5_diffuse, sphere5_specular,
			sphere5_shineness, sphere5_reflectance,
			sphere5_refract, sphere5_trans, 5);

	// sphere 6 - orange
	Point sphere6_ctr = {1.3, 0.5, -3.7};
	float sphere6_rad = 0.8;
	float sphere6_ambient[] = {0.8, 0.4, 0.1};
	float sphere6_diffuse[] = {0.9, 0.4, 0.1};
	float sphere6_specular[] = {0.8, 0.8, 0.2};
	float sphere6_shineness = 50;
	float sphere6_reflectance = 0.7;
	float sphere6_refract = 1.5;
	float sphere6_trans = 0.7;
	scene = add_sphere(scene, sphere6_ctr, sphere6_rad,
			sphere6_ambient, sphere6_diffuse, sphere6_specular,
			sphere6_shineness, sphere6_reflectance,
			sphere6_refract, sphere6_trans, 6);
}
