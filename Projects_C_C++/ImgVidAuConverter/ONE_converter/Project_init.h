#ifndef _PROJECT_INIT_
#define _PROJECT_INIT_
#include <QString>


#ifdef Q_OS_WIN32

//common typedefs
typedef unsigned char          u8c;
typedef char                   s8c;
typedef unsigned short         u16i;
typedef signed   short         s16i;
typedef unsigned  int          u32i;
typedef signed int		       s32i;
typedef signed long long int   s64i;
typedef unsigned long long int u64i;
typedef float				  __32f;
typedef double				  __64f;

#endif
template <typename T>
struct mm_pair {
   T first;
   T second;
};

template <typename T>
struct mm_triple {
    T first;
    T second;
    T third;
};

struct mm_ret {
    QString err;
    int err_code;
};

int get_file_size(std::string filename);

#endif
