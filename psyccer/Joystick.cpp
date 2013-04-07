#include "Joystick.h"

static LPDIRECTINPUTDEVICE lpDID = NULL;

//////////////////////////////////////////////////////////////////////////////////
// EnumJoystick
//
BOOL FAR PASCAL EnumJoystick(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef)
{
    LPDIRECTINPUT pDI = (LPDIRECTINPUT)pvRef;

    if(pDI->CreateDevice(pdinst->guidInstance, &lpDID, NULL) != DI_OK)
        return DIENUM_CONTINUE;

    return DIENUM_STOP;
}

//////////////////////////////////////////////////////////////////////////////////
// Constructor
//
Joystick::Joystick(void)
{
    m_lpDI = NULL;
    m_lpDIDJoystick = NULL;
    m_bJoystick = FALSE;

    JoystickPos.x = 0;
    JoystickPos.y = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Destructor
//
Joystick::~Joystick(void)
{
    RELEASE(m_lpDIDJoystick);
    RELEASE(m_lpDI);
}

//////////////////////////////////////////////////////////////////////////////////
// Joystick Create
//
BOOL Joystick::Create(void *hInst, void *hWnd)
{
    HRESULT rval;

	DirectInputCreate
    rval = DirectInput8Create((HINSTANCE)hInst, DIRECTINPUT_VERSION, &m_lpDI, NULL);
    if(rval != DI_OK) return FALSE;

    // Enumerate the joystick device
    rval = m_lpDI->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoystick, m_lpDI, DIEDFL_ATTACHEDONLY);
    if(lpDID != NULL)
    {
        rval = lpDID->QueryInterface(IID_IDirectInputDevice2, (LPVOID *)&m_lpDIDJoystick);
        if(rval != DI_OK) return FALSE;

        RELEASE(lpDID);

        m_lpDIDJoystick->SetDataFormat(&c_dfDIJoystick);
        m_lpDIDJoystick->SetCooperativeLevel((HWND)hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

        // Set the X-axis range (-1000 to +1000)
        DIPROPRANGE diprg;
        diprg.diph.dwSize = sizeof(diprg);
        diprg.diph.dwHeaderSize = sizeof(diprg.diph);
        diprg.diph.dwObj = DIJOFS_X;
        diprg.diph.dwHow = DIPH_BYOFFSET;
        diprg.lMin = MIN;
        diprg.lMax = MAX;

        if(m_lpDIDJoystick->SetProperty(DIPROP_RANGE, &diprg.diph) != DI_OK)
            return FALSE;

        // And again for Y-axis range
        diprg.diph.dwObj = DIJOFS_Y;

        if(m_lpDIDJoystick->SetProperty(DIPROP_RANGE, &diprg.diph) != DI_OK)
            return FALSE;

        // Set X axis dead zone to 10%
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(dipdw);
        dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
        dipdw.diph.dwObj = DIJOFS_X;
        dipdw.diph.dwHow = DIPH_BYOFFSET;
        dipdw.dwData = 1000;

        if(m_lpDIDJoystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph) != DI_OK)
            return FALSE;

        dipdw.diph.dwObj = DIJOFS_Y;

        // Set Y axis dead zone to 10%
        if(m_lpDIDJoystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph) != DI_OK)
            return FALSE;

        rval = m_lpDIDJoystick->Acquire();
        if(rval != DI_OK) return FALSE;

        m_bJoystick = TRUE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Joystick ReAcquire
//
void Joystick::ReAcquire(void)
{
    m_lpDIDJoystick->Acquire();
}

//////////////////////////////////////////////////////////////////////////////////
// Joystick UnAcquire
//
void Joystick::UnAcquire(void)
{
    m_lpDIDJoystick->Unacquire();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Joystick Update
//
void Joystick::Update(void)
{
    DIJOYSTATE JoyState;

    if(m_bJoystick)
    {
        m_lpDIDJoystick->Poll();

        if(m_lpDIDJoystick->GetDeviceState(sizeof(JoyState), &JoyState) == (DIERR_INPUTLOST | DIERR_NOTACQUIRED))
            m_lpDIDJoystick->Acquire();

        JoystickPos.x = JoyState.lX;
        JoystickPos.y = JoyState.lY;

        JoystickB1 = JoyState.rgbButtons[0];
        JoystickB2 = JoyState.rgbButtons[1];
        JoystickB3 = JoyState.rgbButtons[2];
        JoystickB4 = JoyState.rgbButtons[3];
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Joystick SetActiveDevices
//
void Joystick::SetActiveDevices(BOOL bJoystick)
{
    m_bJoystick = bJoystick;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Joystick SetJoystickRelative
//
void Joystick::SetJoystickAbs(void)
{
    DIPROPDWORD dipdw;

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = DIPROPAXISMODE_ABS;
    m_lpDIDJoystick->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// RunJoystickControlPanel
//
void Joystick::RunJoystickControlPanel(void *hWnd)
{
    m_lpDIDJoystick->RunControlPanel((HWND)hWnd, 0);
}
