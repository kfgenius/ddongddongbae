/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/

#include <tchar.h>

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

extern IDirectDrawSurface7* _LoadPcx( IDirectDraw7 *pdd, LPCTSTR name, BOOL flag );
extern IDirectDrawSurface7* _LoadImage( IDirectDraw7 *pdd, LPCTSTR strFilename, BOOL flag );
extern IDirectDrawSurface7*  DDLoadBitmap(IDirectDraw7 *pdd, LPCTSTR szBitmap, int dx, int dy, BOOL flag );
extern HRESULT               DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD                 DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb);
extern HRESULT               DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

extern int BmpXsize, BmpYsize;
