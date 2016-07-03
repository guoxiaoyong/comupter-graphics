#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <limits.h>
#include <float.h>

#include <meshmo.h>
#include <geomet.h>


    VList* VList_Create(int size)
    {
        VList* vlist;

        vlist = (VList*)malloc( sizeof(VList) );
        if (vlist == NULL) return NULL;

        vlist->v    = (Vector3D*)malloc( sizeof(Vector3D)*size);
        vlist->vn   = (Vector3D*)malloc( sizeof(Vector3D)*size);
        vlist->vc   = (RGBColor*)malloc( sizeof(RGBColor)*size);
        vlist->size = size;
        vlist->len  = 0;


        /*old*/
        vlist->vx   = (Vector3D*)malloc( sizeof(Vector3D)*size);
        vlist->p    = (Point2D* )malloc( sizeof(Point2D)*size);

        return vlist;
    }


    void VList_Destroy(VList* vlist)
    {
        free(vlist->v);
        free(vlist->vn);
        free(vlist->vc);

        /*old*/
        free(vlist->vx);
        free(vlist->p);

        free(vlist);
    }

    int VList_Add(VList* vlist, Vector3D* v)
    {
        assert(vlist->len+1 <= vlist->size);
        vlist->v[vlist->len] = *v;
        vlist->len++;

        return vlist->len;
    }


    int VList_AddList(VList* vlist, VList* vadd)
    {
        int i;
        for (i = 0; i < vadd->len; ++i)
        {
            VList_Add(vlist, &vadd->v[i]);
        }

        return vlist->len;
    }

    void VList_AverageNormal(VList* vlist, FList* flist)
    {
        int i, j;
        for (i = 0; i < vlist->len; ++i)
        {
            Vector3D_Set(&vlist->vn[i], 0.0f, 0.0f, 0.0f);
        }

        for (i = 0; i < flist->len; ++i)
        {
            for (j = 0; j < flist->f[i].len; ++j)
            {
                Vector3D_Add(&flist->f[i].vn, &vlist->v[ flist->f[i].v[j] ], &vlist->vn[ flist->f[i].v[j] ]);
            }
        }

        for (i = 0; i < vlist->len; ++i)
        {
            Vector3D_Normalize(&vlist->vn[i]);
        }
    }



    float VList_Range(VList* vlist, Vector3D* Vlow, Vector3D* Vup)
    {
        int i;
        float Xlow, Xup, Ylow, Yup, Zlow, Zup;

        Xlow = +9999;
        Xup  = -9999;

        Ylow = +9999;
        Yup  = -9999;

        Zlow = +9999;
        Zup  = -9999;

        for (i = 0; i < vlist->len; i++)
        {
            /* find Xlow Xup */
            if ( VEC3TAK(vlist->v[i], 0) < Xlow )
            {
                Xlow = VEC3TAK(vlist->v[i], 0);
            }

            if ( VEC3TAK(vlist->v[i], 0) > Xup)
            {
                Xup = VEC3TAK(vlist->v[i], 0);
            }

            /* find Ylow Yup */
            if ( VEC3TAK(vlist->v[i], 1) < Ylow )
            {
                Ylow = VEC3TAK(vlist->v[i], 1);
            }

            if ( VEC3TAK(vlist->v[i], 1) > Yup)
            {
                Yup = VEC3TAK(vlist->v[i], 1);
            }

            /* find Zlow Zup */
            if ( VEC3TAK(vlist->v[i], 2) < Zlow )
            {
                Zlow = VEC3TAK(vlist->v[i], 2);
            }

            if ( VEC3TAK(vlist->v[i], 2) > Zup)
            {
                Zup = VEC3TAK(vlist->v[i], 2);
            }
        }

        if (Vlow != NULL)
        {
            VEC3GET(Vlow, 0) = Xlow;
            VEC3GET(Vlow, 1) = Ylow;
            VEC3GET(Vlow, 2) = Zlow;
        }

        if (Vup != NULL)
        {
            VEC3GET(Vup, 0) = Xup;
            VEC3GET(Vup, 1) = Yup;
            VEC3GET(Vup, 2) = Zup;
        }

        float r = fabs(Xlow);
        r = r > fabs(Ylow)?r:Ylow;
        r = r > fabs(Zlow)?r:Zlow;
        r = r > fabs(Xup)?r:Xup;
        r = r > fabs(Yup)?r:Yup;
        r = r > fabs(Zup)?r:Zup;
        return r;
    }


    float VList_RangeX(VList* vlist, Vector3D* Vlow, Vector3D* Vup)
    {
        int i;
        float Xlow, Xup, Ylow, Yup, Zlow, Zup;

        Xlow = +9999;
        Xup  = -9999;

        Ylow = +9999;
        Yup  = -9999;

        Zlow = +9999;
        Zup  = -9999;

        for (i = 0; i < vlist->len; i++)
        {
            /* find Xlow Xup */
            if ( VEC3TAK(vlist->vx[i], 0) < Xlow )
            {
                Xlow = VEC3TAK(vlist->vx[i], 0);
            }

            if ( VEC3TAK(vlist->vx[i], 0) > Xup)
            {
                Xup = VEC3TAK(vlist->vx[i], 0);
            }

            /* find Ylow Yup */
            if ( VEC3TAK(vlist->vx[i], 1) < Ylow )
            {
                Ylow = VEC3TAK(vlist->vx[i], 1);
            }

            if ( VEC3TAK(vlist->vx[i], 1) > Yup)
            {
                Yup = VEC3TAK(vlist->vx[i], 1);
            }

            /* find Zlow Zup */
            if ( VEC3TAK(vlist->vx[i], 2) < Zlow )
            {
                Zlow = VEC3TAK(vlist->vx[i], 2);
            }

            if ( VEC3TAK(vlist->vx[i], 2) > Zup)
            {
                Zup = VEC3TAK(vlist->vx[i], 2);
            }
        }

        if (Vlow != NULL)
        {
            VEC3GET(Vlow, 0) = Xlow;
            VEC3GET(Vlow, 1) = Ylow;
            VEC3GET(Vlow, 2) = Zlow;
        }

        if (Vup != NULL)
        {
            VEC3GET(Vup, 0) = Xup;
            VEC3GET(Vup, 1) = Yup;
            VEC3GET(Vup, 2) = Zup;
        }

        float r = fabs(Xlow);
        r = r > fabs(Ylow)?r:Ylow;
        r = r > fabs(Zlow)?r:Zlow;
        r = r > fabs(Xup)?r:Xup;
        r = r > fabs(Yup)?r:Yup;
        r = r > fabs(Zup)?r:Zup;
        return r;
    }


    void VList_BoundingBox(VList* vlist)
    {
        int i;
        float Xlow, Xup, Ylow, Yup, Zlow, Zup;

        Xlow = +FLT_MAX;
        Xup  = -FLT_MAX;

        Ylow = +FLT_MAX;
        Yup  = -FLT_MAX;

        Zlow = +FLT_MAX;
        Zup  = -FLT_MAX;

        for (i = 0; i < vlist->len; i++)
        {
            /* find Xlow Xup */
            if ( VEC3TAK(vlist->v[i], 0) < Xlow )
            {
                Xlow = VEC3TAK(vlist->v[i], 0);
            }

            if ( VEC3TAK(vlist->v[i], 0) > Xup)
            {
                Xup = VEC3TAK(vlist->v[i], 0);
            }

            /* find Ylow Yup */
            if ( VEC3TAK(vlist->v[i], 1) < Ylow )
            {
                Ylow = VEC3TAK(vlist->v[i], 1);
            }

            if ( VEC3TAK(vlist->v[i], 1) > Yup)
            {
                Yup = VEC3TAK(vlist->v[i], 1);
            }

            /* find Zlow Zup */
            if ( VEC3TAK(vlist->v[i], 2) < Zlow )
            {
                Zlow = VEC3TAK(vlist->v[i], 2);
            }

            if ( VEC3TAK(vlist->v[i], 2) > Zup)
            {
                Zup = VEC3TAK(vlist->v[i], 2);
            }
        }

        VEC3TAK(vlist->bbox[0], 0) = Xlow;
        VEC3TAK(vlist->bbox[0], 1) = Ylow;
        VEC3TAK(vlist->bbox[0], 2) = Zlow;

        VEC3TAK(vlist->bbox[1], 0) = Xup;
        VEC3TAK(vlist->bbox[1], 1) = Yup;
        VEC3TAK(vlist->bbox[1], 2) = Zup;
    }




    void VList_Print(VList* vlist)
    {
        int i;
        for (i = 0; i < vlist->len; ++i)
        {
            printf("v[%d] = [%f, %f, %f];\n", i,
                   VEC3TAK(vlist->v[i], 0),
                   VEC3TAK(vlist->v[i], 1),
                   VEC3TAK(vlist->v[i], 2) );
        }
    }


    /**#############################################################3
     * EList member functions
     */
    EList* EList_Create(int size)
    {
        EList* elist;

        elist = (EList*)malloc(sizeof(EList));
        elist->vlist = NULL;
        elist->size  = size;
        elist->len   = 0;
        elist->e     = (Edge*)calloc(sizeof(Edge), size);

        return elist;
    }

    void EList_Destroy(EList* elist)
    {
        free(elist->e);
        free(elist);
    }

    void EList_Print(EList* elist)
    {
        int i;
        fprintf(stderr, "len = %d\n", elist->len);
        for (i = 0; i < elist->len; ++i)
        {
            fprintf(stderr, "e[%d] = [%d, %d]\n", i, (elist->e[i]).v[0], (elist->e[i]).v[1]);
        }
    }

    int EList_Add(EList* elist, Edge* e)
    {
        assert(elist->len+1 <= elist->size);
        elist->e[elist->len] = *e;
        elist->e[elist->len].visible = 1;
        elist->len++;

        return elist->len;
    }

    int EList_AddList(EList* elist, EList* eadd)
    {
        int i;
        for (i = 0; i < eadd->len; ++i)
        {
            EList_Add(elist, &eadd->e[i]);
        }

        return elist->len;
    }

    int EList_SetVisible(EList* elist, int n, int flag)
    {
        assert(n <= elist->len);
        (elist->e[n]).visible = flag;

        return n;
    }

    int EList_Find(EList* elist, Edge* e)
    {
        int i;
        for (i = 0; i < elist->len; ++i)
        {
            if ( (e->v[0] == elist->e[i].v[0]) && (e->v[1] == elist->e[i].v[1]) ) return (i+1);
            if ( (e->v[1] == elist->e[i].v[0]) && (e->v[0] == elist->e[i].v[1]) ) return (i+1);
        }

        return 0;
    }


    int EList_FromFList(EList* elist, FList* flist)
    {
        int i, j, len;
        Edge e;

        for (i = 0; i < flist->len; ++i)
        {
            len = flist->f[i].len;
            for (j = 0; j < len; ++j)
            {
                e.v[0] = flist->f[i].v[(j+0)%len];
                e.v[1] = flist->f[i].v[(j+1)%len];
                if ( !EList_Find(elist, &e) ) EList_Add(elist, &e);
            }
        }

        return elist->len;
    }


    /**###################################################################
     */
    FList* FList_Create(int size)
    {
        FList* flist;

        flist = (FList*)malloc(sizeof(FList));
        flist->vlist = NULL;
        flist->size  = size;
        flist->len   = 0;
        flist->f     = (Face*)calloc(sizeof(Face), size);

        return flist;
    }

    void FList_Destroy(FList* flist)
    {
        free(flist->f);
        free(flist);
    }


    int FList_Add(FList* flist, Face* f)
    {
        assert(flist->len+1 <= flist->size);
        flist->f[flist->len] = *f;
        flist->len++;

        return flist->len;
    }


    void FList_Print(FList* flist)
    {
        int i, j;
        for (i = 0; i < flist->len; ++i)
        {
            printf("f[%d] = [", i);
            for (j = 0; j < (flist->f[i]).len-1; ++j)
            {
                printf("%d, ", (flist->f[i]).v[j]);
            }
            printf("%d];\n", (flist->f[i]).v[j]);
        }
    }


    int FList_SetVisible(FList* flist, int n, int flag)
    {
        assert(n <= flist->len);
        (flist->f[n]).visible = flag;

        return n;
    }

    int FList_FaceNormal(VList* vlist, FList* flist)
    {
        int i;
        for (i = 0; i < flist->len; ++i)
        {
            Face_3DNormal(vlist, &flist->f[i]);
        }

        return flist->len;
    }

    int FList_NormalizeVN(VList* vlist, FList* flist)
    {
        int i;
        for (i = 0; i < flist->len; ++i)
        {
            Face_3DNormal(vlist, &flist->f[i]);
            Vector3D_Normalize( &(flist->f[i].vn) );
        }

        return flist->len;
    }


    void FList_TriangleBoundingBox(VList* vlist, FList* flist)
    {
        int i;
        for (i = 0; i < flist->len; ++i)
        {
            Face_3DTriangleBoundingBox(vlist, &flist->f[i]);
        }
    }

    void FList_BoundingBox(FList* flist)
    {
        int i;
        float xmin, ymin, zmin, xmax, ymax, zmax;

        xmin = VEC3TAK((flist->f[0]).bbox[0], 0);
        ymin = VEC3TAK((flist->f[0]).bbox[0], 1);
        zmin = VEC3TAK((flist->f[0]).bbox[0], 2);

        xmax = VEC3TAK((flist->f[0]).bbox[1], 0);
        ymax = VEC3TAK((flist->f[0]).bbox[1], 1);
        zmax = VEC3TAK((flist->f[0]).bbox[1], 2);

        for (i = 1; i < flist->len; ++i)
        {
            if ( xmin > VEC3TAK((flist->f[i]).bbox[0], 0) )
            {
                xmin = VEC3TAK((flist->f[i]).bbox[0], 0);
            }

            if ( ymin > VEC3TAK((flist->f[i]).bbox[0], 1) )
            {
                ymin = VEC3TAK((flist->f[i]).bbox[0], 1);
            }

            if ( zmin > VEC3TAK((flist->f[i]).bbox[0], 2) )
            {
                zmin = VEC3TAK((flist->f[i]).bbox[0], 2);
            }

            if ( xmax < VEC3TAK((flist->f[i]).bbox[1], 0) )
            {
                xmax = VEC3TAK((flist->f[i]).bbox[1], 0);
            }

            if ( ymax < VEC3TAK((flist->f[i]).bbox[1], 1) )
            {
                ymax = VEC3TAK((flist->f[i]).bbox[1], 1);
            }

            if ( zmax < VEC3TAK((flist->f[i]).bbox[1], 2) )
            {
                zmax = VEC3TAK((flist->f[i]).bbox[1], 2);
            }
        }

        VEC3TAK(flist->bbox[0], 0 ) = xmin;
        VEC3TAK(flist->bbox[0], 1 ) = ymin;
        VEC3TAK(flist->bbox[0], 2 ) = zmin;

        VEC3TAK(flist->bbox[1], 0 ) = xmax;
        VEC3TAK(flist->bbox[1], 1 ) = ymax;
        VEC3TAK(flist->bbox[1], 2 ) = zmax;

        VEC3TAK(flist->bbox[2], 0) = (xmin+xmax)/2.0f;
        VEC3TAK(flist->bbox[2], 1) = (ymin+ymax)/2.0f;
        VEC3TAK(flist->bbox[2], 2) = (zmin+zmax)/2.0f;

        VEC3TAK(flist->bbox[3], 0) = (xmax-xmin);
        VEC3TAK(flist->bbox[3], 1) = (ymax-ymin);
        VEC3TAK(flist->bbox[3], 2) = (zmax-zmin);
    }


    void FList_UpdateBboxshift(FList* flist, Vector3D* origin)
    {
        Vector3D_Minus(&flist->bbox[0], origin, &flist->bboxshift[0]);
        Vector3D_Minus(&flist->bbox[1], origin, &flist->bboxshift[1]);
    }


    void FList_TriangleBaryCoord(VList* vlist, FList* flist)
    {
        int i;
        for (i = 0; i < flist->len; ++i)
        {
            Face_3DBaryCoord(vlist, &flist->f[i]);
        }
    }

    void FList_UpdateFaceBboxshift(FList* flist, Vector3D* origin)
    {
        int i;
        FList_UpdateBboxshift(flist, origin);
        for (i = 0; i < flist->len; ++i)
        {
            Face_UpdateBboxshift(&flist->f[i], origin);
        }
    }





    /**#############################################################
     * Edge member function
     * */

    int Edge_Equal(Edge* e1, Edge* e2)
    {
        if ((e1->v[0] == e2->v[0]) && (e1->v[1] == e2->v[1])) return 1;
        if ((e1->v[0] == e2->v[1]) && (e1->v[1] == e2->v[0])) return 1;

        return 0;
    }

    int Edge_IsTriangleEdge(Edge* e, Face* f)
    {
        Edge e1, e2, e3;
        assert(f->len == 3);
        e1.v[0] = f->v[0];
        e1.v[1] = f->v[1];

        e2.v[0] = f->v[1];
        e2.v[1] = f->v[2];

        e3.v[0] = f->v[2];
        e3.v[1] = f->v[0];

        if ( Edge_Equal(e, &e1) || Edge_Equal(e, &e2) || Edge_Equal(e, &e3) )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    void Edge_Sort(Edge* e, int flag)
    {
        int tmp;

        if ( e->v[0] > e->v[1] && flag == 0)
        {
            tmp = e->v[0];
            e->v[0] = e->v[1];
            e->v[1] = tmp;

            return;
        }

        if ( e->v[0] < e->v[1] && flag > 0)
        {
            tmp = e->v[0];
            e->v[0] = e->v[1];
            e->v[1] = tmp;

            return;
        }
    }

    Point2D* Edge_2DGet(VList* vlist, Edge* e, int n)
    {
        assert( n == 0 || n == 1 );
        return &vlist->p[ e->v[n] ];
    }

    Vector3D* Edge_3DGet(VList* vlist, Edge* e, int n)
    {
        assert( n == 0 || n == 1 );
        return &vlist->v[ e->v[n] ];
    }

    Vector3D* Edge_3DGetX(VList* vlist, Edge* e, int n)
    {
        assert(n>=0);
        assert(n<2);
        return &vlist->vx[ e->v[n] ];
    }

    /*
    Vector3D* Edge_2DPList(VList* vlist, Edge* e)
    {
    }
    */


    /*FIXME*/
    /*int Edge_SeeTriangleList(VList* vlist, FList* flist, Edge* e, Vector3D* origin, EList* eltmp, EList* elout, VList* vlout)
    {
        int i;

        for (i = 0; i < flist->len; ++i)
        {
            Edge_SeeTriangle(vlist, &flist->f[i], e, origin, elout, vlout);
            VList_AddList(vlist, vlout);
            EList_AddList(elout, eltmp);
        }

        return elout->len;
    }*/


    /*FIXME*/
#if 0
    int Edge_SeeTriangle(VList* vlist, Face* f, Edge* e, Vector3D* origin, EList* elout, VList* vlout)
    {
        int info, isEdgeBeforeTriangle;

        PList plist;
        Point2D p[2];
        plist.p    = p;
        plist.len  = 2;
        plist.size = 2;

        info = GEOMET_2DEdgeTriangleCross(vlist, e, f, &plist);
        if (info == 3) /* edge and triangle are faraway */
        {
            elout->e[0] = *e;
            elout->len  = 1;
            vlout->len  = 0;
            return 1;
        }

        isEdgeBeforeTriangle = GEOMET_3DEdgeBeforeTriangle(vlist, e, f, origin);
        if (isEdgeBeforeTriangle == 1 || isEdgeBeforeTriangle == 0)
        {
            elout->e[0] = *e;
            elout->len  = 1;
            vlout->len  = 0;
            return 1;
        }
#endif

#if 0
        /* edge in triangle */
        if (info == 0)
        {
            elout->len = 0;
            vlout->len = 0;
            return 0;
        }

        if (info == 1)
        {
            CAM_Pixel2Float(xx);
            GEOMET_3DLineCrossVector3D(origin, x, b1, b2, x);
            VList_Add(vlist, x);
            isEdgeBeforeTriangle = GEOMET_3DEdgeBeforeTriangle(vlist, e, f, origin);
            isEdgeBeforeTriangle = GEOMET_3DEdgeBeforeTriangle(vlist, e, f, origin);
        }


        if (info == 2)
        {
            /* space line intersection */
            CAM_Pixel2Float(xx);
            GEOMET_3DLineCrossVector3D(origin, x, b1, b2, x);
            VList_Add(vlist, x);
            isEdgeBeforeTriangle = GEOMET_3DEdgeBeforeTriangle(vlist, e, f, origin);
            isEdgeBeforeTriangle = GEOMET_3DEdgeBeforeTriangle(vlist, e, f, origin);
        }
#endif

        //return 0;
        //}




        Point2D* Face_2DGet(VList* vlist, Face* f, int n)
        {
            assert(n>=0);
            assert(n< f->len);
            return &vlist->p[ f->v[n] ];
        }

        Vector3D* Face_3DGet(VList* vlist, Face* f, int n)
        {
            assert(n>=0);
            assert(n< f->len);
            return &vlist->v[ f->v[n] ];
        }

        Vector3D* Face_3DGetX(VList* vlist, Face* f, int n)
        {
            assert(n>=0);
            assert(n< f->len);
            return &vlist->vx[ f->v[n] ];
        }

        void Face_GetEdge(Face* f, int n, Edge* e)
        {
            e->v[0] = f->v[n+0 % f->len];
            e->v[1] = f->v[n+1 % f->len];
        }


        void Face_3DNormal(VList* vlist, Face* f)
        {
            Vector3D* v1 = Face_3DGet(vlist, f, 0);
            Vector3D* v2 = Face_3DGet(vlist, f, 1);
            Vector3D* v3 = Face_3DGet(vlist, f, 2);
            Vector3D_FaceNormal(v1, v2, v3, &f->vn);
        }


        void Face_3DTriangleBoundingBox(VList* vlist, Face* f)
        {
            assert(f->len == 3);

            Vector3D* v1 = Face_3DGet(vlist, f, 0);
            Vector3D* v2 = Face_3DGet(vlist, f, 1);
            Vector3D* v3 = Face_3DGet(vlist, f, 2);

            VEC3TAK(f->bbox[0], 0) =  MIN3( VEC3GET(v1, 0), VEC3GET(v2, 0), VEC3GET(v3, 0) );
            VEC3TAK(f->bbox[0], 1) =  MIN3( VEC3GET(v1, 1), VEC3GET(v2, 1), VEC3GET(v3, 1) );
            VEC3TAK(f->bbox[0], 2) =  MIN3( VEC3GET(v1, 2), VEC3GET(v2, 2), VEC3GET(v3, 2) );

            VEC3TAK(f->bbox[1], 0) =  MAX3( VEC3GET(v1, 0), VEC3GET(v2, 0), VEC3GET(v3, 0) );
            VEC3TAK(f->bbox[1], 1) =  MAX3( VEC3GET(v1, 1), VEC3GET(v2, 1), VEC3GET(v3, 1) );
            VEC3TAK(f->bbox[1], 2) =  MAX3( VEC3GET(v1, 2), VEC3GET(v2, 2), VEC3GET(v3, 2) );
        }

        void Face_3DBaryCoord(VList* vlist, Face* f)
        {
            Vector3D* fv1 = Face_3DGet(vlist, f, 0);
            Vector3D* fv2 = Face_3DGet(vlist, f, 1);
            Vector3D* fv3 = Face_3DGet(vlist, f, 2);

            Vector3D_Minus(fv2, fv1, &f->bary[0]);
            Vector3D_Minus(fv3, fv1, &f->bary[1]);
        }


        void Face_UpdateBboxshift(Face* face, Vector3D* origin)
        {
            Vector3D_Minus(&face->bbox[0], origin, &face->bboxshift[0]);
            Vector3D_Minus(&face->bbox[1], origin, &face->bboxshift[1]);
        }

#ifdef __cplusplus
    }
#endif




