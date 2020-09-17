#define USE_MMGR 0
#define MMGR_ASSERT 0
#define USE_FILEWATCHER 1

//MathGeoLib--------------------------------------------------------
#include "ExternalTools/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "ExternalTools/MathGeoLib/lib_x86/lDebug/MathGeoLib.lib")
#else
#pragma comment (lib, "ExternalTools/MathGeoLib/lib_x86/lRelease/MathGeoLib.lib")
#endif
//--------------------------------------------------------------------