#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "chessboard.h"

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
float intersect_sphere(Point o, Vector u, Spheres *sph, Point *hit) {
  normalize(&u);
  Vector v = get_vec(sph->center, o);
  float a = pow(vec_len(u), 2);
  float b = 2 * (vec_dot(u, v));
  float c = pow(vec_len(v), 2) - pow(sph->radius, 2);

  //printf("a = %f, b = %f, c = %f\n", a, b, c);

  float discrim = pow(b, 2) - 4 * a * c;
  if ( discrim < 0 )
    return -1.0;

  float sqrt_dis = sqrt(discrim);
  float res1 = (-b + sqrt_dis)/(2 * a);
  float res2 = (-b - sqrt_dis)/(2 * a);
  float res;
  if ( res1 >= 0 && res1 < res2 )
  {
    res = res1;
    hit->x = o.x + res * u.x;
    hit->y = o.y + res * u.y;
    hit->z = o.z + res * u.z;
    return res;
  }
  else if ( res2 >= 0 && res2 <= res1)
  {
    res = res2;
    hit->x = o.x + res * u.x;
    hit->y = o.y + res * u.y;
    hit->z = o.z + res * u.z;
    return res;
  }
  return -1.0;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point o, Vector u, Spheres *sph, Point *hit, int sindex) {
  Spheres *closest_sph = NULL;
  Point closest_hit;
  float min = -1;

  while (sph != NULL)
  {
    // skips this sphere (used for shadows)
    if (sph->index == sindex)
    {
      sph = sph->next;
      continue;
    }

    Point hit;
    float intersect = intersect_sphere(o, u, sph, &hit);

    if ( intersect > 0 && (intersect < min || min < 0) )
    {
      min = intersect;
      closest_sph = sph;
      closest_hit = hit;
    }

    sph = sph->next;
  }

  if (hit != NULL)
    *hit = closest_hit;
	return closest_sph;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
		    float dif[], float spe[], float shine, 
		    float refl, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient)[0] = amb[0];
  (new_sphere->mat_ambient)[1] = amb[1];
  (new_sphere->mat_ambient)[2] = amb[2];
  (new_sphere->mat_diffuse)[0] = dif[0];
  (new_sphere->mat_diffuse)[1] = dif[1];
  (new_sphere->mat_diffuse)[2] = dif[2];
  (new_sphere->mat_specular)[0] = spe[0];
  (new_sphere->mat_specular)[1] = spe[1];
  (new_sphere->mat_specular)[2] = spe[2];
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = get_vec(sph->center, q);
  normalize(&rc);
  return rc;
}
