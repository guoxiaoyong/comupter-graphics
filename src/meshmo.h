#ifndef __MESHMODEL_H__
#define __MESHMODEL_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <data3d.h>

    /* vertex list*/
    VList* VList_Create(int size);
    void  VList_Destroy(VList* vlist);
    int   VList_Add(VList* vlist, Vector3D* v);
    int   VList_AddList(VList* vlist, VList* vadd);
    void  VList_Print(VList* vlist);
    void  VList_BoundingBox(VList* vlist);
    float VList_Range(VList* vlist, Vector3D* Vlow, Vector3D* Vup);
    float VList_RangeX(VList* vlist, Vector3D* Vlow, Vector3D* Vup);
    void  VList_AverageNormal(VList* vlist, FList* flist);

    EList* EList_Create(int size);
    void  EList_Destroy(EList* elist);
    void  EList_Print(EList* elist);
    int   EList_Add(EList* elist, Edge* e);
    int   EList_AddList(EList* elist, EList* eadd);
    int   EList_SetVisible(EList* elist, int n, int flag);
    int   EList_FromFList(EList* elist, FList* flist);

    FList* FList_Create(int size);
    void  FList_Destroy(FList* flist);
    int   FList_Add(FList* flist, Face* f);
    void  FList_Print(FList* flist);
    int   FList_SetVisible(FList* flist, int n, int flag);
    int   FList_FaceNormal(VList* vlist, FList* flist);
    int   FList_NormalizeVN(VList* vlist, FList* flist);
    void  FList_BoundingBox(FList* flist);
    void  FList_TriangleBoundingBox(VList* vlist, FList* flist);
    void  FList_TriangleBaryCoord(VList* vlist, FList* flist);
    void  FList_UpdateBboxshift(FList* flist, Vector3D* origin);
    void  FList_UpdateFaceBboxshift(FList* flist, Vector3D* origin);

    Point2D*  Edge_2DGet(VList* vlist, Edge* e, int n);
    Vector3D* Edge_3DGet(VList* vlist, Edge* e, int n);
    Vector3D* Edge_3DGetX(VList* vlist, Edge* e, int n);
    void      Edge_Sort(Edge* e, int flag);
    int       Edge_Equal(Edge* e1, Edge* e2);
    int       Edge_IsTriangleEdge(Edge* e, Face* f);
    int       Edge_SeeTriangle(VList* vlist, Face* f, Edge* e, Vector3D* origin, EList* elout, VList* vlout);
    int       Edge_SeeTriangleList(VList* vlist, FList* flist, Edge* e, Vector3D* origin, EList* eltmp, EList* elout, VList* vlout);


    Point2D*  Face_2DGet(VList* vlist, Face* f, int n);
    Vector3D* Face_3DGet(VList* vlist, Face* f, int n);
    Vector3D* Face_3DGetX(VList* vlist, Face* f, int n);
    void      Face_GetEdge(Face* f, int n, Edge* e);
    void      Face_3DNormal(VList* vlist, Face* f);
    void      Face_3DTriangleBoundingBox(VList* vlist, Face* f);
    void      Face_3DBaryCoord(VList* vlist, Face* f);
    void      Face_UpdateBboxshift(Face* face, Vector3D* origin);

#ifdef __cplusplus
}
#endif


#endif
