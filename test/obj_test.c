#include <stdio.h>
#include <math.h>

#include <meshmo.h>
#include <objlib.h>
#include <memstr.h>


int main(void)
{
    char line[1024];
    int fsize;
    char* info;

    char filename[] = "../data/cam.obj";

    char* objstring = LoadIntoMemory(filename, &fsize);

    info = memstr_getline(objstring, line);

    while (info != NULL)
    {
        printf("%s\n", line);
        info = memstr_getline(NULL, NULL);
    }


    fsize = OBJ_count(objstring, 'v');
    printf("number of vertices: %d\n", fsize);
    
    VList* vlist = OBJ_VList(objstring);
    VList_Print(vlist);
    VList_Destroy(vlist);



    char str[] = "f 1 2 3 4 6";
    int  n[100];
    sscanf(str, "f %d %d %d %d %d %d", n,n+1,n+2,n+3,n+4,n+5);
    printf("n=[%d,%d,%d,%d,%d,%d]\n", n[0],n[1],n[2],n[3],n[4],n[5]);

    return 0;
}



