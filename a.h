/*
Dumb macros
*/

#include <stddef.h>
#include <string.h>

typedef char Ct;
typedef char* STR;
typedef int It;
typedef size_t SZt;
typedef void Vt;
typedef __uint64_t U64t;
typedef __uint32_t U32t;
typedef __int64_t I64t;
typedef __int32_t I32t;


#define R(x) return x
#define ASSERTFALSE(a,msg) if(a){printf(msg);exit(1);}
#define C const
#define DO(x) while(x)
#define MEMCL(x,l) memset(x,0,l)
#define SIZE(x) sizeof(x)
#define ITR(i,x) for(size_t i=0;i<x;i++)
#define ITRFROM(i,s,x) for (size_t i=s;i<x;i++)