#ifndef __DATA3D_H__
#define __DATA3D_H__


#ifdef __cplusplus
extern "C" {
#endif


#include <tcimg.h>

#define MIN2(x, y)  (x) < (y) ? (x) : (y)
#define MIN3(x, y, z)  ( (x) < (y) ? ((x) < (z) ? (x):(z)) : ((y) < (z) ? (y):(z)) )

#define MAX2(x, y)  (x) > (y) ? (x) : (y)
#define MAX3(x, y, z)  ( (x) > (y) ? ((x) > (z) ? (x):(z)) : ((y) > (z) ? (y):(z)) )

#define SWAP(x, y)  (x) += (y); (y) = (x)-(y); (x) -= (y);




#define MAT3GET(A, i, j)  ( A->data[j*3+i] )
#define VEC3GET(v, i)     ( v->data[i]     )

#define MAT3TAK(A, i, j)  ( A.data[j*3+i] )
#define VEC3TAK(v, i)     ( v.data[i]     )


    typedef struct Matrix3x4 {
        float data[12];
    } Matrix3x4;

    typedef struct Vector3D {
        float data[3];
    } Vector3D;

    typedef struct DepthBuffer
    {
        float* data;
        int w;
        int h;
        int len;
    } DepthBuffer;


    typedef struct LightSource
    {
        Vector3D position;
        RGBColor color;
    } LightSource;


    typedef struct Camera
    {
        Vector3D* position;
        Vector3D* lookat;
        Vector3D* look;
        Vector3D* up;
        Vector3D* right;

        Matrix3x4* wrl2cam;

        /* image plane */
        TrueColorImage* image;

        int WPixelsPERUnit;
        int HPixelsPERUnit;

        int BackFaceFlag;
        float near;
        float far;

    } Camera;


#ifndef MAX_VNUM
#define MAX_VNUM 16
#endif

    typedef struct Face
    {
        int v[MAX_VNUM];
        int len;
        Vector3D vn;
        Vector3D bbox[2];
        Vector3D bboxshift[2];
        Vector3D bary[2];

        PList plist1;
        PList plist2;
        PList plist3;

        float el1;
        float el2;
        float el3;

        int visible;
        int e[MAX_VNUM];
    } Face;

    typedef struct Edge
    {
        int v[2];
        int visible;
        PList* plist;
    } Edge;




    /*
     * vertex list
     **/
    typedef struct VList
    {
        Vector3D* v;
        Vector3D* vn;
        RGBColor* vc;

        int len;
        int size;

        Vector3D  bbox[4];
        Vector3D  bboxshift[4];

        /*old*/
        Vector3D* vx;
        Point2D* p;
    } VList;



    typedef struct EList
    {
        VList* vlist;
        Edge* e;
        int len;
        int size;
    } EList;



    typedef struct FList
    {
        VList* vlist;
        Face* f;
        int len;
        int size;

        Vector3D bbox[2];
        Vector3D bboxshift[2];
    } FList;



    typedef struct Object
    {
        VList* vlist;
        EList* elist;
        FList* flist;
    } Object;




#ifdef __cplusplus
}
#endif


#endif


