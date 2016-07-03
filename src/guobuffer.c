
#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include "guobuffer.h"

    int guo_BigSmall()
    {
        unsigned int num = 0x00000001;
        unsigned char* ptr;

        ptr = (unsigned char*)&num;
        if ( ptr[0] == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    void guo_ReverseBytes(char* mem, int n)
    {
        int i;
        char tmp;

        for (i = 0; i < n/2; ++i)
        {
            tmp        = mem[i];
            mem[i]     = mem[n-1-i];
            mem[n-1-i] = tmp;
        }
    }

    unsigned short guo_BigUint16(unsigned short num)
    {
        if (guo_BigSmall() == 1)
        {
            guo_ReverseBytes((char*)&num, sizeof(num));
        }
        return num;
    }

    unsigned char guo_BitOnes(unsigned char num)
    {
        unsigned char sum;
        unsigned char i;

        sum = 0u;
        for (i = 0; i < num; ++i)
        {
            sum = (unsigned char)( sum ^ (1u < i) );
        }

        return sum;
    }

    unsigned char guo_GetBit(unsigned char num, unsigned char n)
    {
        unsigned int mask = 0x0001;
        assert(n <= 7);

        return (unsigned char)(( num & (mask << n) ) >> n);
    }

    unsigned char guo_SetBit(unsigned char* num, unsigned char n, unsigned char bitnum)
    {
        assert(bitnum == 0 || bitnum == 1);
    
        if (bitnum == 0)
        {
            return *num = ( (*num) & ((1 << n) ^ 0xFF) );
        }
        else // bitnum == 1
        {
            return *num = ( (*num) | (1 << n) );
        }
    }



    size_t guo_GetSizeFromFileName(const char* FileName)
    {
        size_t FileSize;
        FILE* fp;

        fp = fopen(FileName, "rb");
        if (fp == NULL)
        {
            fprintf(stderr, "guo_GetFileSize: cannot open the file %s\n", FileName);
            return(-1);
        }

        fseek(fp, 0, SEEK_END);
        FileSize = ftell(fp);
        fclose(fp);

        return FileSize;
    }


    size_t guo_GetSizeFromFilePoint(FILE* fp)
    {
        size_t FileSize;
        long int pos;

        if (fp == NULL)
        {
            fprintf(stderr, "guo_GetSizeFromFilePoint error: fp == NULL");
            return(-1);
        }

        pos = ftell(fp);
        fseek(fp, 0, SEEK_END);
        FileSize = ftell(fp);
        fseek(fp, pos, SEEK_SET);

        return FileSize;
    }


    guobuffer* guobuffer_Create(size_t n)
    {
        guobuffer* buf;
        buf = (guobuffer*)malloc( sizeof(guobuffer) );

        if (buf == NULL)
        {
            return NULL;
        }
        else
        {
            buf->mem    = (unsigned char*)malloc(n);
            buf->size   = n;
            buf->used   = 0;
            buf->ptr    = buf->mem;
            buf->bitpos = 0;
            return buf;
        }
    }


    guobuffer* guobuffer_CreateFromFile(const char* filename)
    {
        guobuffer* buf = NULL;
        size_t size;

        size = guo_GetSizeFromFileName(filename);
        buf  = guobuffer_Create(size);

        if (buf) guobuffer_ReadFile(buf, filename);

        return buf;
    }


    void guobuffer_Destroy(guobuffer* buf)
    {
        free(buf->mem);
        free(buf);
        buf = NULL;
    }

    void guobuffer_WriteUint8(guobuffer* buf, const unsigned char num)
    {
        memcpy(buf->ptr, &num, sizeof(unsigned char));
        buf->ptr  += sizeof(unsigned char);
        buf->used += sizeof(unsigned char);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }

    void guobuffer_WriteSint8(guobuffer* buf, const signed char num)
    {
        memcpy(buf->ptr, &num, sizeof(signed char));
        buf->ptr  += sizeof(signed char);
        buf->used += sizeof(signed char);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }

    void guobuffer_WriteUint16(guobuffer* buf, const unsigned short num)
    {
        memcpy(buf->ptr, &num, sizeof(unsigned short));
        buf->ptr  += sizeof(unsigned short);
        buf->used += sizeof(unsigned short);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }

    void guobuffer_WriteSint16(guobuffer* buf, const signed short num)
    {
        memcpy(buf->ptr, &num, sizeof(signed short));
        buf->ptr  += sizeof(signed short);
        buf->used += sizeof(signed short);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }

    void guobuffer_WriteUint32(guobuffer* buf, const unsigned int num)
    {
        memcpy(buf->ptr, &num, sizeof(unsigned int));
        buf->ptr  += sizeof(unsigned int);
        buf->used += sizeof(unsigned int);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }

    void guobuffer_WriteSint32(guobuffer* buf, const signed int num)
    {
        memcpy(buf->ptr, &num, sizeof(signed int));
        buf->ptr  += sizeof(signed int);
        buf->used += sizeof(signed int);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }


    void guobuffer_WriteChars(guobuffer* buf, const char* num, const size_t n)
    {
        memcpy(buf->ptr, num, n);
        buf->ptr  += (sizeof(char)*n);
        buf->used += (sizeof(char)*n);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }


    void guobuffer_ReadChars(guobuffer* buf, char* num, const size_t n)
    {
        memcpy(num, buf->ptr, n);
        buf->ptr  += (sizeof(char)*n);
        buf->used += (sizeof(char)*n);

        assert(buf->bitpos == 0);
        assert(buf->used <= buf->size);
    }

    void guobuffer_SetPtr();


    void guobuffer_SetBitsMode(guobuffer* buf, int mode)
    {
        if (mode == 0)
        {
            buf->bitmode = 0;
            strcpy(buf->bitmode_str, "MSB2LSB");
        }
        else if (mode == 1)
        {
            buf->bitmode = 1;
            strcpy(buf->bitmode_str, "LSB2MSB");
        }
        else
        {
            fprintf(stderr, "guobuffer_SetBitsMode: error!\n");
        }
    }

    void guobuffer_SetBits(guobuffer* buf, const unsigned char bitnum)
    {
        assert(bitnum == 0 || bitnum == 1);

        if (buf->bitmode == 1)
        {
            guo_SetBit(buf->ptr, bitnum, buf->bitpos);
        }
        else
        {
            guo_SetBit(buf->ptr, bitnum, 7u-buf->bitpos);
        }

        buf->bitpos++;

        if (buf->bitpos == 8)
        {
            buf->bitpos = 0;
            buf->ptr++;
        }
    }

    void guobuffer_SetBitStream(guobuffer* buf, const unsigned char* bits, const unsigned char n)
    {
        assert(n > 0);
        unsigned int i;
        for (i = 0; i < n; ++i)
        {
            guobuffer_SetBits(buf, bits[i]);
        }
    }



    int guobuffer_WriteFile(guobuffer* buf, const char* filename)
    {
        FILE* fp;
        size_t len;

        fp = fopen(filename, "wb");
        if (fp == NULL)
        {
            fprintf(stderr, "guobuffer_WriteFile: failed to open %s.\n", filename);
            return -1;
        }
        len = fwrite(buf->mem, 1, buf->used, fp);
        assert(len == buf->used);
        fclose(fp);

        return 0;
    }


    int guobuffer_ReadFile(guobuffer* buf, const char* filename)
    {
        FILE* fp;
        size_t len, info;


        fp = fopen(filename, "rb");
        if (fp == NULL)
        {
            fprintf(stderr, "guobuffer_ReadFile: failed to open %s.\n", filename);
            return -1;
        }

        len  = guo_GetSizeFromFilePoint(fp);
        rewind(fp);
        info = fread((void*)buf->ptr, 1u, len, fp);

        buf->ptr  += len;
        buf->used += len;

        if (len != info)
        {
            fprintf(stderr, "reading error! len = %u, info = %u\n", len, info);
        }
        assert(len == info);
        fclose(fp);

        return 0;
    }


#ifdef __cplusplus
}
#endif

