/*
 * Tyler Wymer
 * CSE 681
 * Lab 6
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "lab6.h"


int* tex;
bool DELME=false;
//globals
Scene scene;
float pixelWidth, pixelHeight;
float vpWidth, vpHeight;
float viewPlaneDist = 1;

float PI = 3.14159265;
int RECURSEDEPTH = 3;


void set_cam(){

    //figure out view plane
    //TODO: adjust to not have to have square images
    vpHeight = viewPlaneDist*tan(scene.cam.angle/(57.2957795));
    vpWidth = vpHeight;
    pixelHeight=vpHeight*2/scene.cam.yres;
    pixelWidth=vpWidth*2/scene.cam.xres;
}

float shadowRay(Ray r){
    float scale=1;
    vector bump;
    v_normalize(r.d);
    v_copy(bump,r.d);
    v_scale(bump,bump,.001);
    for(int i=0; i<scene.numObjects; i++){
        HitData* hd = new HitData;
        Ray tRay;
        //bump ray before transformation
        v_add(tRay.o, r.o, bump);
        //conver ray to object space
        //TODO: r/p
        tRay.o[3]=1;
        r.d[3]=0;
        mv_mult(tRay.o, scene.objects[i]->inverse, tRay.o);
        mv_mult(tRay.d, scene.objects[i]->inverse, r.d);
        tRay.o[3]=1;
        tRay.d[3]=0;
        v_normalize(tRay.d);
        bool hit;
        switch (scene.objects[i]->type){
            case SPHERE:
                hit = intersectSphere(tRay,hd);
                break;
            case CUBE:
                hit = intersectCube(tRay,hd);
                break;
            case TRIANGLES:
                hit = intersectTriangles(tRay,hd,scene.objects[i]->triangles);
        }
        if(hit && hd->external==1){
            if(scene.objects[i]->kt==0){
                scale=0;
                break;
            } else {
                scale*=scene.objects[i]->kt;
            }
        }
        delete hd;
    }
    //d1printf("Scale returned: %f\n",scale);
    return scale;
}
/*
   HitData* closestObject(Ray r){
   HitData* hd = new HitData;
   HitData* hdTemp = new HitData;
   hd->t_world = scene.cam.yon;
   vector temp;
   for(int i=0; i<scene.numObjects; i++){
   bool hit=false;
   Ray tRay;
//bump ray before transformation
vector bump;
v_normalize(r.d);
v_copy(bump,r.d);
v_scale(bump,bump,.01);
v_add(tRay.o, r.o, bump);
//conver ray to object space
//TODO: r/p
tRay.o[3]=1;
r.d[3]=0;
mv_mult(tRay.o, scene.objects[i]->inverse, tRay.o);
mv_mult(tRay.d, scene.objects[i]->inverse, r.d);
tRay.o[3]=1;
tRay.d[3]=0;
v_normalize(tRay.d);

switch (scene.objects[i]->type){
case SPHERE:
hit = intersectSphere(tRay,hdTemp);
break;
case CUBE:
hit = intersectCube(tRay,hdTemp);
break;
case TRIANGLES:
hit = intersectTriangles(tRay,hdTemp,scene.objects[i]->triangles);
}
if(hit && hdTemp->external==1){
//calculate world space t
vector tVec;
v_scale(tVec,tRay.d,hdTemp->t_obj);
tVec[3]=0;
mv_mult(tVec, scene.objects[i]->trans, tVec);
//d1v_print(tVec);
hdTemp->t_world=v_length(tVec);
//    hdTemp->t_world=hdTemp->t_obj;
//d1printf("tworld: %f\n",hdTemp->t_world);
if(hdTemp->t_world < hd->t_world && hdTemp->t_world > 0.00){
hdTemp->index=i;
//hd = new HitData;
delete hd;
hd=hdTemp;
} else {
//if the new object is not closer
//and hdTemp != hd
if(hd != hdTemp){
delete hdTemp;
hdTemp = new HitData;
}
}
}

}
if(hd != hdTemp){
delete hdTemp;
}
if(hd->index<0) return hd;
//Now we need to calculate some world space info for this
//intersection


//world point
//world normal
matrix normalMat;
m_transpose(normalMat, scene.objects[hd->index]->inverse);
mv_mult(hd->normal_world, normalMat, hd->normal_obj);
hd->normal_world[3]=0;
v_normalize(hd->normal_world);

v_scale(hd->point_world, r.d, hd->t_world);
v_add(hd->point_world, hd->point_world, r.o);

return hd;
}
*/
HitData* closestObject(Ray r){
    HitData* hd;
    int objectNum = -1;
    hd->t_world = scene.cam.yon;
    vector temp;
    for(int i=0; i<scene.numObjects; i++){
        HitData* hdTemp = new HitData;
        bool hit=false;
        Ray tRay;
        //bump ray before transformation
        vector bump;
        v_normalize(r.d);
        v_copy(bump,r.d);
        v_scale(bump,bump,.01);
        v_add(tRay.o, r.o, bump);
        //conver ray to object space
        //TODO: r/p
        tRay.o[3]=1;
        r.d[3]=0;
        mv_mult(tRay.o, scene.objects[i]->inverse, tRay.o);
        mv_mult(tRay.d, scene.objects[i]->inverse, r.d);
        tRay.o[3]=1;
        tRay.d[3]=0;
        v_normalize(tRay.d);

        switch (scene.objects[i]->type){
            case SPHERE:
                hit = intersectSphere(tRay,hdTemp);
                break;
            case CUBE:
                hit = intersectCube(tRay,hdTemp);
                break;
            case TRIANGLES:
                hit = intersectTriangles(tRay,hdTemp,scene.objects[i]->triangles);
        }
        if(hit && hdTemp->external==1){
            //calculate world space t
            vector tVec;
            v_scale(tVec,tRay.d,hdTemp->t_obj);
            tVec[3]=0;
            mv_mult(tVec, scene.objects[i]->trans, tVec);
            //d1v_print(tVec);
            hdTemp->t_world=v_length(tVec);
            //    hdTemp->t_world=hdTemp->t_obj;
            //d1printf("tworld: %f\n",hdTemp->t_world);
            if(hdTemp->t_world < hd->t_world && hdTemp->t_world > 0.00){
                objectNum=i;
                //     delete hd;
                *hd=*hdTemp;
            }
        }
        delete hdTemp;

    }
    if(objectNum<0){
        hd->index = objectNum;
        return hd;
    }
    //Now we need to calculate some world space info for this
    //intersection


    //world point
    //world normal
    matrix normalMat;
    m_transpose(normalMat, scene.objects[objectNum]->inverse);
    mv_mult(hd->normal_world, normalMat, hd->normal_obj);
    hd->normal_world[3]=0;
    v_normalize(hd->normal_world);

    v_scale(hd->point_world, r.d, hd->t_world);
    v_add(hd->point_world, hd->point_world, r.o);

    hd->index = objectNum;
    return hd;
}

int rings(float r){
    return (int(r))%2;
}
void textureColor(Object* obj, HitData* hd, vector color){
    float xc = hd->point_obj[0];
    float yc = hd->point_obj[1];
    float zc = hd->point_obj[2];
    if(obj->tex=="rings"){
        float r = sqrt(xc*xc+yc*yc);
        int jump =  rings(r/.1);
        if(jump==0){
            color[0]=1;
            color[1]=1;
            color[2]=1;
        } else {
            color[0]=0;
            color[1]=0;
            color[2]=0;
        }
    }
    if(obj->tex=="checker"){
        xc = (xc<0 ? xc*-1+1 : xc);
        yc = (yc<0 ? yc*-1+1 : yc);
        int jump=((int)(xc/.2) + (int)(yc/.2))%2;
        if(jump==0){
            color[0]=.8;
            color[1]=0;
            color[2]=0;
        } else {
            color[0]=.2;
            color[1]=.2;
            color[2]=.2;
        }
    }
    if(obj->tex=="stripe"){
        float test = xc;
        test = (test<0 ? test*-1+1 : test);
        int jump = (int)(test/.2)%2;
        if(jump==0){
            color[0]=1;
            color[1]=1;
            color[2]=1;
        } else {
            color[0]=0;
            color[1]=0;
            color[2]=0;
        }
    }
    if(obj->tex=="image"){
        float theta = acos(hd->point_obj[2]);
        float phi = atan2(hd->point_obj[1],hd->point_obj[0]);
        float x = cos(phi)*sin(theta);
        float uc = phi/2/PI;
        float uv = (PI-theta)/PI;


        int i,j;
        i=(int)(uc*100);
        j=(int)(uv*100);
        color[0]=(float)tex[(i+j*100)*3]/255;
        color[1]=(float)tex[(i+j*100)*3+1]/255;
        color[2]=(float)tex[(i+j*100)*3+2]/255;
    }
}
bool shade(Ray r, int depth, vector color){
    int objectHit = -1;
    vector norm;
    float closest=scene.cam.yon;
    float t;
    //printf("r before\n");
    //v_print(r.d);
    HitData* hd;
    hd = closestObject(r);
    objectHit = hd->index;
    //printf("r after\n");
    //v_print(r.d);
    if(objectHit == -1){
        //no intersections found
        color[0]=scene.bg_r;
        color[1]=scene.bg_g;
        color[2]=scene.bg_b;
        return false;
    } else {
        //d1printf("object hit: %d\n",objectHit);
        float baseColor[3];
        if(scene.objects[objectHit]->tex == "none" || scene.objects[objectHit]->tex == "smooth"){
            baseColor[0]=scene.objects[objectHit]->r;
            baseColor[1]=scene.objects[objectHit]->g;
            baseColor[2]=scene.objects[objectHit]->b;
        } else {
            //printf("tex\n");
            textureColor(scene.objects[objectHit],hd,baseColor);
            //printf("bc: %f %f %f\n", baseColor[0],baseColor[1],baseColor[2]);
        } 

        illuminate(hd->point_world,scene.objects[objectHit], hd->normal_world, scene, baseColor, color);

        if(depth>0){
            //printf("l0 %f l1 %f l2 %f\n",light[0],light[1],light[2]);
            //printf("c0 %f c1 %f c2 %f\n",color[0],color[1],color[2]);
        }
        float kr = scene.objects[objectHit]->kr;
        float kt = scene.objects[objectHit]->kt;
        float gl = scene.objects[objectHit]->gl;

        //reflection!
        if(kr>0 && depth < RECURSEDEPTH){
            //we want to set a ray from this point we are at pointing in the direction
            //of the reflection of the incoming ray.
            //TODO: this ray is also calculated in illuminate...
            vector re, e;
            v_subtract(e, r.o, hd->point_world);
            v_normalize(e);
            v_scale(re, hd->normal_world, v_dot(e,hd->normal_world)*2);
            v_subtract(re,re,e);
            //v_normalize(re);
            vector reflectColor;
            Ray reflectRay;
            v_copy(reflectRay.o,hd->point_world);
            v_copy(reflectRay.d,re);
            shade(reflectRay, depth+1, reflectColor); 
            if(gl > 0){
                int glossRays = 2;
                vector tempColor;
                for(int i=0; i<glossRays; i++){
                    float jx, jy, jz;
                    //jx = randFloat(-.1,.1);
                    //jy = randFloat(-.1,.1);
                    //jz = randFloat(-.1,.1);
                    jx = randFloat(-gl,gl);
                    jy = randFloat(-gl,gl);
                    jz = randFloat(-gl,gl);
                    vector jitter;
                    jitter[0]=jx;
                    jitter[1]=jy;
                    jitter[2]=jz;
                    v_add(jitter,re,jitter);
                    jitter[3]=0;
                    v_copy(reflectRay.d,jitter);
                    shade(reflectRay, depth+1, tempColor);
                    //v_scale(tempColor,tempColor, gl/glossRays);
                    v_scale(tempColor,tempColor, 1.0/glossRays);
                    v_add(reflectColor, tempColor, reflectColor);
                }

            }
            //This check for depth level seems uncomfortable but
            //I was receiving very little new light from reflections
            //so I tried it and it made my images look much like the samples
            //w
            //so I kept it.
            if(depth==0){
                color[0]+=reflectColor[0]*kr;
                color[1]+=reflectColor[1]*kr;
                color[2]+=reflectColor[2]*kr;
            } else { 
                color[0]+=reflectColor[0];
                color[1]+=reflectColor[1];
                color[2]+=reflectColor[2];
            }
            /*
               if(depth==0){
               color[0]+=reflectColor[0]*kr;
               color[1]+=reflectColor[1]*kr;
               color[2]+=reflectColor[2]*kr;
               } else { 
               color[0]+=reflectColor[0];
               color[1]+=reflectColor[1];
               color[2]+=reflectColor[2];
               }*/
            //            printf("color: %f %f %f\n",color[0],color[1],color[2]);
        }
        if(kt>0 && depth < RECURSEDEPTH){
            DELME=true;
            //printf("r: %i\n",sphereHit);
            //if we are hitting outside of sphere
            float n1, n2;
            //TODO: swap test 161-199 with r.d
            vector test;
            v_subtract(test, hd->point_world, r.o);
            v_normalize(test);
            if(v_dot(test, norm) < 0){
                n1=1.0;
                n2=scene.objects[objectHit]->n1;
            } else {    //hitting from inside
                v_scale(norm, norm,-1);
                n1=scene.objects[objectHit]->n1;
                n2=1.0;
                //color[0]=0;
                //color[1]=0;
                //color[2]=0;
            }
            /*
               float cosTheta1 = -v_dot(test,norm);
               float sinTheta1 = sqrt(1-cosTheta1*cosTheta1);
               float sinTheta2 = n1*sinTheta1/n2;
               */
            float cosTheta2 = 1-(n1/n2)*(n1/n2)*(1-(v_dot(test,norm)*v_dot(test,norm)));
            if(cosTheta2>=0){
                cosTheta2=sqrt(cosTheta2);
                vector T;
                vector temp;
                v_scale(T, test, n1/n2);

                v_scale(temp, norm, v_dot(test,norm)*n1/n2);
                v_subtract(T, T, temp);
                v_scale(temp, norm, cosTheta2);
                v_subtract(T,T,temp);
                Ray rT;
                v_normalize(rT.d);
                v_copy(rT.d, T);
                v_copy(rT.o, hd->point_world);
                float refractColor[3]={0,0,0};
                shade(rT, depth+1, refractColor);
                color[0]=(1-kt)*color[0] + refractColor[0]*kt;
                color[1]=(1-kt)*color[1] + refractColor[1]*kt;
                color[2]=(1-kt)*color[2] + refractColor[2]*kt;
            }
        }


        //Cap color
        /*
           if(depth==0){
           for(int i=0;i<3;i++){
           if(color[i]>1)
           color[i]=1;
           }        
           }
           */
        //Fade color.. but only at origin of
        //recursion
        //    printf("color: %f %f %f\n",color[0],color[1],color[2]);
        if(true){  
            double max=-1;
            for(int i=0;i<3;i++){
                if(color[i]>max)
                    max=color[i];
            }

            if(max>1){
                for(int i=0;i<3;i++){
                    color[i]=color[i]/(max);
                }
            }
        }

    }    
    delete hd;
    return true;
}

float randFloat(float low, float high){
    float temp = ( rand()/(static_cast<float>(RAND_MAX)+1.0)) * (high-low)+low;
    return temp;
}
float jitter(int x){
    float temp=randFloat(-1/(float)(2*x),1/(float)(2*x));
    return temp;
}
int main(int argc, char* argv[]){
    tex = readPPM();
    parse(scene,argv[1]); 
    set_cam(); 

    std::ofstream outFile;
    //    outFile.open(scene.outName);
    outFile.open("output.ppm");
    outFile << "P3\n";
    outFile << "# Generated by Tyler Wymer's ray tracer.\n";
    outFile << scene.cam.xres << " " << scene.cam.yres << "\n255\n";

    vector u,v,w;
    v_subtract(w, scene.cam.coi, scene.cam.pos);
    v_normalize(w);
    float theta = scene.cam.tilt*3.14/180;
    //vector up = {cos(theta)-sin(theta),sin(theta)+cos(theta),0,0};
    //vector up={0,1,0,0};
    vector up ={sin(theta),cos(theta),0,0};
    v_cross(u, up, w);
    v_normalize(u);
    v_scale(u,u,-1);
    v_cross(v, u, w);
    v_normalize(v);


    vector scanStart;
    vector pixelCenter;
    //scan start is technicall
    v_copy(scanStart,w);
    vector temp;
    v_scale(temp, u, scene.cam.xres/2*pixelWidth);
    v_subtract(scanStart, scanStart, temp);
    v_scale(temp, v, scene.cam.yres/2*pixelHeight);
    v_add(scanStart, scanStart, temp);


    vector pixWidthScaled;
    v_scale(pixWidthScaled, u, pixelWidth);
    vector pixHeightScaled;
    v_scale(pixHeightScaled, v, pixelHeight);

    //calculate needed fractions of pixels for AA
    vector width4, width8, height4, height8;
    v_scale(width4, pixWidthScaled, .25);
    v_scale(height4, pixHeightScaled, .25);
    v_scale(width8, pixWidthScaled, .125);
    v_scale(height8, pixHeightScaled, .125);

    int numPix = scene.cam.yres*scene.cam.xres;
    int perPix=1;
    if(scene.aa)
        perPix=16;
    //    vector colorRow[scene.cam.xres];
    //		std::string outLine = "";
    //        std::stringstream Stream;
    //		Stream.str(outLine);
    for(int i=0; i<scene.cam.yres; i++){
        v_copy(pixelCenter, scanStart);
        for(int j=0; j<scene.cam.xres; j++){
            float progress = (i*j*perPix+1)/(numPix*perPix+1)*100;
            if((int)progress%10)
                printf("%f\n",progress);
            Ray r;
            v_copy(r.o,scene.cam.pos);

            vector colors[16];

            //use a temp vector for this so we don't
            //trash pixelCenter
            vector aaCenter, finalColor;

            if(scene.aa==1){
                v_copy(r.d,pixelCenter);
                shade(r, 0, finalColor);
            } else {
                finalColor[0]=0;
                finalColor[1]=0;
                finalColor[2]=0;
                //start at top left
                v_subtract(aaCenter, pixelCenter, width4);
                v_subtract(aaCenter, pixelCenter, width8);
                v_add(aaCenter,pixelCenter, height4);
                v_add(aaCenter,pixelCenter, height8);

                for(int i=0; i<4; i++){
                    for(int j=0; j<4; j++){
                        vector jCenter, temp;
                        //TODO: jitter here
                        float sx = jitter(4);
                        float sy = jitter(4);
                        v_scale(temp, pixWidthScaled, sx);
                        v_add(jCenter, aaCenter, temp);
                        v_scale(temp, pixHeightScaled, sy);
                        v_add(jCenter, aaCenter, temp);

                        v_copy(r.d,jCenter);
                        shade(r,0,colors[i*j]);
                        v_add(finalColor,finalColor,colors[i*j]);
                        v_add(aaCenter, aaCenter, width4);
                    }
                    v_subtract(aaCenter, aaCenter, pixWidthScaled);
                    v_add(aaCenter, aaCenter, height4);
                }
                v_scale(finalColor,finalColor,.0625);
            }

            outFile << (int)(finalColor[0]*255);
            outFile << " ";
            outFile << (int)(finalColor[1]*255);
            outFile << " ";
            outFile << (int)(finalColor[2]*255);
            outFile << " \n";

            //            v_copy(colorRow[j],finalColor);

            v_add(pixelCenter, pixWidthScaled, pixelCenter);

        }
        //        for(int k=0; k<scene.cam.xres; k++){
        //            outFile << (int)(colorRow[k][0]*255);
        //           outFile << " ";
        //            outFile << (int)(colorRow[k][1]*255);
        //            outFile << " ";
        //            outFile << (int)(colorRow[k][2]*255);
        //            outFile << " \n";
        //        }
        //        outLine = Stream.str();
        //		outFile << outLine << "\n";

        v_subtract(scanStart,scanStart, pixHeightScaled);

    }

    delete[](tex);
    printf("done!\n");

}
