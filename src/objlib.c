/**
 * @file objlib.c
 * @author Guo Xiaoyong
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <camera.h>
#include <meshmo.h>
#include <memstr.h>


    int OBJ_count(char* objstring, char ot)
    {
        static char buffer[1024];
        int numV = 0;
        char* info;

        info = memstr_getline(objstring, buffer);
        while (info != NULL)
        {
            if ( buffer[0]  == ot )
            {
                numV++;
            }

            info = memstr_getline(NULL, NULL);
        }

        return numV;
    }


    VList* OBJ_VList(char* objstring)
    {
        static char buffer[1024];
        int vlen, i;
        VList* vlist;
        Vector3D v;

        vlen  = OBJ_count(objstring, 'v');
        vlist = VList_Create(vlen);
        memstr_getline(objstring, buffer);

        i = 0;
        while (i < vlen)
        {
            if (buffer[0] == 'v')
            {
                sscanf(buffer, "v %f %f %f", v.data, v.data+1, v.data+2);
                VList_Add(vlist, &v);
                i++;
            }

            memstr_getline(NULL, NULL);
        }

        return vlist;
    }


    FList* OBJ_FList(char* objstring)
    {
        static char buffer[1024];
        int flen;
        int i, k;
        FList* flist;
        Face f;

        flen  = OBJ_count(objstring, 'f');
        flist = FList_Create(flen);

        memstr_getline(objstring, buffer);

        i = 0;
        while (i < flen)
        {
            memset(&f, 0, sizeof(f));
            if (buffer[0] == 'f')
            {
                sscanf(buffer, "f %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                       f.v+0, f.v+1, f.v+2, f.v+3, f.v+4, f.v+5, f.v+6, f.v+7,
                       f.v+8, f.v+9, f.v+10, f.v+11, f.v+12, f.v+13, f.v+14, f.v+15);


                for (k = 0; k < MAX_VNUM; k++)
                {
                    f.v[k]--;
                    if (f.v[k] == -1)
                    {
                        f.len = k;
                        break;
                    }
                }

                FList_Add(flist, &f);
                i++;
            }

            memstr_getline(NULL, NULL);
        }

        return flist;
    }




#ifdef __cplusplus
}
#endif

