#include "lab6.h"
#include <stdio.h>


void printFace(Triangles* t, face f){
    printf("face: %d %d %d\n" , f[0], f[1],f[2]);
    for(int i=0; i<3; i++){
        printf("%d: ",i);
        v_print(t->vertices[f[i]]);
    }
    printf("\n");
}
void calculateNorms(Triangles *t){

    //FACE NORMALS
    vector* norms = new vector[t->fCount];
    //printf("face norms\n");
    for(int i=0; i < t->fCount; i++){
        vector temp1;
        vector temp2;
        v_subtract(temp1, t->vertices[t->faces[i][1]], t->vertices[t->faces[i][0]]);
        v_subtract(temp2, t->vertices[t->faces[i][2]], t->vertices[t->faces[i][0]]);

        v_cross(norms[i],temp1,temp2);
        v_normalize(norms[i]);
        v_scale(norms[i],norms[i],-1);
        //v_print(norms[i]);
    }
    t->fNorms=norms;

    //VERTEX NORMALS
    norms = new vector[t->vCount];
    for(int i=0; i < t->vCount; i++){
        norms[i][0]=0;
        norms[i][1]=0;
        norms[i][2]=0;
        norms[i][3]=0;
    }
    //printf("\n");
    for(int j=0; j < t->fCount; j++){
        //printf("\n");
        for(int k=0; k<3; k++){
            //printf("indexing: %d\n",t->faces[j][k]);
            vector test;
            v_scale(test, t->fNorms[j], -1);
            v_add(norms[t->faces[j][k]], norms[t->faces[j][k]], test);
        }
    }
    for(int i=0; i < t->vCount; i++){
        norms[i][3]=0;
        v_scale(norms[i],norms[i],-1);
        v_normalize(norms[i]);
    }
    t->vNorms = norms;

    //D LIST
    float* list = new float[t->fCount];
    for(int i=0; i < t->fCount; i++){
        list[i]=-v_dot(t->vertices[t->faces[i][0]], t->fNorms[i]);
    }
    t->dList = list;
}

bool intersectTriangles(Ray ray, HitData* hd, Triangles* t){
    //test bounding sphere
    if(!intersectSphere(ray, hd, t->radius))
        return false;
    hd->t_obj=9999;
    HitData* hdTemp = new HitData;
    for(int i=0; i < t->fCount; i++){
        //printFace(t, t->faces[i]);
        vector bc;
        if(intersectTriangle(ray, t->vertices[t->faces[i][0]], t->vertices[t->faces[i][1]],t->vertices[t->faces[i][2]],hdTemp,t->dList[i],t->fNorms[i], bc)){
            //TODO: internal?
            hdTemp->external=1;
            if(!t->smooth){
                v_copy(hdTemp->normal_obj,t->fNorms[i]);
            } else { 
                //printFace(t,t->faces[i]);
                //printf("\nPhong vertices.\n");
                vector nv1,nv2,nv3;
                //printf("a,b,c\n");
                //v_print(bc);
                //printf("v3, v1, v2:\n");
                //v_print(t->vNorms[t->faces[i][2]]);
                //v_print(t->vNorms[t->faces[i][0]]);
                //v_print(t->vNorms[t->faces[i][1]]);
                v_scale(nv3, t->vNorms[t->faces[i][2]], bc[0]);
                v_scale(nv1, t->vNorms[t->faces[i][0]], bc[1]);
                v_scale(nv2, t->vNorms[t->faces[i][1]], bc[2]);
                v_add(nv1, nv1, nv2);
                v_add(nv1, nv1, nv3);
                v_scale(nv1, nv1, 1/bc[3]);
                //v_print(bc);
                //dddprintf("\n");
                //dddv_print(nv1);
                v_normalize(nv1);
                nv1[3]=0;
                v_copy(hdTemp->normal_obj,nv1);
                //dddv_print(hdTemp->normal_obj);
            }
            if(hdTemp->t_obj < hd->t_obj && hdTemp->t_obj > 0){
                //TODO:should i record which face it is?
                free(hd);
                *hd=*hdTemp;
            }
        }
    }
    if(hd->t_obj<9000){
        return true;
    } else {
        return false;
    }
}

bool intersectTriangle(Ray ray, vector v1, vector v2, vector v3, HitData* hd, float d, vector norm, vector bc){
    //dddprintf("\n");
    v_normalize(ray.d);
    //POINT OF INTERSECTION
    //parallel?
    if(v_dot(ray.d,norm)>-.1)
        return false;
    //t value calculation from
    //http://www.cs.toronto.edu/~smalik/418/tutorial8_ray_primitive_intersections.pdf
    float t = -(v_dot(ray.o, norm)+d)/(v_dot(ray.d,norm));
    //dddprintf("t value: %f\n", t);
    if(t>0)
        hd->t_obj=t;
    else
        return false;
    vector point;
    v_scale(point, ray.d, t);
    v_add(point, ray.o, point);
    point[3]=1;


    //Calculate Areas
    //
    //temp vectors for areas
    vector temp1, temp2;
    //Full triangle area
    float area;
    v_subtract(temp1, v3, v1);
    v_subtract(temp2, v2, v1);
    v_cross(temp1, temp1, temp2);
    area = .5*v_length(temp1);
    //dddprintf("Triangle area calculations\n");
    //dddprintf("Area: %f\n", area);
    //Sub triangle areas
    float a,b,c;
    //a
    v_subtract(temp1, point, v1);
    v_subtract(temp2, v2, v1);
    v_cross(temp1, temp1, temp2);
    a=.5*v_length(temp1);

    //b
    v_subtract(temp1, point, v2);
    v_subtract(temp2, v3, v2);
    v_cross(temp1, temp1, temp2);
    b=.5*v_length(temp1);

    //c
    v_subtract(temp1, point, v3);
    v_subtract(temp2, v1, v3);
    v_cross(temp1, temp1, temp2);
    c=.5*v_length(temp1);


    //TODO:start
    //http://www.blackpawn.com/texts/pointinpoly/default.html
    vector tv0, tv1, tv2;
    float dot00,dot01,dot02,dot11,dot12;
    v_subtract(tv0,v3,v1);
    v_subtract(tv1,v2,v1);
    v_subtract(tv2,point,v1);
    
    dot00 = v_dot(tv0, tv0);
    dot01 = v_dot(tv0, tv1);
    dot02 = v_dot(tv0, tv2);
    dot11 = v_dot(tv1, tv1);
    dot12 = v_dot(tv1, tv2);
    float invDenom= 1/(dot00*dot11-dot01*dot01);
    bc[1]=(dot11 * dot02 - dot01 * dot12) * invDenom;
    bc[2]=(dot00 * dot12 - dot01 * dot02) * invDenom;
    //stop
        
    
    //dddprintf("barycentric test:\n");
    //dddprintf("alpha: %f, beta: %f, gamma: %f\n",bc[0],bc[1],bc[2]);
    
    if( (bc[2]+bc[1]<1) && bc[2]>0 && bc[1]>0){
        bc[0]=a;
        bc[1]=b;
        bc[2]=c;
        bc[3]=a+b+c;
        //TODO: return useful info
        //dddprintf("Hit!\n");
        v_copy(hd->point_obj,point);
        //dddprintf("Assigning intersect point:\n");
        //dddv_print(point);
        return true;
    } else {
        //dddprintf("Miss!\n");
        return false;
    }
    //TODO: interpolate vertex normals
}
    
