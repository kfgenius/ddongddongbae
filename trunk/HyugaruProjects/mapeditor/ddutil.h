#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

extern IDirectDrawSurface* _LoadImage(IDirectDraw *pdd, LPCTSTR strFilename, BOOL flag);
extern IDirectDrawSurface* DDLoadBitmap(IDirectDraw *pdd, LPCTSTR szBitmap, int dx, int dy, BOOL flag);
extern HRESULT DDCopyBitmap(IDirectDrawSurface *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);
extern HRESULT DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);

#ifdef __cplusplus
}
#endif

extern int BmpXsize, BmpYsize;