/**
 * @file memstr.h
 * @author Guo Xiaoyong
 */


#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Get the size of the file
     **/
    int GetFileSize(char* FileName);
    char* LoadIntoMemory(char* FileName, int* outFileSize);
    char* memstr_getline(char* str, char* buf);

#ifdef __cplusplus
}
#endif

