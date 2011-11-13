#include <math.h>
#include <stdio.h>
#include "lab6.h"

void v_print(vector v){
    printf("v[0]: %f v[1]: %f v[2]: %f v[3]: %f\n",v[0],v[1],v[2],v[3]);
}
float v_length(vector v){
    float len = 0;
    for(int i=0;i<3;i++)
        len+=v[i]*v[i];
    return sqrt(len);
}
void v_normalize(vector result, vector v){
    float len = v_length(v);
    for(int i=0;i<3;i++)
        result[i] = v[i]/len;
    result[3]=0;
}
void v_normalize(vector v){
    float len = v_length(v);
    for(int i=0;i<3;i++)
        v[i] = v[i]/len;
}
void v_copy(vector result, vector v){
    for(int i=0;i<4;i++)
        result[i]=v[i];
}
void v_scale(vector result, vector v, float scale){
    for(int i=0;i<3;i++)
        result[i]=v[i]*scale;
    result[3]=0;
}
float v_dot(vector v1, vector v2){
    return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}
void v_cross(vector result, vector v1, vector v2){
    vector temp;
    temp[0] = v1[1]*v2[2] - v1[2]*v2[1];
    temp[1] = v1[2]*v2[0] - v1[0]*v2[2];
    temp[2] = v1[0]*v2[1] - v1[1]*v2[0];
    temp[3] = 0;
    v_copy(result,temp);
}
void v_add(vector result, vector v1, vector v2){
    for(int i=0;i<3;i++)
        result[i]=v1[i]+v2[i];
}
void v_subtract(vector result, vector v1, vector v2){
    for(int i=0;i<3;i++)
        result[i]=v1[i]-v2[i];
}
