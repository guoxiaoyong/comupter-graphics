/**
 * @file memstr.c
 * @author Guo Xiaoyong
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include <memstr.h>



    /**
     * Get the size of the file
     **/
    int GetFileSize(char* FileName)
    {
        int FileSize;
        FILE* fp;

        fp = fopen(FileName, "r");

        if (fp == NULL)
        {
            fprintf(stderr, "GetFileSize: cannot open %s\n", FileName);
            return(-1);
        }

        fseek(fp, 0, SEEK_END);
        FileSize = (int)ftell(fp);
        fclose(fp);

        return FileSize;
    }


    char* LoadIntoMemory(char* FileName, int* outFileSize)
    {
        int FileSize;
        int FileSizeX;
        FILE* fp;
        char* FileAsString;
        int info;

        FileSize     =  GetFileSize(FileName);
        FileSizeX    =  FileSize + 256;
        FileAsString =  (char*)malloc(FileSizeX);

        if (FileAsString == NULL)
        {
            fprintf(stderr, "LoadIntoMemory: cannot allocation %d bytes of continuous memory!\n", FileSizeX);
            return NULL;
        }

        fp = fopen(FileName, "rb");
        info = fread(FileAsString, 1, FileSize, fp);
        assert(info == FileSize);
        FileAsString[FileSize] = '\0';
        fclose(fp);

        if (outFileSize != NULL)
        {
            *outFileSize = FileSize;
        }

        return FileAsString;
    }


    char* memstr_getline(char* str, char* buf)
    {
        static const char eol[] = "\r\n";

        static char* memstr  = NULL;
        static char* buffer  = NULL;
        static char* currpos = NULL;
        static char* nextpos;

        size_t siz;

        if (str != NULL)
        {
            memstr  = str;
            currpos = memstr;
        }


        if (buf != NULL)
        {
            buffer = buf;
        }

        assert(memstr  != NULL);
        assert(currpos != NULL);
        assert(buffer  != NULL);

        nextpos = strpbrk(currpos, eol);
        if (nextpos == NULL)
        {
            return NULL;
        }

        siz = (int)(nextpos-currpos);
        memcpy(buffer, currpos, siz);
        buffer[siz] = '\0';

        currpos = nextpos+1;
        while (currpos[0] == '\r' || currpos[0] == '\n')
        {
            currpos++;
        }

        return buffer;
    }




#ifdef __cplusplus
}
#endif

