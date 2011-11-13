#include "lab6.h"
#include <math.h>


//TODO: don't pass scene....
void illuminate(vector point, Object* s, vector normal, Scene scene, float initColor[], vector color){
//    printf("ic: %f %f %f\n", initColor[0],initColor[1],initColor[2]);
    //construct ray from the point in space
    //where this collision occured
    float diffuse=0; 
    float specular=0;
    for(int i=0; i<scene.numLights; i++){
        //TODO: divide by type of light
        switch(scene.lights[i]->type){
            case DIRECTIONAL:
                {
                    DirLight* dl = static_cast<DirLight*>(scene.lights[i]->li);
                    vector dir;
                    v_copy(dir, dl->dir);
                    v_normalize(dir);
                    v_scale(dir, dir, -1);
                    
                    vector re, e;
                    v_subtract(e, scene.cam.pos, point);
                    v_normalize(e);
                    v_scale(re, normal, v_dot(e, normal)*2);
                    v_subtract(re, re, e);
                    v_normalize(re);
                    float dotprod = v_dot(dir, normal);
                    if(dotprod>0){
                        Ray rayToLight;
                        v_copy(rayToLight.o, point);
                        v_copy(rayToLight.d, dir);
                        
                        float scale = shadowRay(rayToLight);
                        diffuse+=dotprod*dl->i;
                        float x=-1;
                        if(s->n!=0){
                            float dotprodSpec = v_dot(re,dir);
                            if(dotprodSpec<0)
                                dotprodSpec=0;
                            x = pow(dotprodSpec, s->n)*dl->i;
                        }
                        if(x>0)
                            specular+=x;
                        diffuse*=scale;
                        specular*=scale;
                    }

                }
                break;
            case SPOT:
                {
                    SpotLight* sl = static_cast<SpotLight*>(scene.lights[i]->li);
                    vector lightPoint;
                    v_copy(lightPoint, sl->pos);
                    vector lightVector;
                    vector lightToPoint;
                    v_subtract(lightVector,lightPoint,point);
                    v_normalize(lightVector);
                    
                    vector re, e;
                    v_subtract(e, scene.cam.pos, point);
                    v_normalize(e);
                    v_scale(re, normal, v_dot(e, normal)*2);
                    v_subtract(re, re, e);
                    v_normalize(re);
                    float dotprod = v_dot(lightVector,normal);
                    //if positive, we are facing the light
                    if(dotprod>0){
                        Ray rayToLight;
                        v_copy(rayToLight.o, point);
                        v_copy(rayToLight.d, lightVector);
                        
                        float scale = shadowRay(rayToLight);
                        vector D;
                        vector dist;
                        v_subtract(dist,lightPoint,point);
                        v_copy(D,sl->dir);
                        v_scale(D,D,-1);
                        float intensity = pow(v_dot(D,lightVector), sl->falloff);
                        if(intensity<0)
                            intensity=0;
                        diffuse+=dotprod*intensity*sl->i;
                        float x=-1;
                        if(s->n!=0){
                            float dotprodSpec = v_dot(re,lightVector);
                            if(dotprodSpec<0)
                                dotprodSpec=0;
                            x = pow(dotprodSpec, s->n)*intensity*sl->i;
                        }
                        if(x>0){
                            specular+=x;
                        }
                        diffuse*=scale;
                        specular*=scale;
                    }
                }
                break;
            case POINT:
                {
                    PointLight* pl = static_cast<PointLight*>(scene.lights[i]->li);
                    vector lightPoint;
                    v_copy(lightPoint, pl->pos);
                    vector lightVector;
                    
                    v_subtract(lightVector,lightPoint,point);
                    v_normalize(lightVector);
                    lightVector[3]=0;
                    point[3]=1;
                    
                    vector re, e;
                    v_subtract(e, scene.cam.pos, point);
                    v_normalize(e);
                    v_scale(re, normal, v_dot(e, normal)*2);
                    v_subtract(re, re, e);
                    v_normalize(re);
                    float dotprod = v_dot(lightVector,normal);
                    //if positive, we are facing the light
                    if(dotprod>0){
                        Ray rayToLight;
                        v_copy(rayToLight.o, point);
                        if(pl->size==0){
                            v_copy(rayToLight.d, lightVector);
                        } else {
                            vector lightTemp;
                            v_subtract(lightTemp,lightPoint,point);
//                            printf("Pre Jitter:\n");
//                            v_print(lightTemp);
//                            printf("size: %f\n",pl->size);
                            float sx=randFloat(-pl->size,pl->size);
                            float sy=randFloat(-pl->size,pl->size);
                            float sz=randFloat(-pl->size,pl->size);
                            vector jitter;
                            jitter[0]=sx;
                            jitter[1]=sy;
                            jitter[2]=sz;
//                            v_print(jitter);
                            v_add(lightTemp, lightTemp, jitter);
//                            printf("Post Jitter:\n");
//                            v_print(lightTemp);
                            v_normalize(lightTemp);
                            lightTemp[3]=0;
                            v_copy(rayToLight.d, lightTemp);
                        }
                        float scale = shadowRay(rayToLight);
                        diffuse+=dotprod*pl->i;
                        float x=-1;
                        if(s->n!=0){
                            float dotprodSpec = v_dot(re,lightVector);
                            if(dotprodSpec<0)
                                dotprodSpec=0;
                            x = pow(dotprodSpec, s->n)*pl->i;
                        }
                        if(x>0){
                            specular+=x;
                        }
                        diffuse*=scale;
                        specular*=scale;
                    }
                }
                break;
        }
    }
        
    color[0]=s->kd*(scene.amb + diffuse)*initColor[0];
    color[0]+=s->ks*specular;
    color[1]=s->kd*(scene.amb + diffuse)*initColor[1];
    color[1]+=s->ks*specular;
    color[2]=s->kd*(scene.amb + diffuse)*initColor[2];
    color[2]+=s->ks*specular;
    //printf("c %f\n",color[0]);
    
} 
