#if  defined(_WIN32)||defined(_WIN64)
#include <direct.h>
#else
#include <stdlib.h>
#include <sys/stat.h>
#define _mkdir(name) mkdir( name, 0755 )
#endif
