#ifndef _GLOBTYPES_H
#define _GLOBTYPES_H           // Show that this file has been included

//********************************************
//  Platform-independent Data Definitions
//********************************************
typedef unsigned char       BOOLEAN;    // BOOLEAN value (FALSE or TRUE)

typedef unsigned char       U8;			// Unsigned 8-bit value
typedef unsigned short      U16;		//    "     16-bit  "
typedef unsigned long       U32;		//    "     32-bit  "
typedef unsigned long long  U64;		//    "     64-bit  "

typedef signed char         I8;			// Signed 8-bit value
typedef signed short        I16;		//   "    16-bit  "
typedef signed long         I32;		//   "    32-bit  "
typedef signed long long    I64;		//   "    64-bit  "

typedef float               FLT32;		// 32-bit floating point value
typedef double              DBL32;		// 32-bit double value
typedef long double         DBL64;		// 64-bit double value

typedef U8*                PU8;		// Pointer to unsigned 8-bit value
typedef U16*               PU16;		//    "    "     "     16-bit  "
typedef U32*               PU32;		//    "    "     "     32-bit  "

typedef char*               PSZ;		// Pointer to NULL-terminated string
typedef unsigned short const int* PSZ16;      // Pointer to NULL-terminated 16-bit string
#ifndef WIN32
typedef void                VOID;
#endif
typedef void*                PVOID;

typedef U16 LCD_COLOR;

//***********************
//    Useful Constants
//***********************
#ifndef FALSE
#define FALSE               0           // Logical FALSE
#endif

#ifndef TRUE
#define TRUE                1           //    "    TRUE
#endif

#define NEVER               0           // Condition that always evaluates to FALSE
#define FOREVER             1           //     "       "    "        "     "  TRUE

#ifndef NULL
#define NULL				0

#endif




void MarturionFatalErrorHandler(PSZ filename, U32 lineNumber, PSZ description);

#define ASSERT(isTrue)  if (isTrue) {} else MarturionFatalErrorHandler(__FILE__, __LINE__, "Assert condition failed")



#endif // _GLOBTYPES_H

