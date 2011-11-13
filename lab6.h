#include <string>

//Define types for matrix/vector to make arguments
//more logical and easy to follow
typedef float matrix[4][4];
typedef float vector[4];
typedef int face[3];
typedef float Color[3];

struct Camera {
    vector pos;
    vector coi;
    float angle;
    float aspect;
    float hither;
    float yon;
    float tilt;
    int xres;
    int yres;
};

struct HitData{
    vector normal_obj;  //object space
    vector point_obj;   //object space
    
    vector normal_world;
    vector point_world;
    
    float t_obj;        //distance
    float t_world;
    bool external;  //1 if external hit, 0 if internal

    int index;
};

enum light_type {
    POINT,
    DIRECTIONAL,
    SPOT 
};

enum object_type {
    SPHERE,
    CUBE,
    TRIANGLES
};

struct PointLight {
    vector pos;
    float i; // intensity
    float size;
};
struct DirLight {
    vector dir;
    float i; // intensity
};
struct SpotLight {
    vector pos;
    vector dir;
    float angle; //hood-angle
    int falloff; //falloff-power
    float i; // intensity
    float size;
};

struct Light {
    light_type type;
    void* li;
};

struct Object {
    object_type type;
    struct Triangles* triangles;
    float r;
    float g;
    float b;
    std::string tex;
    float kd;
    float ks;
    int n;
    float kr;
    float gl;
    float kt;
    float n1;
    float tr;

    matrix trans;
    matrix inverse;
};


struct Triangles {
    float radius;
    int vCount, fCount;
    //the vertices of face i are
    //vertex a = vertices[faces[i][0]] 
    //...
    vector *vertices; 
    face *faces;
    vector *fNorms;
    vector *vNorms;
    float *dList;
    bool smooth;
};


struct Scene {
    Camera cam;
    char outName[20];
    int numObjects;
    struct Object *objects[10];
    int numLights;
    struct Light *lights[10];
    float bg_r;
    float bg_g;
    float bg_b;
    float amb;
    int aa;
};

struct Ray {
    vector o; //origin
    vector d; //direction
};

//vector.cpp
extern float v_length(vector v);
extern void v_normalize(vector result, vector v);
extern void v_normalize(vector v);
extern void v_copy(vector result, vector v);
extern float v_dot(vector v1, vector v2);
extern void v_cross(vector result, vector v1, vector v2);
extern void v_add(vector result, vector v1, vector v2);
extern void v_print(vector v);
extern void v_scale(vector result, vector v, float scale);
extern void v_subtract(vector result, vector v1, vector v2);

//matrix.cpp
extern void setIdentity(matrix mat);
extern void mv_mult(vector result, matrix mat, vector v);
extern void mm_mult(matrix result, matrix m1, matrix m2);
extern void m_copy(matrix mdest, matrix m);
extern void m_print(matrix m);
extern void m_transpose(matrix result, matrix m);

//parser.cpp
extern void parse(Scene &scene, char* name);
extern int* readPPM();

//sphere.cpp
extern bool intersectSphere(Ray ray, HitData* hd);
extern bool intersectSphere(Ray ray, HitData* hd, float radius);

//cube.cpp
extern bool intersectCube(Ray ray, HitData* hd);

//triangle.cpp
extern bool intersectTriangles(Ray ray, HitData* hd, Triangles* t);
extern bool intersectTriangle(Ray ray, vector v1, vector v2, vector v3, HitData* hd, float d, vector norm, vector bc);
extern void calculateNorms(Triangles *t);
extern void printFace(Triangles *t, face f);

//lights.cpp
extern void illuminate(vector point, Object* s, vector normal, Scene scene, float initColor[], vector light);

//main.cpp
extern float shadowRay(Ray r);
float randFloat(float low, float high);
float jitter(int x);

