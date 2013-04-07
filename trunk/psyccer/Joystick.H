#ifndef Joystick_H
#define Joystick_H

#pragma warning (disable : 4244)

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <dinput.h>

#define MIN     -1000
#define MAX     1000

//////////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////////
extern "C"
{
    // DirectX Errors
    extern void DDError(HRESULT hErr, void* hWnd);
    extern void DSError(HRESULT hErr, void* hWnd);
}

//////////////////////////////////////////////////////////////////////////////////
// Joystick Class
//////////////////////////////////////////////////////////////////////////////////

class Joystick
{
public:
    Joystick(void);
    ~Joystick(void);

    BOOL Create(void *hInst, void *hWnd);
    void ReAcquire(void);
    void UnAcquire(void);
    void Update(void);

    void SetActiveDevices(BOOL bJoystick);
    void SetJoystickAbs(void);
    void RunJoystickControlPanel(void *hWnd = NULL);

public:
    LPDIRECTINPUT m_lpDI;
    LPDIRECTINPUTDEVICE2 m_lpDIDJoystick;

    POINT JoystickPos;
    BOOL JoystickB1, JoystickB2, JoystickB3, JoystickB4;

    BOOL m_bJoystick;
};

//////////////////////////////////////////////////////////////////////////////////
// MACROS
//////////////////////////////////////////////////////////////////////////////////
#define RELEASE(x) if(x != NULL) { x->Release(); x = NULL; }
#define ODS(a) OutputDebugString(a)

#endif
