#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <camera.h>
#include <meshmo.h>
#include <memstr.h>
#include <objlib.h>
#include <geomet.h>

#include <head.c>
#include <xwing.c>
#include <bunny.c>


int main(void)
{
    int i;
    char* objstring;

    Vector3D* p1;
    Vector3D* p2;
    Vector3D* p3;

    objstring = objstring_bunny;
    VList* vlist = OBJ_VList(objstring);
    FList* flist = OBJ_FList(objstring);

    for (i = 0; i < flist->len; ++i)
    {
         p1 = Face_3DGet(vlist, &flist->f[i], 0);
         p2 = Face_3DGet(vlist, &flist->f[i], 1);
         p3 = Face_3DGet(vlist, &flist->f[i], 2);
         printf("triangle {\n");
         printf("<%f, %f, %f>,\n",VEC3GET(p1, 0),VEC3GET(p1, 1),VEC3GET(p1, 2));
         printf("<%f, %f, %f>,\n",VEC3GET(p2, 0),VEC3GET(p2, 1),VEC3GET(p2, 2));
         printf("<%f, %f, %f> \n",VEC3GET(p3, 0),VEC3GET(p3, 1),VEC3GET(p3, 2));
         printf("texture { Green }\n");
         printf("}\n");
    }

    VList_Destroy(vlist);
    FList_Destroy(flist);
    return 0;
}



