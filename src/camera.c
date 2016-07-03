/**
 * @file camera.c
 * @author Guo Xiaoyong
 * @date 04/11/2011
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <limits.h>

#include <camera.h>
#include <geomet.h>
#include <meshmo.h>


    Vector3D* Vector3D_Create(float x, float y, float z)
    {
        Vector3D* v;

        v = (Vector3D*)malloc( sizeof(Vector3D) );
        VEC3GET(v, 0) = x;
        VEC3GET(v, 1) = y;
        VEC3GET(v, 2) = z;

        return v;
    }


    void Vector3D_Destroy(Vector3D* v)
    {
        free(v);
    }

    int Vector3D_IsEqual(Vector3D* v1, Vector3D* v2)
    {
        float x, y, z;
        x = VEC3GET(v1, 0) - VEC3GET(v2, 0);
        y = VEC3GET(v1, 1) - VEC3GET(v2, 1);
        z = VEC3GET(v1, 2) - VEC3GET(v2, 2);

        if ( x*x+y*y+z*z < 1e-6 ) return 1;
        else return 0;
    }


    void Vector3D_CrossProduct(Vector3D *x, Vector3D *y, Vector3D *z)
    {
        VEC3GET(z, 0) = (VEC3GET(x, 1) * VEC3GET(y, 2) - VEC3GET(y, 1) * VEC3GET(x, 2));
        VEC3GET(z, 1) = (VEC3GET(x, 2) * VEC3GET(y, 0) - VEC3GET(y, 2) * VEC3GET(x, 0));
        VEC3GET(z, 2) = (VEC3GET(x, 0) * VEC3GET(y, 1) - VEC3GET(y, 0) * VEC3GET(x, 1));
    }

    float Vector3D_InnerProduct(Vector3D *x, Vector3D *y)
    {
        return VEC3GET(x, 0)*VEC3GET(y, 0) +
               VEC3GET(x, 1)*VEC3GET(y, 1) +
               VEC3GET(x, 2)*VEC3GET(y, 2);
    }

    void  Vector3D_Minus(Vector3D* x, Vector3D* y, Vector3D* z)
    {
        VEC3GET(z, 0) = VEC3GET(x, 0) - VEC3GET(y, 0);
        VEC3GET(z, 1) = VEC3GET(x, 1) - VEC3GET(y, 1);
        VEC3GET(z, 2) = VEC3GET(x, 2) - VEC3GET(y, 2);
    }

    void  Vector3D_Add(Vector3D* x, Vector3D* y, Vector3D* z)
    {
        VEC3GET(z, 0) = VEC3GET(x, 0) + VEC3GET(y, 0);
        VEC3GET(z, 1) = VEC3GET(x, 1) + VEC3GET(y, 1);
        VEC3GET(z, 2) = VEC3GET(x, 2) + VEC3GET(y, 2);
    }


    void  Vector3D_Divide(Vector3D* x, Vector3D* y, Vector3D* z)
    {
        if ( fabs( VEC3GET(y, 0) ) > 1e-6 )
        {
            VEC3GET(z, 0) = VEC3GET(x, 0) / VEC3GET(y, 0);
        }
        else
        {
            if ( VEC3GET(x, 0) >= 0 )
            {
                VEC3GET(z, 0) = +1e10;
            }
            else
            {
                VEC3GET(z, 0) = -1e10;
            }
        }


        if ( fabs( VEC3GET(y, 1) ) > 1e-4 )
        {
            VEC3GET(z, 1) = VEC3GET(x, 1) / VEC3GET(y, 1);
        }
        else
        {
            if ( VEC3GET(x, 1) >= 0 )
            {
                VEC3GET(z, 1) = +1e10;
            }
            else
            {
                VEC3GET(z, 1) = -1e10;
            }
        }


        if ( fabs( VEC3GET(y, 2) ) > 1e-4 )
        {
            VEC3GET(z, 2) = VEC3GET(x, 2) / VEC3GET(y, 2);
        }
        else
        {
            if ( VEC3GET(x, 2) >= 0 )
            {
                VEC3GET(z, 2) = +1e10;
            }
            else
            {
                VEC3GET(z, 2) = -1e10;
            }
        }
    }

    void Vector3D_HouseholdReflection(Vector3D* v, Vector3D* in, Vector3D* out)
    {
        assert( fabs(Vector3D_Norm(v)-1.0f) < 1e-5f );

        float s = 2.0f * Vector3D_InnerProduct(in, v);

        VEC3GET(out, 0) = VEC3GET(in, 0) - s*VEC3GET(v, 0);
        VEC3GET(out, 1) = VEC3GET(in, 1) - s*VEC3GET(v, 1);
        VEC3GET(out, 2) = VEC3GET(in, 2) - s*VEC3GET(v, 2);
    }

    void Vector3D_Mirror(Vector3D* in, Vector3D* SurfaceNormal, Vector3D* out)
    {
        Vector3D z, x;

        Vector3D_CrossProduct(in, SurfaceNormal, &z);
        Vector3D_CrossProduct(&z, SurfaceNormal, &x);
        Vector3D_Normalize(&x);
        Vector3D_HouseholdReflection(&x, in, out);
    }


    void Vector3D_axpy(Vector3D* x, float c, Vector3D* y)
    {
        VEC3GET(y, 0) += VEC3GET(x, 0)*c;
        VEC3GET(y, 1) += VEC3GET(x, 1)*c;
        VEC3GET(y, 2) += VEC3GET(x, 2)*c;
    }

    void Vector3D_Scale(Vector3D* x, float c)
    {
        VEC3GET(x, 0) *= c;
        VEC3GET(x, 1) *= c;
        VEC3GET(x, 2) *= c;
    }

    float Vector3D_Norm(Vector3D *x)
    {
        return (float)sqrt( VEC3GET(x, 0) * VEC3GET(x, 0)
                            + VEC3GET(x, 1) * VEC3GET(x, 1)
                            + VEC3GET(x, 2) * VEC3GET(x, 2) );
    }


    float Vector3D_NormSquare(Vector3D *x)
    {
        return (VEC3GET(x, 0) * VEC3GET(x, 0)
                + VEC3GET(x, 1) * VEC3GET(x, 1)
                + VEC3GET(x, 2) * VEC3GET(x, 2) );
    }

    void Vector3D_Normalize(Vector3D* x)
    {
        float norm2 = Vector3D_Norm(x);
        if (norm2 != 0.0f)
        {
            VEC3GET(x, 0) /= norm2;
            VEC3GET(x, 1) /= norm2;
            VEC3GET(x, 2) /= norm2;
        }
    }


    void Vector3D_Negtive(Vector3D *x)
    {
        VEC3GET(x, 0) = -VEC3GET(x, 0);
        VEC3GET(x, 1) = -VEC3GET(x, 1);
        VEC3GET(x, 2) = -VEC3GET(x, 2);
    }


    void Vector3D_Center(Vector3D *x, Vector3D* y, Vector3D* m)
    {
        VEC3GET(m, 0) = (VEC3GET(x, 0) + VEC3GET(y, 0))/2.0f;
        VEC3GET(m, 1) = (VEC3GET(x, 1) + VEC3GET(y, 1))/2.0f;
        VEC3GET(m, 2) = (VEC3GET(x, 2) + VEC3GET(y, 2))/2.0f;
    }




    float Vector3D_Det2x2(Vector3D* a, Vector3D* b)
    {
        return VEC3GET(a, 0)*VEC3GET(b, 1) - VEC3GET(a, 1)*VEC3GET(b, 0);
    }


    float Vector3D_Det3x3(Vector3D* a, Vector3D* b, Vector3D* c)
    {
        return +VEC3GET(a,0)*VEC3GET(b,1)*VEC3GET(c,2)
               -VEC3GET(a,0)*VEC3GET(b,2)*VEC3GET(c,1)
               -VEC3GET(a,1)*VEC3GET(b,0)*VEC3GET(c,2)
               +VEC3GET(a,1)*VEC3GET(b,2)*VEC3GET(c,0)
               +VEC3GET(a,2)*VEC3GET(b,0)*VEC3GET(c,1)
               -VEC3GET(a,2)*VEC3GET(b,1)*VEC3GET(c,0);
    }

    int Vector3D_Inverse3x3(Vector3D* a, Vector3D* b, Vector3D* c, Vector3D* x, Vector3D* y, Vector3D* z)
    {
        float d = Vector3D_Det3x3(a, b, c);

        if (d < 1e-4) return 0;

        VEC3GET(x, 0) = VEC3GET(b, 1)*VEC3GET(c, 2) - VEC3GET(b, 2)*VEC3GET(c, 1);
        VEC3GET(x, 1) = VEC3GET(a, 2)*VEC3GET(c, 1) - VEC3GET(a, 1)*VEC3GET(c, 2);
        VEC3GET(x, 2) = VEC3GET(a, 1)*VEC3GET(b, 2) - VEC3GET(a, 2)*VEC3GET(b, 1);

        VEC3GET(y, 0) = VEC3GET(b, 2)*VEC3GET(c, 0) - VEC3GET(b, 0)*VEC3GET(c, 2);
        VEC3GET(y, 1) = VEC3GET(a, 0)*VEC3GET(c, 2) - VEC3GET(a, 2)*VEC3GET(c, 0);
        VEC3GET(y, 2) = VEC3GET(a, 2)*VEC3GET(b, 0) - VEC3GET(a, 0)*VEC3GET(b, 2);

        VEC3GET(z, 0) = VEC3GET(b, 0)*VEC3GET(c, 1) - VEC3GET(b, 1)*VEC3GET(c, 0);
        VEC3GET(z, 1) = VEC3GET(a, 1)*VEC3GET(c, 0) - VEC3GET(a, 0)*VEC3GET(c, 1);
        VEC3GET(z, 2) = VEC3GET(a, 0)*VEC3GET(b, 1) - VEC3GET(a, 1)*VEC3GET(b, 0);

        VEC3GET(x, 0) /= d;
        VEC3GET(x, 1) /= d;
        VEC3GET(x, 2) /= d;

        VEC3GET(y, 0) /= d;
        VEC3GET(y, 1) /= d;
        VEC3GET(y, 2) /= d;

        VEC3GET(z, 0) /= d;
        VEC3GET(z, 1) /= d;
        VEC3GET(z, 2) /= d;

        return 1;
    }

    void Vector3D_Mul3x3(Vector3D* a1, Vector3D* a2, Vector3D* a3, Vector3D* b, Vector3D* x)
    {
        VEC3GET(x, 0) = VEC3GET(b, 0)*VEC3GET(a1, 0)+VEC3GET(b, 1)*VEC3GET(a2, 0) + VEC3GET(b, 2)*VEC3GET(a3, 0);
        VEC3GET(x, 1) = VEC3GET(b, 0)*VEC3GET(a1, 1)+VEC3GET(b, 1)*VEC3GET(a2, 1) + VEC3GET(b, 2)*VEC3GET(a3, 1);
        VEC3GET(x, 2) = VEC3GET(b, 0)*VEC3GET(a1, 2)+VEC3GET(b, 1)*VEC3GET(a2, 2) + VEC3GET(b, 2)*VEC3GET(a3, 2);
    }

    int Vector3D_Solve3x3(Vector3D* a1, Vector3D* a2, Vector3D* a3, Vector3D* b, Vector3D* x)
    {
        /*
        int info;
        Vector3D p1, p2, p3;
        info = Vector3D_Inverse3x3(a1, a2, a3, &p1, &p2, &p3);

        if (info == 0) return 0;

        Vector3D_Mul3x3(&p1, &p2, &p3, b, x);

        return 1;
        */

        /* cramer's rule */
        float det, x1, x2, x3;
        det = Vector3D_Det3x3(a1, a2, a3);

        /*FIXME*/
        if ( det == 0.0 ) return 0;

        x1  = Vector3D_Det3x3(b,  a2, a3);
        x2  = Vector3D_Det3x3(a1, b,  a3);
        x3  = Vector3D_Det3x3(a1, a2, b);

        Vector3D_Set(x, x1/det, x2/det, x3/det);
        return 1;
    }

    void Vector3D_Set(Vector3D* v, float x, float y, float z)
    {
        VEC3GET(v, 0) = x;
        VEC3GET(v, 1) = y;
        VEC3GET(v, 2) = z;
    }

    void Vector3D_Order(Vector3D* bmin, Vector3D* bmax)
    {
        if ( VEC3GET(bmin, 0) > VEC3GET(bmax, 0) )
        {
            SWAP(VEC3GET(bmin, 0), VEC3GET(bmax, 0));
        }

        if ( VEC3GET(bmin, 1) > VEC3GET(bmax, 1) )
        {
            SWAP(VEC3GET(bmin, 1), VEC3GET(bmax, 1));
        }

        if ( VEC3GET(bmin, 2) > VEC3GET(bmax, 2) )
        {
            SWAP(VEC3GET(bmin, 2), VEC3GET(bmax, 2));
        }
    }



    int AUX_Intersection(float I1min, float I1max, float I2min, float I2max, float* Imin, float* Imax)
    {
        if ( (I1min > I2max) || (I1max < I2min) )
        {
            return 0;
        }
        else
        {
            *Imin = MAX2(I1min, I2min);
            *Imax = MIN2(I1max, I2max);
            return 1;
        }
    }

    int Vector3D_HasIntersection(Vector3D* bmin, Vector3D* bmax)
    {
        float x, y;
        int info;

        info = AUX_Intersection(VEC3GET(bmin, 0), VEC3GET(bmax, 0), VEC3GET(bmin, 1), VEC3GET(bmax, 1), &x, &y);
        if (0 == info)
        {
            return 0;
        }
        else
        {
            return AUX_Intersection(VEC3GET(bmin, 2), VEC3GET(bmax, 2), x, y, &x, &y);
        }
    }

    int Vector3D_IsPositive(Vector3D* x)
    {
        if ( (VEC3GET(x, 0) > 0.0f) && (VEC3GET(x, 1) > 0.0f) && (VEC3GET(x, 2) > 0.0f) )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    int Vector3D_GreaterThanOne(Vector3D* x)
    {
        if ( (VEC3GET(x, 0) > 1.0f) && (VEC3GET(x, 1) > 1.0f) && (VEC3GET(x, 2) > 1.0f) )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }



    float Vector3D_Distance(Vector3D *x, Vector3D* y)
    {
        Vector3D z;
        Vector3D_Minus(x, y, &z);

        return Vector3D_Norm(&z);
    }

    void Vector3D_Shift(Vector3D* v, int n, char flag)
    {
        float a, b;

        n %= 3;
        if (flag == 'L')
        {
            if (n == 1)
            {
                a = VEC3GET(v, 0);
                VEC3GET(v, 0) = VEC3GET(v, 1);
                VEC3GET(v, 1) = VEC3GET(v, 2);
                VEC3GET(v, 2) = a;
            }
            else if (n == 2)
            {
                a = VEC3GET(v, 0);
                b = VEC3GET(v, 1);
                VEC3GET(v, 0) = VEC3GET(v, 2);
                VEC3GET(v, 1) = a;
                VEC3GET(v, 2) = b;
            }
        }
        else // flag == 'R'
        {
            if (n == 1)
            {
                a = VEC3GET(v, 2);
                VEC3GET(v, 2) = VEC3GET(v, 1);
                VEC3GET(v, 1) = VEC3GET(v, 0);
                VEC3GET(v, 0) = a;
            }
            else if (n == 2)
            {
                a = VEC3GET(v, 2);
                b = VEC3GET(v, 1);
                VEC3GET(v, 2) = VEC3GET(v, 0);
                VEC3GET(v, 1) = a;
                VEC3GET(v, 0) = b;
            }
        }
    }

    void Vector3D_FaceNormal(Vector3D* x, Vector3D* y, Vector3D* z, Vector3D* FaceNormal)
    {
        Vector3D a;
        Vector3D b;
        Vector3D_Minus(y, x, &a);
        Vector3D_Minus(z, y, &b);
        Vector3D_CrossProduct(&a, &b, FaceNormal);
    }

    int Vector3D_SameSign(Vector3D* x)
    {
        int a, b, c;
        a = VEC3GET(x, 0) >= 0.0 ? 1:-1;
        b = VEC3GET(x, 1) >= 0.0 ? 1:-1;
        c = VEC3GET(x, 2) >= 0.0 ? 1:-1;

        if (a==b && b==c) return 1;
        else return -1;
    }

    void Vector3D_VSet(Vector3D* v, Vector3D* p)
    {
        VEC3GET(v, 0) = VEC3GET(p, 0);
        VEC3GET(v, 1) = VEC3GET(p, 1);
        VEC3GET(v, 2) = VEC3GET(p, 2);
    }

    void Vector3D_Print(Vector3D* v)
    {
        printf("[%f, %f, %f]\n", VEC3GET(v, 0), VEC3GET(v, 1), VEC3GET(v, 2));
    }


    /**
     * LightSource
     **/
    LightSource* LightSource_Create(Vector3D* p, RGBColor* color)
    {
        LightSource* light;
        light = (LightSource*)malloc( sizeof(LightSource) );

        if (p != NULL)
        {
            Vector3D_VSet( &(light->position), p);
        }

        if (color != NULL)
        {
            RGBColor_VSet( &(light->color), color);
        }

        return light;
    }

    void LightSource_Destroy(LightSource* light)
    {
        free(light);
    }


    void LightSource_SetPosition(LightSource* light, Vector3D* p)
    {
        Vector3D_VSet( &(light->position), p);
    }

    void LightSource_SetColor(LightSource* light, RGBColor* color)
    {
        RGBColor_VSet( &(light->color), color);
    }



    void Rotate_And_Translate(Matrix3x4* M, Vector3D* x, Vector3D* y)
    {
        VEC3GET(y, 0) = MAT3GET(M, 0, 0) * VEC3GET(x, 0)
                        + MAT3GET(M, 0, 1) * VEC3GET(x, 1)
                        + MAT3GET(M, 0, 2) * VEC3GET(x, 2) + MAT3GET(M, 0, 3);

        VEC3GET(y, 1) = MAT3GET(M, 1, 0) * VEC3GET(x, 0)
                        + MAT3GET(M, 1, 1) * VEC3GET(x, 1)
                        + MAT3GET(M, 1, 2) * VEC3GET(x, 2) + MAT3GET(M, 1, 3);

        VEC3GET(y, 2) = MAT3GET(M, 2, 0) * VEC3GET(x, 0)
                        + MAT3GET(M, 2, 1) * VEC3GET(x, 1)
                        + MAT3GET(M, 2, 2) * VEC3GET(x, 2) + MAT3GET(M, 2, 3);
    }



    void Translate_And_Rotate(Matrix3x4* M, Vector3D* x, Vector3D* y)
    {
        static Vector3D xtmp;
        VEC3GET((&xtmp), 0) = VEC3GET(x, 0) + MAT3GET(M, 0, 3);
        VEC3GET((&xtmp), 1) = VEC3GET(x, 1) + MAT3GET(M, 1, 3);
        VEC3GET((&xtmp), 2) = VEC3GET(x, 2) + MAT3GET(M, 2, 3);

        VEC3GET(y, 0) = MAT3GET(M, 0, 0) * VEC3GET((&xtmp), 0)
                        + MAT3GET(M, 0, 1) * VEC3GET((&xtmp), 1)
                        + MAT3GET(M, 0, 2) * VEC3GET((&xtmp), 2);

        VEC3GET(y, 1) = MAT3GET(M, 1, 0) * VEC3GET((&xtmp), 0)
                        + MAT3GET(M, 1, 1) * VEC3GET((&xtmp), 1)
                        + MAT3GET(M, 1, 2) * VEC3GET((&xtmp), 2);

        VEC3GET(y, 2) = MAT3GET(M, 2, 0) * VEC3GET((&xtmp), 0)
                        + MAT3GET(M, 2, 1) * VEC3GET((&xtmp), 1)
                        + MAT3GET(M, 2, 2) * VEC3GET((&xtmp), 2);
    }


    void Inverse_Rotate_And_Translate(Matrix3x4* M, Vector3D* x, Vector3D* y)
    {
        static Vector3D xtmp;
        VEC3GET((&xtmp), 0) = VEC3GET(x, 0) - MAT3GET(M, 0, 3);
        VEC3GET((&xtmp), 1) = VEC3GET(x, 1) - MAT3GET(M, 1, 3);
        VEC3GET((&xtmp), 2) = VEC3GET(x, 2) - MAT3GET(M, 2, 3);

        VEC3GET(y, 0) = + MAT3GET(M, 0, 0) * VEC3GET((&xtmp), 0)
                        + MAT3GET(M, 1, 0) * VEC3GET((&xtmp), 1)
                        + MAT3GET(M, 2, 0) * VEC3GET((&xtmp), 2);

        VEC3GET(y, 1) = + MAT3GET(M, 0, 1) * VEC3GET((&xtmp), 0)
                        + MAT3GET(M, 1, 1) * VEC3GET((&xtmp), 1)
                        + MAT3GET(M, 2, 1) * VEC3GET((&xtmp), 2);

        VEC3GET(y, 2) = + MAT3GET(M, 0, 2) * VEC3GET((&xtmp), 0)
                        + MAT3GET(M, 1, 2) * VEC3GET((&xtmp), 1)
                        + MAT3GET(M, 2, 2) * VEC3GET((&xtmp), 2);
    }


    void Inverse_Translate_And_Rotate(Matrix3x4* M, Vector3D* x, Vector3D* y)
    {
        VEC3GET(y, 0) = + MAT3GET(M, 0, 0) * VEC3GET(x, 0)
                        + MAT3GET(M, 1, 0) * VEC3GET(x, 1)
                        + MAT3GET(M, 2, 0) * VEC3GET(x, 2) - MAT3GET(M, 0, 3);

        VEC3GET(y, 1) = + MAT3GET(M, 0, 1) * VEC3GET(x, 0)
                        + MAT3GET(M, 1, 1) * VEC3GET(x, 1)
                        + MAT3GET(M, 2, 1) * VEC3GET(x, 2) - MAT3GET(M, 1, 3);

        VEC3GET(y, 2) = + MAT3GET(M, 0, 2) * VEC3GET(x, 0)
                        + MAT3GET(M, 1, 2) * VEC3GET(x, 1)
                        + MAT3GET(M, 2, 2) * VEC3GET(x, 2) - MAT3GET(M, 2, 3);
    }



#if 0
    DepthBuffer* DepthBuffer_Create(int width, int height)
    {
        int i;
        DepthBuffer* dbuff = (DepthBuffer*)malloc( sizeof(DepthBuffer) );
        dbuff->w = width;
        dbuff->h = height;
        dbuff->len = width*height;
        dbuff->data = (float*)malloc(sizeof(float) * dbuff->len);

        for(i = 0; i < dbuff->len; ++i)
        {
            dbuff->data[i] = FLT_MAX;
        }

        return dbuff;
    }

    void DepthBuffer_Destroy(DepthBuffer* dbuff)
    {
        free(dbuff->data);
        free(dbuff);
    }

    void DepthBuffer_Set(DepthBuffer* dbuff, int x, int y, float depth)
    {
        dbuff->data[x*dbuff->w + y] = depth;
    }


    float DepthBuffer_Get(DepthBuffer* dbuff, int x, int y)
    {
        return dbuff->data[x*dbuff->w + y];
    }

#endif




    /*##########################################################################
     * create vector and  matrix
     *##########################################################################*/



    Matrix3x4* Matrix3x4_Create(float m00, float m10, float m20,
                                float m01, float m11, float m21,
                                float m02, float m12, float m22,
                                float m03, float m13, float m23)
    {
        Matrix3x4* m;
        m = (Matrix3x4*)malloc( sizeof(Matrix3x4) );

        MAT3GET(m, 0, 0) = m00;
        MAT3GET(m, 1, 0) = m10;
        MAT3GET(m, 2, 0) = m20;

        MAT3GET(m, 0, 1) = m01;
        MAT3GET(m, 1, 1) = m11;
        MAT3GET(m, 2, 1) = m21;

        MAT3GET(m, 0, 2) = m02;
        MAT3GET(m, 1, 2) = m12;
        MAT3GET(m, 2, 2) = m22;

        MAT3GET(m, 0, 3) = m03;
        MAT3GET(m, 1, 3) = m13;
        MAT3GET(m, 2, 3) = m23;

        return m;
    }


    void Matrix3x4_Destroy(Matrix3x4* m)
    {
        free(m);
    }




    /* world coordinates to camera coordinates */
    Camera* CAM_Create( int Width,
                        int Height,
                        int WPixelsPERUnit,
                        int HPixelsPERUnit )
    {
        Camera* camera = (Camera*)malloc( sizeof(Camera) );

        if (camera == NULL)
        {
            fprintf(stderr, "CAM_Create error: cannot create camera object!\n");
        }

        camera->image = TCIMG_Create(Width, Height);

        camera->position = Vector3D_Create(0.0f, 0.0f, 20.0f);
        camera->lookat   = Vector3D_Create(0.0f, 0.0f, 0.0f);
        camera->up       = Vector3D_Create(0.0f, 1.0f, 0.0f);

        camera->look     = Vector3D_Create(0.0f, 0.0f, 0.0f);
        camera->right    = Vector3D_Create(1.0f, 0.0f, 0.0f);

        camera->wrl2cam  = Matrix3x4_Create(1.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f,
                                            0.0f, 0.0f, 1.0f);


        camera->WPixelsPERUnit = WPixelsPERUnit;
        camera->HPixelsPERUnit = HPixelsPERUnit;

        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
        CAM_CompuWRL2CAM(camera);

        camera->BackFaceFlag = 0;
        return camera;
    }




    void CAM_Destroy(Camera* camera)
    {
        TCIMG_Destroy(camera->image);

        Vector3D_Destroy(camera->position);
        Vector3D_Destroy(camera->lookat);
        Vector3D_Destroy(camera->look);
        Vector3D_Destroy(camera->right);
        Vector3D_Destroy(camera->up);
        Matrix3x4_Destroy(camera->wrl2cam);

        free(camera);
    }




    /*##############################################################
     *
     *  camera setup external parameters
     *
     * ###################################################################*/

    void CAM_SetPosition(Camera* camera, Vector3D* position)
    {
        *(camera->position) = *position;
    }

    void CAM_SetLookAt(Camera* camera, Vector3D* lookat)
    {
        *camera->lookat = *lookat;
        Vector3D_Minus(camera->lookat, camera->position, camera->look);
        Vector3D_Normalize(camera->look);
    }

    void CAM_SetLook(Camera* camera, Vector3D* look)
    {
        *(camera->look) = *look;
        Vector3D_Normalize(camera->look);
    }

    void CAM_SetRight(Camera* camera, Vector3D* right)
    {
        *(camera->right) = *right;
        Vector3D_Normalize(camera->right);
    }

    void CAM_SetUp(Camera* camera, Vector3D* up)
    {
        *(camera->up) = *up;
        Vector3D_Normalize(camera->up);
    }


    void CAM_MoveUpFixLookat(Camera* camera, float StepLength)
    {
        Vector3D_axpy(camera->up, StepLength, camera->position);
        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }

    void CAM_MoveForwardFixLookat(Camera* camera, float StepLength)
    {
        Vector3D_axpy(camera->look, StepLength, camera->position);
        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }

    void CAM_MoveRightFixLookat(Camera* camera, float StepLength)
    {
        Vector3D_axpy(camera->right, StepLength, camera->position);
        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }

    void CAM_MoveUp(Camera* camera, float StepLength)
    {
        Vector3D_axpy(camera->up, StepLength, camera->position);
        Vector3D_axpy(camera->up, StepLength, camera->lookat);
        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }

    void CAM_MoveForward(Camera* camera, float StepLength)
    {
        Vector3D_axpy(camera->look, StepLength, camera->position);
        Vector3D_axpy(camera->look, StepLength, camera->lookat);
        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }

    void CAM_MoveRight(Camera* camera, float StepLength)
    {
        Vector3D_axpy(camera->right, StepLength, camera->position);
        Vector3D_axpy(camera->right, StepLength, camera->lookat);
        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }


    void CAM_RotateUp(Camera* camera, float theta)
    {
        Vector3D r;
        float radius, StepLength;

        Vector3D_Minus(camera->position, camera->lookat, &r);
        radius = Vector3D_Norm(&r);
        StepLength = radius*tan(theta);
        Vector3D_axpy(camera->up, StepLength, camera->position);

        Vector3D_Minus(camera->position, camera->lookat, &r);
        Vector3D_Normalize(&r);
        Vector3D_Scale(&r, radius);
        Vector3D_Add(camera->lookat, &r, camera->position);

        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }


    void CAM_RotateRight(Camera* camera, float theta)
    {
        Vector3D r;
        float radius, StepLength;

        Vector3D_Minus(camera->position, camera->lookat, &r);
        radius = Vector3D_Norm(&r);
        StepLength = radius*tan(theta);
        Vector3D_axpy(camera->right, StepLength, camera->position);

        Vector3D_Minus(camera->position, camera->lookat, &r);
        Vector3D_Normalize(&r);
        Vector3D_Scale(&r, radius);
        Vector3D_Add(camera->lookat, &r, camera->position);

        CAM_SetAllFromPositionLookat(camera, camera->position, camera->lookat);
    }


    void CAM_SetAllFromPositionLookat(Camera* camera, Vector3D* position, Vector3D* lookat)
    {
        if (camera->position != position) *(camera->position) = *position;
        if (camera->lookat != lookat) *(camera->lookat) = *lookat;

        Vector3D_Minus(camera->lookat, camera->position, camera->look);
        Vector3D_Normalize(camera->look);

        Vector3D_CrossProduct(camera->look, camera->up, camera->right);
        Vector3D_Normalize(camera->right);

        Vector3D_CrossProduct(camera->right, camera->look, camera->up);
        Vector3D_Normalize(camera->up);

        assert( CAM_ValidateCoord(camera) );
    }


    int CAM_ValidateCoord(Camera* camera)
    {
        Vector3D tmp;
        Vector3D_CrossProduct(camera->up, camera->right, &tmp);
        tmp.data[0] -= VEC3GET(camera->look, 0);
        tmp.data[1] -= VEC3GET(camera->look, 1);
        tmp.data[2] -= VEC3GET(camera->look, 2);
        if ( Vector3D_Norm(&tmp) > 1e-3 )
        {
            fprintf(stderr, "up-right-look check: %f\n", Vector3D_Norm(&tmp));
            goto PRINT;
        }

        Vector3D_CrossProduct(camera->right, camera->look, &tmp);
        tmp.data[0] -= VEC3GET(camera->up, 0);
        tmp.data[1] -= VEC3GET(camera->up, 1);
        tmp.data[2] -= VEC3GET(camera->up, 2);
        if ( Vector3D_Norm(&tmp) > 1e-6 )
        {
            fprintf(stderr, "look-right-up check: %f\n", Vector3D_Norm(&tmp));
            goto PRINT;
        }

        Vector3D_CrossProduct(camera->look, camera->up, &tmp);
        tmp.data[0] -= VEC3GET(camera->right, 0);
        tmp.data[1] -= VEC3GET(camera->right, 1);
        tmp.data[2] -= VEC3GET(camera->right, 2);
        if ( Vector3D_Norm(&tmp) > 1e-6 )
        {
            fprintf(stderr, "up-look-right check: %f\n", Vector3D_Norm(&tmp));
            goto PRINT;
        }

        return 1;

PRINT:
        fprintf(stderr, "right: [%f, %f, %f]\n",
                VEC3GET(camera->right, 0),
                VEC3GET(camera->right, 1),
                VEC3GET(camera->right, 2));

        fprintf(stderr, "up: [%f, %f, %f]\n",
                VEC3GET(camera->up, 0),
                VEC3GET(camera->up, 1),
                VEC3GET(camera->up, 2));

        fprintf(stderr, "look: [%f, %f, %f]\n",
                VEC3GET(camera->look, 0),
                VEC3GET(camera->look, 1),
                VEC3GET(camera->look, 2));

        return 0;
    }


    void CAM_CompuWRL2CAM(Camera* camera)
    {
        assert( CAM_ValidateCoord(camera) );

        MAT3GET(camera->wrl2cam, 0, 0) = VEC3GET(camera->right, 0);
        MAT3GET(camera->wrl2cam, 0, 1) = VEC3GET(camera->right, 1);
        MAT3GET(camera->wrl2cam, 0, 2) = VEC3GET(camera->right, 2);

        MAT3GET(camera->wrl2cam, 1, 0) = VEC3GET(camera->up, 0);
        MAT3GET(camera->wrl2cam, 1, 1) = VEC3GET(camera->up, 1);
        MAT3GET(camera->wrl2cam, 1, 2) = VEC3GET(camera->up, 2);

        MAT3GET(camera->wrl2cam, 2, 0) = VEC3GET(camera->look, 0);
        MAT3GET(camera->wrl2cam, 2, 1) = VEC3GET(camera->look, 1);
        MAT3GET(camera->wrl2cam, 2, 2) = VEC3GET(camera->look, 2);

        MAT3GET(camera->wrl2cam, 0, 3) = -VEC3GET(camera->position, 0);
        MAT3GET(camera->wrl2cam, 1, 3) = -VEC3GET(camera->position, 1);
        MAT3GET(camera->wrl2cam, 2, 3) = -VEC3GET(camera->position, 2);
    }



    void CAM_Perspective(Camera* camera, Vector3D* x, int* xp, int* yp, Vector3D* tmp)
    {
        Translate_And_Rotate(camera->wrl2cam, x, tmp);
        CAM_Float2Pixel(camera, tmp, xp, yp);
    }


    void CAM_Float2Pixel(Camera* camera, Vector3D* v, int* x, int* y)
    {
        if ( fabs( VEC3GET(v, 2) ) < 1e-3)
        {
            *x = INT_MAX;
            *y = INT_MAX;
        }
        else
        {
            *x = (int)round(VEC3GET(v, 0) / VEC3GET(v, 2) * camera->WPixelsPERUnit);
            *y = (int)round(VEC3GET(v, 1) / VEC3GET(v, 2) * camera->HPixelsPERUnit);

            *x += ( camera->image->ImgWidth  / 2 );
            *y += ( camera->image->ImgHeight / 2 );
        }
    }


    void CAM_Pixel2Float(Camera* camera, int x, int y, Vector3D* v, int flag)
    {
        x -= ( camera->image->ImgWidth  / 2 );
        y -= ( camera->image->ImgHeight / 2 );

        Vector3D vx;
        VEC3TAK(vx, 0) = (float)x / (float)camera->WPixelsPERUnit;
        VEC3TAK(vx, 1) = (float)y / (float)camera->HPixelsPERUnit;
        VEC3TAK(vx, 2) = 1.0f;

        if (flag > 0)
        {
            Inverse_Translate_And_Rotate(camera->wrl2cam, &vx, v);
        }
        else
        {
            *v = vx;
        }
    }


    void CAM_Pixel2Ray(Camera* camera, int x, int y, Vector3D* ray)
    {
        Vector3D v;
        CAM_Pixel2Float(camera, x, y, &v, 1);
        Vector3D_Minus(&v, camera->position, ray);
    }


    void CAM_VList_Perspective(Camera* camera, VList* vlist)
    {
        int i;
        for (i = 0; i < vlist->len; ++i)
        {
            CAM_Perspective( camera, &vlist->v[i], &vlist->p[i].x, &vlist->p[i].y, &vlist->vx[i] );
        }
    }


    void CAM_Face_Draw(Camera* camera, Face* f, VList* vlist)
    {
        int i;
        Point2D* p;
        p = Face_2DGet(vlist, f, 0);
        TCIMG_MoveTo( camera->image, p->x, p->y);
        for (i = 1; i < f->len; ++i)
        {
            p = Face_2DGet(vlist, f, i);
            TCIMG_LineTo(camera->image, p->x, p->y);
        }

        p = Face_2DGet(vlist, f, 0);
        TCIMG_LineTo(camera->image, p->x, p->y);
    }


    void CAM_Triangle_Fill(Camera* camera, Face* f, VList* vlist)
    {
        Point2D* p1;
        Point2D* p2;
        Point2D* p3;

        assert(f->len == 3);

        p1 = Face_2DGet(vlist, f, 0);
        p2 = Face_2DGet(vlist, f, 1);
        p3 = Face_2DGet(vlist, f, 2);

        TCIMG_TriangleFill(camera->image, p1, p2, p3);
    }


    void CAM_Triangle_FillColor(Camera* camera, Face* f, RGBColor* color, VList* vlist)
    {
        Point2D* p1;
        Point2D* p2;
        Point2D* p3;
        assert(f->len == 3);
        p1 = Face_2DGet(vlist, f, 0);
        p2 = Face_2DGet(vlist, f, 1);
        p3 = Face_2DGet(vlist, f, 2);

        TCIMG_TriangleFillColor(camera->image, p1, p2, p3, color);
    }


    void CAM_Triangle_FillGradient(Camera* camera, Face* f, RGBColor* c1, RGBColor* c2, RGBColor* c3, VList* vlist)
    {
        Point2D* p1;
        Point2D* p2;
        Point2D* p3;
        assert(f->len == 3);
        p1 = Face_2DGet(vlist, f, 0);
        p2 = Face_2DGet(vlist, f, 1);
        p3 = Face_2DGet(vlist, f, 2);

        TCIMG_TriangleFillGradient(camera->image, p1, p2, p3, c1, c2, c3);
    }


    void CAM_Edge_Draw(Camera* camera, Edge* e, VList* vlist)
    {
        TCIMG_MoveTo(camera->image, vlist->p[e->v[0]].x, vlist->p[e->v[0]].y);
        TCIMG_LineTo(camera->image, vlist->p[e->v[1]].x, vlist->p[e->v[1]].y);
    }


    void CAM_Edge_DrawColor(Camera* camera, Edge* e, VList* vlist, RGBColor* c1, RGBColor* c2)
    {
        TCIMG_LineToGradient(camera->image, &vlist->p[e->v[0]], &vlist->p[e->v[1]], c1, c2);
    }



    void CAM_Face_DrawColor(Camera* camera, Face* f, VList* vlist, RGBColor* rgb)
    {
        int i;
        TCIMG_SetPenClr(camera->image, RGBColor_GetR(rgb), RGBColor_GetG(rgb), RGBColor_GetB(rgb));
        TCIMG_MoveTo( camera->image, vlist->p[f->v[0]].x, vlist->p[f->v[0]].y);
        for (i = 1; i < f->len; ++i)
        {
            TCIMG_LineTo(camera->image, vlist->p[f->v[i]].x, vlist->p[f->v[i]].y);
        }

        TCIMG_LineTo(camera->image, vlist->p[f->v[0]].x, vlist->p[f->v[0]].y);
    }


    int CAM_SetBackFace(Camera* camera, int flag)
    {
        return camera->BackFaceFlag = flag;
    }

    int CAM_IsBackFace(Camera* camera, VList* vlist, Face* f)
    {
        Vector3D view;
        float isbf;

        Vector3D_Minus(&vlist->v[f->v[0]], camera->position, &view);
        isbf = Vector3D_InnerProduct(&view, &f->vn);

        if (isbf > 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }


    /*
     *
     *   [+a, +b]    [+d, -b]
     *   [+c, +d] =  [-c, +a]
     * */
    int AUX_ToBary(Point2D* p1, Point2D* p2, Point2D* p3, Point2D* p, float* a1, float* a2, float* a3)
    {
        Point2D v1, v2, b;
        Point2D_Minus(p2, p1, &v1);
        Point2D_Minus(p3, p1, &v2);
        Point2D_Minus(p,  p1, &b);
        int det = v1.x*v2.y-v1.y*v2.x;

        if (det == 0)
        {
            return 0;
        }

        *a2 = (float)(+v2.y*b.x - v2.x*b.y) / (float)det;
        *a3 = (float)(-v1.y*b.x + v1.x*b.y) / (float)det;
        *a1 = 1.0f - *a2 - *a3;

        return 1;
    }

    void CAM_DrawTriangle( Camera* camera,
                           Point2D* p1, Point2D* p2, Point2D* p3,
                           RGBColor* c1, RGBColor* c2, RGBColor* c3,
                           float d1, float d2, float d3 )
    {
        int xmin, xmax, ymin, ymax;
        int i, j;
        float L1, L2, L3, a1, a2, a3, d;
        RGBColor PixColor;
        Point2D p;

        // Draw vertex
        TCIMG_Depth_SetPixelColor(camera->image, p1, c1, d1);
        TCIMG_Depth_SetPixelColor(camera->image, p2, c2, d2);
        TCIMG_Depth_SetPixelColor(camera->image, p3, c3, d3);

        L1 = Point2D_Distance(p1, p2);
        L2 = Point2D_Distance(p2, p3);
        L3 = Point2D_Distance(p3, p1);

        PList_Line(camera->image->PL0, p1, p2);
        PList_Line(camera->image->PL1, p2, p3);
        PList_Line(camera->image->PL2, p3, p1);


        // Draw edge
        for (i = 1; i < camera->image->PL0->len-1; ++i)
        {
            a1 = (float)Point2D_Distance(&camera->image->PL0->p[i], p1) / L1;
            RGBColor_LinearGradient(c2, c1, a1, &PixColor);
            d = d2*a1 + d1*(1.0f-a1);
            TCIMG_Depth_SetPixelColor(camera->image, &camera->image->PL0->p[i], &PixColor, d);
        }

        for (i = 1; i < camera->image->PL1->len-1; ++i)
        {
            a1 = (float)Point2D_Distance(&camera->image->PL1->p[i], p2) / L2;
            RGBColor_LinearGradient(c3, c2, a1, &PixColor);
            d = d3*a1 + d2*(1.0f-a1);
            TCIMG_Depth_SetPixelColor(camera->image, &camera->image->PL1->p[i], &PixColor, d);
        }


        for (i = 1; i < camera->image->PL2->len-1; ++i)
        {
            a1 = (float)Point2D_Distance(&camera->image->PL2->p[i], p3) / L3;
            RGBColor_LinearGradient(c1, c3, a1, &PixColor);
            d = d1*a1 + d3*(1.0f-a1);
            TCIMG_Depth_SetPixelColor(camera->image, &camera->image->PL2->p[i], &PixColor, d);
        }


        p1->z = 1;
        p2->z = 1;
        p3->z = 1;
        p.z   = 1;

        xmin = MIN3(p1->x, p2->x, p3->x);
        xmax = MAX3(p1->x, p2->x, p3->x);
        ymin = MIN3(p1->y, p2->y, p3->y);
        ymax = MAX3(p1->y, p2->y, p3->y);

        for (j = ymin; j <= ymax; ++j)
        {
            for (i = xmin; i <= xmax; ++i)
            {
                /* if (i, j) in the triangle */
                p.x = i;
                p.y = j;
                if ( Point2D_InTriangle(&p, p1, p2, p3) )
                {
                    int info = AUX_ToBary(p1, p2, p3, &p, &a1, &a2, &a3);
                    if (info == 0) return;
                    assert(a1 >= -0.00001f);
                    assert(a2 >= -0.00001f);
                    assert(a3 >= -0.00001f);

                    RGBColor_BaryInterpolation(c1, c2, c3, a1, a2, a3, &PixColor);
                    d = a1*d1 + a2*d2 + a3*d3;
                    TCIMG_Depth_SetPixelColor(camera->image, &p, &PixColor, d);
                }
            }
        }
    }


    void CAM_DrawTriangleColor( Camera* camera,
                                Point2D* p1, Point2D* p2, Point2D* p3,
                                RGBColor* c, float d1, float d2, float d3 )
    {
        int xmin, xmax, ymin, ymax;
        int i, j;
        float L1, L2, L3, a1, a2, a3, d;
        Point2D p;

        // Draw vertex
        TCIMG_Depth_SetPixelColor(camera->image, p1, c, d1);
        TCIMG_Depth_SetPixelColor(camera->image, p2, c, d2);
        TCIMG_Depth_SetPixelColor(camera->image, p3, c, d3);

        L1 = Point2D_Distance(p1, p2);
        L2 = Point2D_Distance(p2, p3);
        L3 = Point2D_Distance(p3, p1);

        PList_Line(camera->image->PL0, p1, p2);
        PList_Line(camera->image->PL1, p2, p3);
        PList_Line(camera->image->PL2, p3, p1);

        // Draw edge
        for (i = 1; i < camera->image->PL0->len-1; ++i)
        {
            a1 = (float)Point2D_Distance(&camera->image->PL0->p[i], p1) / L1;
            d = d2*a1 + d1*(1.0f-a1);
            TCIMG_Depth_SetPixelColor(camera->image, &camera->image->PL0->p[i], c, d);
        }

        for (i = 1; i < camera->image->PL1->len-1; ++i)
        {
            a1 = (float)Point2D_Distance(&camera->image->PL1->p[i], p2) / L2;
            d = d3*a1 + d2*(1.0f-a1);
            TCIMG_Depth_SetPixelColor(camera->image, &camera->image->PL1->p[i], c, d);
        }


        for (i = 1; i < camera->image->PL2->len-1; ++i)
        {
            a1 = (float)Point2D_Distance(&camera->image->PL2->p[i], p3) / L3;
            d = d1*a1 + d3*(1.0f-a1);
            TCIMG_Depth_SetPixelColor(camera->image, &camera->image->PL2->p[i], c, d);
        }


        p1->z = 1;
        p2->z = 1;
        p3->z = 1;
        p.z   = 1;

        xmin = MIN3(p1->x, p2->x, p3->x);
        xmax = MAX3(p1->x, p2->x, p3->x);
        ymin = MIN3(p1->y, p2->y, p3->y);
        ymax = MAX3(p1->y, p2->y, p3->y);

        for (j = ymin; j <= ymax; ++j)
        {
            for (i = xmin; i <= xmax; ++i)
            {
                /* if (i, j) in the triangle */
                p.x = i;
                p.y = j;
                if ( Point2D_InTriangle(&p, p1, p2, p3) )
                {
                    int info = AUX_ToBary(p1, p2, p3, &p, &a1, &a2, &a3);
                    if (info == 0) return;
                    assert(a1 >= -0.00001f);
                    assert(a2 >= -0.00001f);
                    assert(a3 >= -0.00001f);

                    d = a1*d1 + a2*d2 + a3*d3;
                    TCIMG_Depth_SetPixelColor(camera->image, &p, c, d);
                }
            }
        }
    }




#ifdef __cplusplus
}
#endif


