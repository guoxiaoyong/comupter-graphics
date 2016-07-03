/**
 * @file objlib.h
 * @author Guo Xiaoyong
 */

#ifndef __OBJLIB_H__
#define __OBJLIB_H__


#ifdef __cplusplus
extern "C" {
#endif
#include <data3d.h>

    int OBJ_count(char* objstring, char ot);
    VList* OBJ_VList(char* objstring);
    FList* OBJ_FList(char* objstring);

#ifdef __cplusplus
}
#endif


#endif
