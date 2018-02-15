

#ifndef REGSHOT_VERSION_H
#define REGSHOT_VERSION_H


#include "version.rc.h"  /* resource related version macros */

#define DO_STRINGIFY(x)   #x
#define STRINGIFY(x)      DO_STRINGIFY(x)

/*
    Define macros for compiler version
*/
#if defined(__GNUC__)
#   define REGSHOT_VERSION_COMPILER   TEXT("GCC ") TEXT(STRINGIFY(__GNUC__)) TEXT(".") TEXT(STRINGIFY(__GNUC_MINOR__)) TEXT(".") TEXT(STRINGIFY(__GNUC_PATCHLEVEL__))
#elif defined(__INTEL_COMPILER)
#   if __INTEL_COMPILER >= 1200
#       define REGSHOT_VERSION_COMPILER TEXT("Intel Compiler 12")
#   else
#       define REGSHOT_VERSION_COMPILER TEXT("Intel Compiler (version unknown)")
#   endif
#elif defined(WDK_BUILD)
#   if _MSC_VER == 1700
#       define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2012 SP0)")
#   elif _MSC_VER == 1600
#       if (_MSC_FULL_VER >= 160040219)
#           define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2010 SP1)")
#       else
#           define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2010 SP0)")
#       endif
#   elif _MSC_VER == 1500
#       if (_MSC_FULL_VER >= 150030729)
#           define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2008 SP1)")
#       else
#           define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2008 SP0)")
#       endif
#   elif _MSC_VER == 1400
#       define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2005)")
#   elif _MSC_VER == 1310
#       define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2003)")
#   elif _MSC_VER == 1300
#       define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 2002)")
#   elif _MSC_VER == 1200
#       define REGSHOT_VERSION_COMPILER TEXT("WDK (MSVC 6.0)")
#   else
#       define REGSHOT_VERSION_COMPILER TEXT("WDK (version unknown)")
#   endif
#elif defined(_MSC_VER)
#   if _MSC_VER == 1700
#       define REGSHOT_VERSION_COMPILER TEXT("MSVC 2012 SP0")
#   elif _MSC_VER == 1600
#       if (_MSC_FULL_VER >= 160040219)
#           define REGSHOT_VERSION_COMPILER TEXT("MSVC 2010 SP1")
#       else
#           define REGSHOT_VERSION_COMPILER TEXT("MSVC 2010 SP0")
#       endif
#   elif _MSC_VER == 1500
#       if (_MSC_FULL_VER >= 150030729)
#           define REGSHOT_VERSION_COMPILER TEXT("MSVC 2008 SP1")
#       else
#           define REGSHOT_VERSION_COMPILER TEXT("MSVC 2008 SP0")
#       endif
#   elif _MSC_VER == 1400
#       define REGSHOT_VERSION_COMPILER TEXT("MSVC 2005")
#   elif _MSC_VER == 1310
#       define REGSHOT_VERSION_COMPILER TEXT("MSVC 2003")
#   elif _MSC_VER == 1300
#       define REGSHOT_VERSION_COMPILER TEXT("MSVC 2002")
#   elif _MSC_VER == 1200
#       define REGSHOT_VERSION_COMPILER TEXT("MSVC 6.0")
#   else
#       define REGSHOT_VERSION_COMPILER TEXT("MSVC (version unknown)")
#   endif
#else
#   define REGSHOT_VERSION_COMPILER TEXT("(Unknown compiler)")
#endif

#endif // REGSHOT_VERSION_H
