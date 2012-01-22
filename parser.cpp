#include "lab6.h"
#include <fstream>
#include <math.h>

matrix cm,tm,im;
std::ifstream inFile;

int* readPPM(){
    int* values = new int[100*100*3];
    std::ifstream inFile;
    inFile.open("tex.ppm");
    std::string s;
    //skip til good stuff
    getline(inFile,s);
    getline(inFile,s);
    getline(inFile,s);
    getline(inFile,s);
    for(int i=0; i<100*100*3; i++){
        inFile>>values[i];
    }
    return values;
}
void parseObject(Object* obj){
    //color
    inFile>>obj->r;
    inFile>>obj->g;
    inFile>>obj->b;

    //other stuff
    inFile>>obj->tex;
    inFile>>obj->kd;
    inFile>>obj->ks;
    inFile>>obj->n;
    inFile>>obj->kr;
    //gl not used
    inFile>>obj->gl;
    inFile>>obj->kt;
    inFile>>obj->n1;
    //tr not used
    inFile>>obj->tr;

    m_copy(obj->trans, cm);
    m_copy(obj->inverse, im);
}
void parse(Scene &scene, char* name){
    //im is the inverse matrix.. if we incrementally
    //build this, it's trivial.
    setIdentity(tm);
    setIdentity(cm);
    setIdentity(im);

    scene.numObjects = 0;
    scene.numLights = 0;
    scene.aa = 1;

    inFile.open(name);
    std::string s;
    while( inFile >> s) {
        if (s.compare("#")==0){
            getline(inFile, s); //just skip the comment
        } else if (s.compare("aa")==0){
            //TODO: parse rest of line for number
            scene.aa = 4;
        } else if (s.compare("camera")==0){
            inFile >> scene.cam.pos[0] >> scene.cam.pos[1] 
                >> scene.cam.pos[2];
            inFile >> scene.cam.coi[0] >> scene.cam.coi[1] 
                >> scene.cam.coi[2];
            inFile >> scene.cam.angle >> scene.cam.aspect;
            inFile >> scene.cam.hither >> scene.cam.yon;
            inFile >> scene.cam.tilt >> scene.cam.xres >> scene.cam.yres;
        } else if (s.compare("cube")==0) {
            scene.objects[scene.numObjects] = new Object;
            parseObject(scene.objects[scene.numObjects]);

            scene.objects[scene.numObjects]->type=CUBE;
            scene.numObjects++;
        } else if (s.compare("sphere")==0) {
            scene.objects[scene.numObjects] = new Object;
            parseObject(scene.objects[scene.numObjects]);

            scene.objects[scene.numObjects]->type=SPHERE;
            scene.numObjects++;
        } else if (s.compare("triangles")==0) {
            scene.objects[scene.numObjects] = new Object;
            parseObject(scene.objects[scene.numObjects]);

            scene.objects[scene.numObjects]->type=TRIANGLES;
            scene.objects[scene.numObjects]->triangles = new Triangles;
            inFile>>scene.objects[scene.numObjects]->triangles->radius;
            int vCount, fCount;
            inFile>> vCount >> fCount;
            printf("vCount: %d, fCount: %d\n", vCount, fCount);
            scene.objects[scene.numObjects]->triangles->vCount = vCount;
            scene.objects[scene.numObjects]->triangles->fCount = fCount;
            vector* vertices = new vector[vCount];
            face* faces = new face[fCount];

            for(int i=0; i<vCount; i++){
                inFile>>vertices[i][0];
                inFile>>vertices[i][1];
                inFile>>vertices[i][2];
                vertices[i][3]=1;
                //v_print(vertices[i]);
            }
            scene.objects[scene.numObjects]->triangles->vertices = vertices;
            for(int i=0; i<fCount; i++){
                inFile>>faces[i][0];
                inFile>>faces[i][1];
                inFile>>faces[i][2];
                //printFace(scene.objects[scene.numObjects]->triangles, faces[i]);
            }
            scene.objects[scene.numObjects]->triangles->faces = faces;
            calculateNorms(scene.objects[scene.numObjects]->triangles);
            if(scene.objects[scene.numObjects]->tex.compare("smooth")==0)
                scene.objects[scene.numObjects]->triangles->smooth=1;
            else
                scene.objects[scene.numObjects]->triangles->smooth=0;

            scene.numObjects++;
        } else if (s.compare("light")==0){
            PointLight* pl = new PointLight;

            inFile>>pl->pos[0];
            inFile>>pl->pos[1];
            inFile>>pl->pos[2];
            inFile>>pl->i;
            inFile>>pl->size;

            scene.lights[scene.numLights] = new Light;
            scene.lights[scene.numLights]->type = POINT;
            scene.lights[scene.numLights]->li = pl;
            /*
               inFile>>scene.lights[scene.numLights].pos[0];
               inFile>>scene.lights[scene.numLights].pos[1];
               inFile>>scene.lights[scene.numLights].pos[2];
               inFile>>scene.lights[scene.numLights].i;
               inFile>>scene.lights[scene.numLights].f;
               */
            scene.numLights++;
        } else if (s.compare("slight")==0){
            SpotLight* sl = new SpotLight;

            inFile>>sl->pos[0];
            inFile>>sl->pos[1];
            inFile>>sl->pos[2];
            inFile>>sl->dir[0];
            inFile>>sl->dir[1];
            inFile>>sl->dir[2];
            inFile>>sl->angle;
            inFile>>sl->falloff;
            inFile>>sl->i;
            inFile>>sl->size;

            scene.lights[scene.numLights] = new Light;
            scene.lights[scene.numLights]->type = SPOT;
            scene.lights[scene.numLights]->li = sl;
            scene.numLights++;
        } else if (s.compare("dlight")==0){
            DirLight* dl = new DirLight;

            inFile>>dl->dir[0];
            inFile>>dl->dir[1];
            inFile>>dl->dir[2];
            inFile>>dl->i;
            scene.lights[scene.numLights] = new Light;
            scene.lights[scene.numLights]->type = DIRECTIONAL;
            scene.lights[scene.numLights]->li = dl;
            scene.numLights++;
        } else if (s.compare("ambient")==0){
            inFile>>scene.amb;
        } else if (s.compare("backgroundColor")==0 || s.compare("background")==0){
            inFile>>scene.bg_r;
            inFile>>scene.bg_g;
            inFile>>scene.bg_b;
        } else if (s.compare("output")==0 || s.compare("outputFilename")==0){
            inFile>>scene.outName;
        } else if (s.compare("identity")==0){
            setIdentity(cm);
            setIdentity(im);
        } else if (s.compare("rotate")==0){
            setIdentity(tm);
            char axis;
            float deg;
            inFile >> axis >> deg;
            deg=deg/57.2957795;
            switch(axis){
                case 'z':
                    tm[0][0] = cos(deg);
                    tm[0][1] = -sin(deg);
                    tm[1][0] = sin(deg);
                    tm[1][1] = cos(deg);
                    break;
                case 'y':
                    tm[0][0] = cos(deg);
                    tm[0][2] = sin(deg);
                    tm[2][0] = -sin(deg);
                    tm[2][2] = cos(deg);
                    break;
                case 'x':
                    tm[1][1] = cos(deg);
                    tm[1][2] = -sin(deg);
                    tm[2][1] = sin(deg);
                    tm[2][2] = cos(deg);
                    break;
            }
            mm_mult(cm,tm,cm);
            float temp;
            m_transpose(tm,tm);
            mm_mult(im,im,tm);
        } else if (s.compare("translate")==0){
            setIdentity(tm);
            float xval, yval, zval;
            inFile >> xval >> yval >> zval;
            tm[0][3] = xval;
            tm[1][3] = yval;
            tm[2][3] = zval;
            mm_mult(cm,tm,cm);
            tm[0][3]*=-1;
            tm[1][3]*=-1;
            tm[2][3]*=-1;
            mm_mult(im,im,tm);
        } else if (s.compare("scale")==0){
            setIdentity(tm);
            float xval, yval, zval;
            inFile >> xval >> yval >> zval;
            tm[0][0] = xval;
            tm[1][1] = yval;
            tm[2][2] = zval;
            mm_mult(cm,tm,cm);
            tm[0][0] = 1/xval;
            tm[1][1] = 1/yval;
            tm[2][2] = 1/zval;
            mm_mult(im,im,tm);
        } else if (s.compare("transform")==0){
            float xval, yval, zval;
            float trans[4];
            float temp[4] = {0,0,0,0};
            inFile >> trans[0] >> trans[1] >> trans [2];
            trans[3] = 1;
            for(int i=0;i<4;i++)
                for(int j=0;j<4;j++)
                    temp[i] += cm[i][j]*trans[j];
            printf("\n");
            for(int i=0;i<4;i++)
                printf("%.2f ", temp[i]);
            printf("\n");
            //TODO: inverse?
        } else if (s.compare("print")==0){
            for(int i=0; i<4; i++){
                printf("\n");
                for(int j=0; j<4; j++)
                    printf("%.2f  ", cm[i][j]);
            }
            printf("\n");
        }
    }
    if(scene.numObjects<1)
        printf("Error: no objects read from file\n");
}

