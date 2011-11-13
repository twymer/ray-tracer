#include "lab6.h"
#include <math.h>

//Reference site:
//http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection
bool intersectSphere(Ray ray, HitData* hd, float radius){
    ray.o[3]=1;
    ray.d[3]=0;
   
    float a = v_dot(ray.d, ray.d);
    float b = 2*v_dot(ray.o,ray.d);
    float c = v_dot(ray.o, ray.o) - radius*radius;
   
    float disc = b*b-4*a*c; 
    
    //printf("b:%f sqrt(disc):%f\n",b,sqrt(disc));
    if(disc<0){
        //printf("\n\n\n");
        return false;
    }
    
    //we have two possible solutions now
    float t1 = (-b-sqrt(disc))/(2*a);
    float t2 = (-b+sqrt(disc))/(2*a);
    //http://www.devmaster.net/wiki/Ray-sphere_intersection
    //TODO: fix this 
       
    if(t1<0 && t2<0){
        hd->t_obj=0;
        hd->external=0;
    } else if(t2<0){
        hd->t_obj=t1;
        hd->external=0;
    } else if(t1<0){
        hd->t_obj=t2;
        hd->external=0;
    } else if(t2<t1){
        hd->t_obj=t2;
        hd->external=1;
    } else if(t1<t2){
        hd->t_obj=t1;
        hd->external=1;
    }
        
    ////printf("object space t value: %f\n",t);
    v_scale(hd->point_obj, ray.d, hd->t_obj);
    v_add(hd->point_obj, hd->point_obj, ray.o);
    hd->point_obj[3] = 1;
    v_copy(hd->normal_obj, hd->point_obj);
    hd->normal_obj[3] = 0;
    v_normalize(hd->normal_obj);
    return true;
}

//silly method to avoid having to add radius in for
//old calls after needing the data for bounding spheres
bool intersectSphere(Ray ray, HitData* hd){
    return intersectSphere(ray, hd, 1);
}
