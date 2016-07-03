#ifndef __GEOMET_H__
#define __GEOMET_H__


#include <camera.h>




int   GEOMET_2DLineCross(VList* vlist, Edge* e1, Edge* e2, Point2D* Pint);
int   GEOMET_2DEdgeFaraway(VList* vlist, Edge* e1, Edge* e2);
int   GEOMET_2DEdgeCross(VList* vlist, Edge* e1, Edge* e2, Point2D* Pint);
void  GEOMET_2DLineEquation(VList* vlist, Edge* e, Vector3D* v);
float GEOMET_2DLineEval(Vector3D* v, Point2D* p);
int   GEOMET_2DInTriangle(VList* vlist, int n, Face* f);
int   GEOMET_2DEdgeTriangleCross(VList* vlist, Edge* e, Face* f, PList* plist);


void GEOMET_3DFaceNormal             (VList* vlist, Face* f, Vector3D* FaceNormal);

int  GEOMET_3DEdgeTriangleFaraway    (VList* vlist, Edge* e, Face* f);
int  GEOMET_3DEdgeTriangleCross      (VList* vlist, Edge* e, Face* f, Vector3D* x);
int  GEOMET_3DEdgeBeforeTriangle     (VList* vlist, Edge* e, Face* f, Vector3D* origin);



int GEOMET_RayPlane(VList* vlist, Face* f, Vector3D* origin, Vector3D* direction, Vector3D* t);
int GEOMET_RayTriangle(VList* vlist, Face* f, Vector3D* origin, Vector3D* ray, Vector3D* t);
int GEOMET_RaySphere(Vector3D* RayOrigin, Vector3D* RayDirection, Vector3D* SphereOrigin, float r, float* t);

int GEOMET_RayFListBoundingBox(Vector3D* direction, FList* flist);
int GEOMET_RayFaceBoundingBox(Vector3D* direction, Face* f);


#endif


