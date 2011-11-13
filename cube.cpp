#include "lab6.h"
#include <math.h>
#include <limits>

bool intersectCube(Ray r, HitData* hd){
//    float tNear = -std::numeric_limits<float>::infinity();
//    float tFar = std::numeric_limits<float>::infinity();

    float tNear=-1000;
    float tFar=1000;
    
    int face;
    v_normalize(r.d);
    float t1,t2;
    for(int i=0;i<3;i++){
        int side = (r.d[i]>0 ? 1 : -1);
        t1=(side-r.o[i])/r.d[i];
        t2=(-side-r.o[i])/r.d[i];
        if(t1>t2){
            float temp = t2;
            t2=t1;
            t1=temp;
        }
        if(t1>tNear){
            tNear=t1;
            face=i;
        }
        if(t2<tFar){
            tFar=t2;
        }
        if(tNear>=tFar)
            //missed box
//            printf("false\n");
            return false;
        if(tFar<0){
            //box is behind ray
//            printf("false\n");
            return false;
        }
    }
    //tNear now contains object space intersection
    hd->t_obj=tNear;
    //use this for intersect point
    v_scale(hd->point_obj,r.d,tNear);
    v_add(hd->point_obj,hd->point_obj,r.o);
    //TODO: generic
    vector norm;
    if(face==0){
        norm[0]=1;
        norm[1]=0;
        norm[2]=0;
        norm[3]=0;
    } else if(face==1){
        norm[0]=0;
        norm[1]=1;
        norm[2]=0;
        norm[3]=0;
    } else if(face==2){
        norm[0]=0;
        norm[1]=0;
        norm[2]=1;
        norm[3]=0;
    }
    v_copy(hd->normal_obj,norm);
    //TODO: not always outside
    hd->external=1;
//    printf("cube int return true\n");
    return true;
}
