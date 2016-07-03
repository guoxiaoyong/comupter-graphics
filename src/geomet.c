


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <data3d.h>
#include <geomet.h>
#include <meshmo.h>


    /* solve p1+ t1(p2-p1) = q1 + t2(q2-q1) */
    int GEOMET_2DLineCross(VList* vlist, Edge* e1, Edge* e2, Point2D* Pint)
    {
        Point2D p, q, b;
        int d;
        float t, tx;

        Point2D* p1 = Edge_2DGet(vlist, e1, 0);
        Point2D* p2 = Edge_2DGet(vlist, e1, 1);

        Point2D* q1 = Edge_2DGet(vlist, e2, 0);
        Point2D* q2 = Edge_2DGet(vlist, e2, 1);

        Point2D_Minus(p2, p1, &p);
        Point2D_Minus(q2, q1, &q);
        Point2D_Minus(q1, p1, &b);

        d = p.x*q.y - p.y*q.x;
        if (d == 0) return 0;

        t  = +(float)(+q.y*b.x - q.x*b.y)/(float)d;
        tx = -(float)(-p.y*b.x + p.x*b.y)/(float)d;

        Pint->x = (int)round(p1->x + t*p.x);
        Pint->y = (int)round(p1->y + t*p.y);

        return 1;
    }

    int GEOMET_2DEdgeFaraway(VList* vlist, Edge* e1, Edge* e2)
    {
        Point2D* p1 = Edge_2DGet(vlist, e1, 0);
        Point2D* p2 = Edge_2DGet(vlist, e1, 1);

        Point2D* q1 = Edge_2DGet(vlist, e2, 0);
        Point2D* q2 = Edge_2DGet(vlist, e2, 1);

        if ( (p1->x < q1->x) && (p1->x < q2->x) && (p2->x < q1->x) && (p2->x < q2->x) ) return 1;
        if ( (p1->y < q1->y) && (p1->y < q2->y) && (p2->y < q1->y) && (p2->y < q2->y) ) return 1;

        if ( (p1->x > q1->x) && (p1->x > q2->x) && (p2->x > q1->x) && (p2->x > q2->x) ) return 1;
        if ( (p1->y > q1->y) && (p1->y > q2->y) && (p2->y > q1->y) && (p2->y > q2->y) ) return 1;

        return 0;
    }

    /* solve p1+ t1(p2-p1) = q1 + t2(q2-q1) */
    int GEOMET_2DEdgeCross(VList* vlist, Edge* e1, Edge* e2, Point2D* Pint)
    {
        Point2D p, q, b;
        int d;
        float t, tx;

        if ( GEOMET_2DEdgeFaraway(vlist, e1, e2) )
        {
            return 0;
        }

        Point2D* p1 = Edge_2DGet(vlist, e1, 0);
        Point2D* p2 = Edge_2DGet(vlist, e1, 1);

        Point2D* q1 = Edge_2DGet(vlist, e2, 0);
        Point2D* q2 = Edge_2DGet(vlist, e2, 1);

        Point2D_Minus(p2, p1, &p);
        Point2D_Minus(q2, q1, &q);
        Point2D_Minus(q1, p1, &b);

        d = p.x*q.y - p.y*q.x;
        if (d == 0) return 0;

        t  = +(float)(+q.y*b.x - q.x*b.y)/(float)d;
        tx = -(float)(-p.y*b.x + p.x*b.y)/(float)d;

        Pint->x = (int)round(p1->x + t*p.x);
        Pint->y = (int)round(p1->y + t*p.y);

        if ( t >= 0.0 && t <= 1.0 && tx >= 0.0 && tx <= 1.0 )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }


    void GEOMET_2DLineEquation(VList* vlist, Edge* e, Vector3D* v)
    {
        Point2D a = vlist->p[e->v[0]];
        Point2D b = vlist->p[e->v[1]];

        VEC3GET(v, 0) = -(b.y-a.y);
        VEC3GET(v, 1) = +(b.x-a.x);
        VEC3GET(v, 2) = VEC3GET(v, 0)*(-a.x) + VEC3GET(v, 1)*(-a.y);
    }


    float GEOMET_2DLineEval(Vector3D* v, Point2D* p)
    {
        return VEC3GET(v, 0)*p->x + VEC3GET(v, 1)*p->y + VEC3GET(v, 2);
    }


    /**
     * Whether a 2D point is in a triangle
     * */
    int GEOMET_2DInTriangle(VList* vlist, int n, Face* f)
    {
        Edge e1, e2, e3;
        Vector3D Line1, Line2, Line3, r;
        Point2D* p = &(vlist->p[n]);

        Face_GetEdge(f, 0, &e1);
        Face_GetEdge(f, 1, &e2);
        Face_GetEdge(f, 2, &e3);

        GEOMET_2DLineEquation(vlist, &e1, &Line1);
        GEOMET_2DLineEquation(vlist, &e2, &Line2);
        GEOMET_2DLineEquation(vlist, &e3, &Line3);

        VEC3TAK(r, 0)  = GEOMET_2DLineEval(&Line1, p);
        VEC3TAK(r, 1)  = GEOMET_2DLineEval(&Line2, p);
        VEC3TAK(r, 2)  = GEOMET_2DLineEval(&Line3, p);

        if ( Vector3D_SameSign(&r) == 1 )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }


    int GEOMET_2DEdgeTriangleCross(VList* vlist, Edge* e, Face* f, PList* plist)
    {
        Point2D p1, p2, p3;
        Edge e1, e2, e3;
        int info1, info2;
        int flag1, flag2, flag3;

        e1.v[0] = f->v[0];
        e1.v[1] = f->v[1];

        e2.v[0] = f->v[1];
        e2.v[1] = f->v[2];

        e3.v[0] = f->v[2];
        e3.v[1] = f->v[0];

        info1 = GEOMET_2DInTriangle(vlist, e->v[0], f);
        info2 = GEOMET_2DInTriangle(vlist, e->v[1], f);

        /* both ends of the edge are in the triangle */
        if (info1 == 1 && info2 == 1)
        {
            plist->len = 0;
            return 0;
        }
        /* one point in triangle, one point out of triangle (one intersection) */
        else if ( (info1 == 1 && info2 == 0) || (info2 == 1 && info1 == 0) )
        {
            flag1 = GEOMET_2DEdgeCross(vlist, e, &e1, &p1);
            flag2 = GEOMET_2DEdgeCross(vlist, e, &e2, &p2);
            flag3 = GEOMET_2DEdgeCross(vlist, e, &e3, &p3);

            if (flag1 == 1)
            {
                plist->p[0] = p1;
            }
            else if (flag2 == 1)
            {
                plist->p[0] = p2;
            }
            else if (flag3 == 1)
            {
                plist->p[0] = p3;
            }

            plist->len = 1;
            return 1;
        }
        /* two points out of Triangle (no intersection, two intersections) */
        else if (info1 == 0 && info2 == 0)
        {
            flag1 = GEOMET_2DEdgeCross(vlist, e, &e1, &p1);
            flag2 = GEOMET_2DEdgeCross(vlist, e, &e2, &p2);
            flag3 = GEOMET_2DEdgeCross(vlist, e, &e3, &p3);

            if (flag1 == 1 && flag2 == 1)
            {
                plist->p[0] = p1;
                plist->p[1] = p2;
                plist->len = 2;
                return 2;
            }
            else if (flag2 == 1 && flag3 == 1)
            {
                plist->p[0] = p2;
                plist->p[1] = p3;
                plist->len = 2;
                return 2;
            }
            else if (flag3 == 1 && flag1 == 1)
            {
                plist->p[0] = p3;
                plist->p[1] = p1;
                plist->len = 2;
                return 2;
            }
            else
            {
                plist->len = 0;
                return 3;
            }
        }


        return 0;
    }


    void GEOMET_3DFaceNormal(VList* vlist, Face* f, Vector3D* vn)
    {
        Vector3D v1 = vlist->v[f->v[0]];
        Vector3D v2 = vlist->v[f->v[1]];
        Vector3D v3 = vlist->v[f->v[2]];
        Vector3D_FaceNormal(&v1, &v2, &v3, vn);
    }


    /* use in computing the intersection point of an edge and a Triangle */
    int GEOMET_3DEdgeTriangleFaraway(VList* vlist, Edge* e, Face* f)
    {
        Vector3D* ev1 = Edge_3DGet(vlist, e, 0);
        Vector3D* ev2 = Edge_3DGet(vlist, e, 1);

        Vector3D* fv1 = Face_3DGet(vlist, f, 0);
        Vector3D* fv2 = Face_3DGet(vlist, f, 1);
        Vector3D* fv3 = Face_3DGet(vlist, f, 2);

        Vector3D x1, x2, x3, y1, y2, y3;

        Vector3D_Minus(ev1, fv1, &x1);
        Vector3D_Minus(ev1, fv2, &x2);
        Vector3D_Minus(ev1, fv3, &x3);

        Vector3D_Minus(ev2, fv1, &y1);
        Vector3D_Minus(ev2, fv2, &y2);
        Vector3D_Minus(ev2, fv3, &y3);


        if ( ( VEC3TAK(x1, 0) < 0 )
                && ( VEC3TAK(x2, 0) < 0 )
                && ( VEC3TAK(x3, 0) < 0 )
                && ( VEC3TAK(y1, 0) < 0 )
                && ( VEC3TAK(y2, 0) < 0 )
                && ( VEC3TAK(y3, 0) < 0 ) ) return 1;


        if ( ( VEC3TAK(x1, 1) < 0 )
                && ( VEC3TAK(x2, 1) < 0 )
                && ( VEC3TAK(x3, 1) < 0 )
                && ( VEC3TAK(y1, 1) < 0 )
                && ( VEC3TAK(y2, 1) < 0 )
                && ( VEC3TAK(y3, 1) < 0 ) ) return 1;


        if ( ( VEC3TAK(x1, 2) < 0 )
                && ( VEC3TAK(x2, 2) < 0 )
                && ( VEC3TAK(x3, 2) < 0 )
                && ( VEC3TAK(y1, 2) < 0 )
                && ( VEC3TAK(y2, 2) < 0 )
                && ( VEC3TAK(y3, 2) < 0 ) ) return 1;


        if ( ( VEC3TAK(x1, 0) > 0 )
                && ( VEC3TAK(x2, 0) > 0 )
                && ( VEC3TAK(x3, 0) > 0 )
                && ( VEC3TAK(y1, 0) > 0 )
                && ( VEC3TAK(y2, 0) > 0 )
                && ( VEC3TAK(y3, 0) > 0 ) ) return 1;


        if ( ( VEC3TAK(x1, 1) > 0 )
                && ( VEC3TAK(x2, 1) > 0 )
                && ( VEC3TAK(x3, 1) > 0 )
                && ( VEC3TAK(y1, 1) > 0 )
                && ( VEC3TAK(y2, 1) > 0 )
                && ( VEC3TAK(y3, 1) > 0 ) ) return 1;


        if ( ( VEC3TAK(x1, 2) > 0 )
                && ( VEC3TAK(x2, 2) > 0 )
                && ( VEC3TAK(x3, 2) > 0 )
                && ( VEC3TAK(y1, 2) > 0 )
                && ( VEC3TAK(y2, 2) > 0 )
                && ( VEC3TAK(y3, 2) > 0 ) ) return 1;


        return 0;
    }


    int GEOMET_3DLineCrossVector3D(Vector3D* a1, Vector3D* a2, Vector3D* b1, Vector3D* b2, Vector3D* x)
    {
        Vector3D a, b, c;
        float d;

        Vector3D_Minus(a2, a1, &a);
        Vector3D_Minus(b2, b1, &b);
        Vector3D_Minus(b1, a1, &c);
        d = Vector3D_Det3x3(&a, &b, &c);

        /* no intersection */
        if ( fabs(d)>1e-3 ) return 0;

        d = Vector3D_Det2x2(&a, &b);
        d = (VEC3TAK(b, 1)*VEC3TAK(c, 0) - VEC3TAK(b, 0)*VEC3TAK(c, 1))/d;

        VEC3GET(x, 0) = VEC3GET(a1, 0) + d*VEC3TAK(a, 0);
        VEC3GET(x, 1) = VEC3GET(a1, 1) + d*VEC3TAK(a, 1);
        VEC3GET(x, 2) = VEC3GET(a1, 2) + d*VEC3TAK(a, 2);

        return 1;
    }

    int GEOMET_3DLineCrossVList(VList* vlist, Edge* e1, Edge* e2, Vector3D* x)
    {
        /* FIXME: first check whether e1 and e2 are faraway */
        Vector3D* a1 = Edge_3DGet(vlist, e1, 0);
        Vector3D* a2 = Edge_3DGet(vlist, e1, 1);

        Vector3D* b1 = Edge_3DGet(vlist, e2, 0);
        Vector3D* b2 = Edge_3DGet(vlist, e2, 1);

        GEOMET_3DLineCrossVector3D(a1, a2, b1, b2, x);

        return 1;
    }


    /*
    int GEOMET_3DEdgeTriangleCross(VList* vlist, Edge* e, Face* f, Vector3D* x)
    {
        Vector3D* v1 = Edge_3DGet(vlist, e, 0);
        Vector3D* v2 = Edge_3DGet(vlist, e, 1);
        Vector3D t;

        int info = GEOMET_3DLinePlaneCross(vlist, f, v1, v2, &t, x);
        if (info == 0) return 0;

        if ( VEC3TAK(t, 1) >= 0.0
                && VEC3TAK(t, 2) >= 0.0
                && VEC3TAK(t, 1)+VEC3TAK(t, 2) <= 1.0
                && VEC3TAK(t, 0) >= 0.0
                && VEC3TAK(t, 1) <= 1.0 )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }*/



    /* used in displaying wire-frame 3D objects */
    /*
    int GEOMET_3DEdgeBeforeTriangle(VList* vlist, Edge* e, Face* f, Vector3D* origin)
    {
        Vector3D t1, t2, x;

        if ( Edge_IsTriangleEdge(e, f) ) return 0;

        Vector3D* ev1 = Edge_3DGet(vlist, e, 0);
        Vector3D* ev2 = Edge_3DGet(vlist, e, 1);

        GEOMET_3DLinePlaneCross(vlist, f, origin, ev1, &t1, &x);
        GEOMET_3DLinePlaneCross(vlist, f, origin, ev2, &t2, &x);

        if ( ( VEC3TAK(t1, 0) >= 1.0 ) && ( VEC3TAK(t2, 0) >= 1.0 ) )
        {
            return 1;
        }
        else if ( ( VEC3TAK(t1, 0) < 1.0 ) && ( VEC3TAK(t2, 0) < 1.0 ) )
        {
            return -1;
        }
        else
        {
            return 2;
        }
    }
    */



#if 0

    float rayTriangleIntersect(const Vector3 &rayOrg, const Vector3 &rayDelta, const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, float minT)
    {
        const float kNoIntersection = 1e30f;

        Vector3 e1 = p1 – p0;
        Vector3 e2 = p2 – p1;

        Vector3 n = crossProduct(e1, e2);
// Compute gradient, which tells us how steep of an angle
// we are approaching the *front* side of the triangle
        float dot = n * rayDelta;
// Check for a ray that is parallel to the triangle or not
// pointing toward the front face of the triangle.
//
// Note that this also will reject degenerate triangles and
// rays as well. We code this in a very particular
// way so that NANs will bail here. (This does not
// behave the same as "dot >= 0.0f" when NANs are involved.)
        if (!(dot < 0.0f))
        {
            return kNoIntersection;
        }
// Compute d value for the plane equation. We will
// use the plane equation with d on the right side:


//
// Ax + By + Cz = d
        float d = n * p0;


// Compute parametric point of intersection with the plane
// containing the triangle, checking at the earliest
// possible stages for trivial rejection
        float t = d – n * rayOrg;
// Is ray origin on the backside of the polygon? Again,
// we phrase the check so that NANs will bail
        if (!(t <= 0.0f))
        {
            return kNoIntersection;
        }


// Closer intersection already found? (Or does
// ray not reach the plane?)
//
// since dot < 0:
//
// t/dot > minT
//
// is the same as
//
// t < dot*minT
//
// (And then we invert it for NAN checking...)

        if (!(t >= dot*minT))
        {
            return kNoIntersection;
        }

// OK, ray intersects the plane. Compute actual parametric
// point of intersection
        t /= dot;
        assert(t >= 0.0f);
        assert(t <= minT);
// Compute 3D point of intersection
        Vector3 p = rayOrg + rayDelta*t;
// Find dominant axis to select which plane
// to project onto, and compute u's and v's

        float u0, u1, u2;
        float v0, v1, v2;
        if (fabs(n.x) > fabs(n.y))
        {
            if (fabs(n.x) > fabs(n.z))
            {
                u0 = p.y – p0.y;
                u1 = p1.y – p0.y;
                u2 = p2.y – p0.y;
                v0 = p.z – p0.z;
                v1 = p1.z – p0.z;
                v2 = p2.z – p0.z;
            }
            else
            {
                u0 = p.x – p0.x;
                u1 = p1.x – p0.x;
                u2 = p2.x – p0.x;
                v0 = p.y – p0.y;
                v1 = p1.y – p0.y;
                v2 = p2.y – p0.y;
            }
        }
        else
        {
            if (fabs(n.y) > fabs(n.z))
            {
                u0 = p.x – p0.x;
                u1 = p1.x – p0.x;
                u2 = p2.x – p0.x;
                v0 = p.z – p0.z;
                v1 = p1.z – p0.z;
                v2 = p2.z – p0.z;
            }
            else
            {
                u0 = p.x – p0.x;
                u1 = p1.x – p0.x;
                u2 = p2.x – p0.x;
                v0 = p.y – p0.y;
                v1 = p1.y – p0.y;
                v2 = p2.y – p0.y;
            }
        }


// Compute denominator, check for invalid
        float temp = u1 * v2 – v1 * u2;
        if (!(temp != 0.0f))
        {
            return kNoIntersection;
        }
        temp = 1.0f / temp;

// Compute barycentric coords, checking for out-of-range
// at each step
        float alpha = (u0 * v2 – v0 * u2) * temp;
        if (!(alpha >= 0.0f))
        {
            return kNoIntersection;
        }
        float beta = (u1 * v0 – v1 * u0) * temp;
        if (!(beta >= 0.0f))
        {
            return kNoIntersection;
        }
        float gamma = 1.0f - alpha - beta;
        if (!(gamma >= 0.0f))
        {
            return kNoIntersection;
        }

        return t;
    }
#endif




    int GEOMET_RayPlane(VList* vlist, Face* f, Vector3D* origin, Vector3D* direction, Vector3D* t)
    {
        Vector3D b;
        int info;

        Vector3D* fv1 = Face_3DGet(vlist, f, 0);
        Vector3D* p1  = &f->bary[0];
        Vector3D* p2  = &f->bary[1];

        //fv1 + t1*p1 + t2*p2 = origin+t*direction
        Vector3D_Minus(fv1, origin, &b);

        //solve t
        info = Vector3D_Solve3x3(direction, p1, p2, &b, t);

        if (info == 0) return 0;

        VEC3GET(t, 1) = -VEC3GET(t, 1);
        VEC3GET(t, 2) = -VEC3GET(t, 2);

        return 1;
    }


    int GEOMET_RayFaceBoundingBox(Vector3D* direction, Face* f)
    {
        static Vector3D tmin, tmax;

        if ( 1.0f/VEC3GET(direction, 0) >= 0.0f)
        {
            VEC3TAK(tmin, 0) = VEC3TAK(f->bboxshift[0], 0) / VEC3GET(direction, 0);
            VEC3TAK(tmax, 0) = VEC3TAK(f->bboxshift[1], 0) / VEC3GET(direction, 0);
        }
        else
        {
            VEC3TAK(tmin, 0) = VEC3TAK(f->bboxshift[1], 0) / VEC3GET(direction, 0);
            VEC3TAK(tmax, 0) = VEC3TAK(f->bboxshift[0], 0) / VEC3GET(direction, 0);
        }

        if ( 1.0f/VEC3GET(direction, 1) >= 0.0f)
        {
            VEC3TAK(tmin, 1) = VEC3TAK(f->bboxshift[0], 1) / VEC3GET(direction, 1);
            VEC3TAK(tmax, 1) = VEC3TAK(f->bboxshift[1], 1) / VEC3GET(direction, 1);
        }
        else
        {
            VEC3TAK(tmin, 1) = VEC3TAK(f->bboxshift[1], 1) / VEC3GET(direction, 1);
            VEC3TAK(tmax, 1) = VEC3TAK(f->bboxshift[0], 1) / VEC3GET(direction, 1);
        }


        if ( 1.0f/VEC3GET(direction, 2) >= 0.0f)
        {
            VEC3TAK(tmin, 2) = VEC3TAK(f->bboxshift[0], 2) / VEC3GET(direction, 2);
            VEC3TAK(tmax, 2) = VEC3TAK(f->bboxshift[1], 2) / VEC3GET(direction, 2);
        }
        else
        {
            VEC3TAK(tmin, 2) = VEC3TAK(f->bboxshift[1], 2) / VEC3GET(direction, 2);
            VEC3TAK(tmax, 2) = VEC3TAK(f->bboxshift[0], 2) / VEC3GET(direction, 2);
        }


        return Vector3D_HasIntersection(&tmin, &tmax);
    }


    int GEOMET_RayFListBoundingBox(Vector3D* direction, FList* flist)
    {
        static Vector3D tmin, tmax;

        if ( 1.0f/VEC3GET(direction, 0) >= 0.0f)
        {
            VEC3TAK(tmin, 0) = VEC3TAK(flist->bboxshift[0], 0) / VEC3GET(direction, 0);
            VEC3TAK(tmax, 0) = VEC3TAK(flist->bboxshift[1], 0) / VEC3GET(direction, 0);
        }
        else
        {
            VEC3TAK(tmin, 0) = VEC3TAK(flist->bboxshift[1], 0) / VEC3GET(direction, 0);
            VEC3TAK(tmax, 0) = VEC3TAK(flist->bboxshift[0], 0) / VEC3GET(direction, 0);
        }

        if ( 1.0f/VEC3GET(direction, 1) >= 0.0f)
        {
            VEC3TAK(tmin, 1) = VEC3TAK(flist->bboxshift[0], 1) / VEC3GET(direction, 1);
            VEC3TAK(tmax, 1) = VEC3TAK(flist->bboxshift[1], 1) / VEC3GET(direction, 1);
        }
        else
        {
            VEC3TAK(tmin, 1) = VEC3TAK(flist->bboxshift[1], 1) / VEC3GET(direction, 1);
            VEC3TAK(tmax, 1) = VEC3TAK(flist->bboxshift[0], 1) / VEC3GET(direction, 1);
        }


        if ( 1.0f/VEC3GET(direction, 2) >= 0.0f)
        {
            VEC3TAK(tmin, 2) = VEC3TAK(flist->bboxshift[0], 2) / VEC3GET(direction, 2);
            VEC3TAK(tmax, 2) = VEC3TAK(flist->bboxshift[1], 2) / VEC3GET(direction, 2);
        }
        else
        {
            VEC3TAK(tmin, 2) = VEC3TAK(flist->bboxshift[1], 2) / VEC3GET(direction, 2);
            VEC3TAK(tmax, 2) = VEC3TAK(flist->bboxshift[0], 2) / VEC3GET(direction, 2);
        }


        return Vector3D_HasIntersection(&tmin, &tmax);
    }




    int GEOMET_RayTriangle(VList* vlist, Face* f, Vector3D* origin, Vector3D* ray, Vector3D* t)
    {
        int info;

        info = GEOMET_RayFaceBoundingBox(ray, f);
        if (info == 0) return 0;

        info = GEOMET_RayPlane(vlist, f, origin, ray, t);
        if (info == 0) return 0;

        if ( VEC3GET(t, 0) > 0.0 && VEC3GET(t, 1) >= 0.0 && VEC3GET(t, 2) >= 0.0 && VEC3GET(t, 1)+VEC3GET(t, 2) <= 1.0 )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }




    int GEOMET_RaySphere(Vector3D* RayOrigin, Vector3D* RayDirection, Vector3D* SphereOrigin, float r, float* t)
    {
        double a, b, c, d;
        Vector3D EC;

        Vector3D_Minus(RayOrigin, SphereOrigin, &EC);

        a = 2.0*Vector3D_NormSquare(RayDirection);
        b = 2.0*Vector3D_InnerProduct(RayDirection, &EC);
        c = Vector3D_NormSquare(&EC) - r*r;

        d = b*b - 2.0*a*c;
        if( d < 0.0 )
        {
            return 0;
        }

        a = 1.0 / a;
        d = sqrt(d);

        *t = (-d-b)*a;
        if( *t >= 0.0 ) return 1;

        *t = (+d-b)*a;
        if( *t >= 0.0 ) return 1;

        return 0;
    }


#ifdef __cplusplus
}
#endif

