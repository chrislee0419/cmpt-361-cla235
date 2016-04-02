#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern float light1_intensity[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 * q = point on surface
 * v = vector from view to point q
 * n = normal vector of point q
 * sph = sphere containing point q
 *********************************************************************/
RGB_float phong(Point q, Vector v, Vector n, Spheres *sph) {
	RGB_float final_colour, glo_amb, amb, spec, dif;
  Vector l, r;

  // vector setup
  l = get_vec(q, light1);
  normalize(&l);
  normalize(&v);
  normalize(&n);
  r = vec_minus(vec_scale(n, 2*vec_dot(n, l)), l);
  normalize(&r);

  amb.r = sph->mat_ambient[0] * light1_intensity[0];
  amb.g = sph->mat_ambient[1] * light1_intensity[1];
  amb.b = sph->mat_ambient[2] * light1_intensity[2];

  float dis = vec_len(get_vec(q, light1));
  float decay = 1.0 / (decay_a + decay_b*dis + decay_c*dis*dis);

  float n_dot_l = vec_dot(n, l);
  if (n_dot_l < 0.0)
    n_dot_l = -n_dot_l;
  dif.r = sph->mat_diffuse[0] * light1_intensity[0] * n_dot_l;
  dif.g = sph->mat_diffuse[1] * light1_intensity[1] * n_dot_l;
  dif.b = sph->mat_diffuse[2] * light1_intensity[2] * n_dot_l;

  float v_dot_r = pow(vec_dot(v, r), sph->mat_shineness);
  if (v_dot_r < 0.0)
    v_dot_r = -v_dot_r;
  spec.r = sph->mat_specular[0] * light1_intensity[0] * v_dot_r;
  spec.g = sph->mat_specular[1] * light1_intensity[1] * v_dot_r;
  spec.b = sph->mat_specular[2] * light1_intensity[2] * v_dot_r;

  if (amb.r > 1.0 || amb.r < 0.0 || dif.r < 0.0 || dif.r > 1.0
    || spec.r < 0.0 || spec.r > 1.0)
    printf("red: a = %f, d = %f, s = %f\n", amb.r, dif.r, spec.r);
  if (amb.g > 1.0 || amb.g < 0.0 || dif.g < 0.0 || dif.g > 1.0
    || spec.g < 0.0 || spec.g > 1.0)
    printf("grn: a = %f, d = %f, s = %f\n", amb.g, dif.g, spec.g);
  if (amb.b > 1.0 || amb.b < 0.0 || dif.b < 0.0 || dif.b > 1.0
    || spec.b < 0.0 || spec.b > 1.0)
    printf("blu: a = %f, d = %f, s = %f\n", amb.b, dif.b, spec.b);

  final_colour.r = amb.r + decay * (dif.r + spec.r);
  final_colour.g = amb.g + decay * (dif.g + spec.g);
  final_colour.b = amb.b + decay * (dif.b + spec.b);

	return final_colour;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point origin, Vector ray) {
	RGB_float colour = {0};
  Point hit;

  Spheres *sph = intersect_scene(origin, ray, scene, &hit);
  if (sph != NULL)
  {
    RGB_float glo_amb;
    glo_amb.r = global_ambient[0];
    glo_amb.g = global_ambient[1];
    glo_amb.b = global_ambient[2];

    RGB_float phong_colour = phong(hit, get_vec(eye_pos, hit), sphere_normal(hit, sph), sph);
  
    colour.r += glo_amb.r + phong_colour.r;
    colour.g += glo_amb.g + phong_colour.g;
    colour.b += glo_amb.b + phong_colour.b;
  }
  else
    colour = background_clr;


	return colour;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  Vector ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = get_vec(eye_pos, cur_pixel_pos);

      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(cur_pixel_pos, ray);

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

// Checkboard for testing
//RGB_float clr = {float(i/32), 0, float(j/32)};
//ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
