/* FILE NAME  : winmsg.cpp
 * PROGRAMMER : AU3
 * LAST UPDATE: 17.07.2022
 * PURPOSE    : WinAPI class.
 *              Window messages functions handlers
 */

#include "Win.h"

/* Refresh timer constant */
#define REFRESH_TIMER  0x30102

/* WM_CREATE window message handle function.
 * ARGUMENTS:
 *   - structure with creation data:
 *       CREATESTRUCT *CS;
 * RETURNS:
 *   (BOOL) TRUE to continue creation window, FALSE to terminate.
 */
BOOL Win::onCreate( CREATESTRUCT *CS )
{
  SetTimer(hWnd, INIT_TIMER, 1, nullptr);
  return TRUE;
} /* End of 'Win::onCreate' function */

/* WM_DESTROY window message handle function.
 * ARGUMENTS: None.
 * RETURNS: None.
 */
VOID Win::onDestroy( VOID )
{
  if (isInit)
  {
    close();
    KillTimer(hWnd, REFRESH_TIMER);
  }
  else
    KillTimer(hWnd, INIT_TIMER);
  PostQuitMessage(30);
} /* End of 'Win::onDestroy' function */


/* WM_SIZE window message handle function.
 * ARGUMENTS:
 *   - sizing flag (see SIZE_***, like SIZE_MAXIMIZED)
 *       UINT State;
 *   - new width and height of client area:
 *       INT W, H;
 * RETURNS: None.
 */
VOID Win::onSize( UINT State, INT W, INT H )
{
  this->w = W;
  this->h = H;
  if (isInit)
    resize();
} /* End of 'Win::onSize' function */

/* WM_ERASEBKGND window message handle function.
 * ARGUMENTS:
 *   - device context of client area:
 *       HDC hDC;
 * RETURNS:
 *   (BOOL) TRUE if background is erased, FALSE otherwise.
 */
BOOL Win::onEraseBkgnd( HDC hDC )
{
  if (isInit)
    erase(hDC);
  return TRUE;
} /* End of 'Win::onEraseBkgnd' function */

/* WM_PAINT window message handle function.
 * ARGUMENTS:
 *   - window device context:
 *       HDC hDC;
 *   - paint message structure pointer:
 *       PAINTSTRUCT *PS;
 * RETURNS: None.
 */
VOID Win::onPaint( HDC hDC, PAINTSTRUCT *PS )
{
  if (isInit)
    paint(hDC);
} /* End of 'Win::onPaint' function */
/* WM_ACTIVATE window message handle function.
 * ARGUMENTS:
 *   - reason (WA_CLICKACTIVE, WA_ACTIVE or WA_INACTIVE):
 *       UINT Reason;
 *   - handle of active window:
 *       HWND hWndActDeact;
 *   - minimized flag:
 *       BOOL IsMinimized;
 * RETURNS: None.
 */
VOID Win::onActivate( UINT Reason, HWND hWndActDeact, BOOL IsMinimized )
{
  isActive = Reason == WA_CLICKACTIVE || Reason == WA_ACTIVE;
  if (isInit)
    activate();
} /* End of 'Win::onActivate' function */

/* WM_TIMER window message handle function.
 * ARGUMENTS:
 *   - specified the timer identifier.
 *       INT Id;
 * RETURNS: None.
 */
VOID Win::onTimer( INT Id )
{
  if (!isInit)
  {
    KillTimer(hWnd, INIT_TIMER);
    SetTimer(hWnd, REFRESH_TIMER, 1, nullptr);
    isInit = TRUE;
    // call callbacks
    init();
    resize();
  }
  timer();
} /* End of 'Win::onTimer' function */

/* WM_MOUSEWHEEL window message handle function.
 * ARGUMENTS:
 *   - mouse window position:
 *       INT X, Y;
 *   - mouse wheel relative delta value:
 *       INT Z;
 *   - mouse keys bits (see MK_*** bits constants):
 *       UINT Keys;
 * RETURNS: None.
 */
VOID Win::onMouseWheel( INT X, INT Y, INT Z, UINT Keys )
{
  mouseWheel += Z;
} /* End of 'Win::onMouseWheel' function */

/* WM_GETMINMAXINFO window message handle function.
 * ARGUMENTS:
 *   - struct to get info:
 *       LPMINMAXINFO MinMax;
 * RETURNS: None.
 */
VOID Win::onGetMinMaxInfo( LPMINMAXINFO MinMax )
{
  MinMax->ptMaxTrackSize.y = GetSystemMetrics(SM_CYMAXTRACK) +
    GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER) * 2;
} /* End of 'Win::onGetMinMaxInfo' function */

/* WM_*BUTTonDOWN window message handle function
 * ARGUMENTS:
 *   - is click double flag:
 *       BOOL IsDouble;
 *   - mouse coordinates:
 *       INT X, Y;
 *   - button flag:
 *       UINT Key;
 * RETURNS: None.
 */
VOID Win::onButtonDown( BOOL IsDouble, INT X, INT Y, UINT Key )
{
  switch (Key)
  {
  case BUTTON_LEFT:
    if (IsDouble)
    {
    }
    else
    {
      SetFocus(hWnd);
    }
    break;
  case BUTTON_RIGHT:
    if (IsDouble)
    {
    }
    else
    {
    }
    break;
  case BUTTON_MIDDLE:
    if (IsDouble)
    {
    }
    else
    {
    }
    break;
  }
} /* End of 'Win::onButtonDown' function */

/* WM_*BUTTonUP window message handle function
 * ARGUMENTS:
 *   - mouse coordinates:
 *       INT X, Y;
 *   - button flag:
 *       UINT Key;
 * RETURNS: None.
 */
VOID Win::onButtonUp( INT X, INT Y, UINT Key )
{
  switch (Key)
  {
  case BUTTON_LEFT:
    break;
  case BUTTON_RIGHT:
    break;
  case BUTTON_MIDDLE:
    break;
  }
} /* End of 'Win::onButtonDown' function */

/* WM_CLOSE window message handle function.
 * ARGUMENTS: None.
 * RETURNS: None.
 */
VOID Win::onClose( VOID )
{
  DestroyWindow(hWnd);
} /* End of 'Win::onClose' function */

/* WM_KEYDOWN window message handle function.
 * ARGUMENTS:
 *   - key of pressed button:
 *       UINT Key;
 * RETURNS: None.
 */
VOID Win::onKeyDown( UINT Key )
{
  if (Key == VK_F11)
    fullScreen();
  if (Key == VK_ESCAPE)
    DestroyWindow(hWnd);
  keyboard(false, Key);
} /* End of 'Win::onKeyDown' function */

/* WM_KEYUP window message handle function.
 * ARGUMENTS:
 *   - key of released button:
 *       UINT Key;
 * RETURNS: None.
 */
VOID Win::onKeyUp( UINT Key )
{
  keyboard(true, Key);
} /* End of 'Win::onKeyDown' function */

/* END OF 'Win.cpp' FILE */