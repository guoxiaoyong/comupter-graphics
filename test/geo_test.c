#include <geomet.h>

int main()
{
    Vector3D* v0 = Vector3D_Create(9, 9, 2);
    Vector3D* v1 = Vector3D_Create(8, 1, 4);
    Vector3D* v2 = Vector3D_Create(3, 5, 1);
    Vector3D* b  = Vector3D_Create(1, 1, 1);

    float d = Vector3D_Det3x3(v0, v1, v2);
    printf("d=%f\n",d);

    Vector3D x, y, z;
    Vector3D_Inverse3x3(v0, v1, v2, &x, &y, &z);
    Vector3D_Print(&x);
    Vector3D_Print(&y);
    Vector3D_Print(&z);

    Vector3D_Solve3x3(v0, v1, v2, b, &x);
    Vector3D_Print(&x);


#if 0
    int info = GEOMET_IsInTriangle(x, v0, v1, v2);
    printf("info = %d\n", info);

    Vector3D* a = Vector3D_Create(1,1,1);
    Vector3D* b = Vector3D_Create(1,2,3);

    info = LinePlaneCross(a, b, v0, v1, v2, x);
    printf("x = [%f, %f, %f], info = %d\n", VEC3GET(x, 0), VEC3GET(x, 1), VEC3GET(x, 2), info);

    Vector3D_Destroy(v0);
    Vector3D_Destroy(v1);
    Vector3D_Destroy(v2);
    Vector3D_Destroy(x);
    Vector3D_Destroy(a);
    Vector3D_Destroy(b);
#endif

    return 0;
}
