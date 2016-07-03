/**
 * @file camera.h
 * @author Guo Xiaoyong
 */



#ifndef __CAMERA_H__
#define __CAMERA_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <data3d.h>


    int AUX_Intersection(float I1min, float I1max, float I2min, float I2max, float* Imin, float* Imax);

    /**
     * @brief cross product
     */
    Vector3D* Vector3D_Create(float x, float y, float z);
    void  Vector3D_Destroy(Vector3D* v);
    void  Vector3D_CrossProduct(Vector3D *x, Vector3D *y, Vector3D *z);
    float Vector3D_InnerProduct(Vector3D *x, Vector3D *y);
    void  Vector3D_Minus(Vector3D* x, Vector3D* y, Vector3D* z);
    void  Vector3D_Add(Vector3D* x, Vector3D* y, Vector3D* z);
    void  Vector3D_Divide(Vector3D* x, Vector3D* y, Vector3D* z);
    float Vector3D_Norm(Vector3D* x);
    float Vector3D_NormSquare(Vector3D *x);
    float Vector3D_Distance(Vector3D* x, Vector3D* y);
    void  Vector3D_Normalize(Vector3D* x);
    float Vector3D_Det2x2(Vector3D* a, Vector3D* b);
    float Vector3D_Det3x3(Vector3D* a, Vector3D* b, Vector3D* c);
    void  Vector3D_Shift(Vector3D* v, int n, char flag);
    void  Vector3D_FaceNormal(Vector3D* x, Vector3D* y, Vector3D* z, Vector3D* FaceNormal);
    int   Vector3D_SameSign(Vector3D* x);
    void  Vector3D_Mul3x3(Vector3D* a1, Vector3D* a2, Vector3D* a3, Vector3D* b, Vector3D* x);
    int   Vector3D_Inverse3x3(Vector3D* a, Vector3D* b, Vector3D* c, Vector3D* x, Vector3D* y, Vector3D* z);
    int   Vector3D_Solve3x3(Vector3D* a1, Vector3D* a2, Vector3D* a3, Vector3D* b, Vector3D* x);
    void  Vector3D_axpy(Vector3D* x, float c, Vector3D* y);
    void  Vector3D_Scale(Vector3D* x, float c);
    void  Vector3D_Print(Vector3D* v);
    void  Vector3D_Set(Vector3D* v, float x, float y, float z);
    void  Vector3D_VSet(Vector3D* v, Vector3D* p);
    void  Vector3D_HouseholdReflection(Vector3D* v, Vector3D* in, Vector3D* out);
    void  Vector3D_Mirror(Vector3D* in, Vector3D* SufaceNormal, Vector3D* out);
    void  Vector3D_Negtive(Vector3D *x);
    void  Vector3D_Center(Vector3D *x, Vector3D* y, Vector3D* m);
    void  Vector3D_Order(Vector3D* bmin, Vector3D* bmax);
    int   Vector3D_HasIntersection(Vector3D* bmin, Vector3D* bmax);
    int   Vector3D_IsPositive(Vector3D* x);
    int   Vector3D_GreaterThanOne(Vector3D* x);


    LightSource* LightSource_Create(Vector3D* p, RGBColor* color);
    void LightSource_Destroy(LightSource* light);
    void LightSource_SetPosition(LightSource* light, Vector3D* p);
    void LightSource_SetColor(LightSource* light, RGBColor* color);

    Matrix3x4* Matrix3x4_Create(float m00, float m10, float m20,
                                float m01, float m11, float m21,
                                float m02, float m12, float m22,
                                float m03, float m13, float m23);

    void Matrix3x4_Destroy(Matrix3x4* m);


    void Rotate_And_Translate          (Matrix3x4* M, Vector3D* x, Vector3D* y);
    void Translate_And_Rotate          (Matrix3x4* M, Vector3D* x, Vector3D* y);
    void Inverse_Translate_And_Rotate  (Matrix3x4* M, Vector3D* x, Vector3D* y);
    void Inverse_Rotate_And_Translate  (Matrix3x4* M, Vector3D* x, Vector3D* y);


    DepthBuffer* DepthBuffer_Create(int width, int height);
    void DepthBuffer_Destroy(DepthBuffer* dbuff);
    void DepthBuffer_Set(DepthBuffer* dbuff, int x, int y, float depth);
    float DepthBuffer_Get(DepthBuffer* dbuff, int x, int y);


    /* camera object constructor */
    Camera* CAM_Create(int Width, int Height, int WPixelsPERUnit, int HPixelsPERUnit);
    void CAM_Destroy(Camera* camera);
    void CAM_SetPosition  (Camera* camera, Vector3D* position);
    void CAM_SetLookAt    (Camera* camera, Vector3D* lookat);
    void CAM_SetLook      (Camera* camera, Vector3D* look);
    void CAM_SetRight     (Camera* camera, Vector3D* right);
    void CAM_SetUp        (Camera* camera, Vector3D* up);
    void CAM_MoveUp       (Camera* camera, float StepLength);
    void CAM_MoveForward  (Camera* camera, float StepLength);
    void CAM_MoveRight    (Camera* camera, float StepLength);
    void CAM_MoveUpFixLookat(Camera* camera, float StepLength);
    void CAM_MoveForwardFixLookat(Camera* camera, float StepLength);
    void CAM_MoveRightFixLookat(Camera* camera, float StepLength);
    void CAM_RotateUp(Camera* camera, float theta);
    void CAM_RotateRight(Camera* camera, float theta);
    void CAM_Project      (Camera* camera, Vector3D* x, Vector3D* y);
    void CAM_CompuWRL2CAM (Camera* camera);
    void CAM_Perspective  (Camera* camera, Vector3D* x, int* xp, int* yp, Vector3D* tmp);
    void CAM_Float2Pixel  (Camera* camera, Vector3D* v, int* x, int* y);
    void CAM_Pixel2Float  (Camera* camera, int x, int y, Vector3D* v, int flag);
    void CAM_Pixel2Ray(Camera* camera, int x, int y, Vector3D* ray);
    int  CAM_ValidateCoord(Camera* camera);
    void CAM_SetAllFromPositionLookat(Camera* camera, Vector3D* position, Vector3D* lookat);
    int  CAM_IsBackFace(Camera* camera, VList* vlist, Face* f);
    int  CAM_SetBackFace(Camera* camera, int flag);
    void CAM_VList_Perspective(Camera* camera, VList* vlist);
    void CAM_Face_Draw(Camera* camera, Face* f, VList* vlist);
    void CAM_Face_DrawColor(Camera* camera, Face* f, VList* vlist, RGBColor* rgb);
    void CAM_Triangle_Fill(Camera* camera, Face* f, VList* vlist);
    void CAM_Triangle_FillColor(Camera* camera, Face* f, RGBColor* color, VList* vlist);
    void CAM_Triangle_FillGradient(Camera* camera, Face* f, RGBColor* c1, RGBColor* c2, RGBColor* c3, VList* vlist);
    void CAM_Edge_Draw(Camera* camera, Edge* e, VList* vlist);
    void CAM_Edge_DrawColor(Camera* camera, Edge* e, VList* vlist, RGBColor* c1, RGBColor* c2);
    void CAM_DrawTriangle(Camera* camera,Point2D* p1,Point2D* p2,Point2D* p3,RGBColor* c1,RGBColor* c2, RGBColor* c3, float d1, float d2, float d3 );
    void CAM_DrawTriangleColor( Camera* camera, Point2D* p1, Point2D* p2, Point2D* p3, RGBColor* c, float d1, float d2, float d3);


#ifdef __cplusplus
}
#endif


#endif



