#pragma once

#include <winsock2.h>
#include "win.h"

#pragma comment(lib, "ws2_32.lib")

class Client : public Win
{
private:
  HWND hTextBox;
  HWND hLabel;
  HWND hSend;
  SOCKET connection;
  HANDLE msgThread;
  bool isShift;

  static void messageHandler( Client *win );
  static LRESULT editProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
  void sendToServer( void );
  void connectToServer( void );
  void createUIElements( void );

  void init( void ) override;
  void resize( void ) override;
  void paint( HDC hDC ) override;
  void keyboard( bool isUp, UINT key ) override;
  void command( HWND handle, WORD id, WORD cmd ) override;

public:
  Client( const std::string &name ) : Win(name), connection(INVALID_SOCKET), isShift(false),
    hTextBox(nullptr), hSend(nullptr), hLabel(nullptr), msgThread(nullptr)
  {
  }

  ~Client( void ) override
  {
    closesocket(connection);
    WaitForSingleObject(msgThread, INFINITE);

    HFONT hFont = (HFONT)SendMessage(hTextBox, WM_GETFONT, 0, 0);
    DeleteObject(hFont);

    WSACleanup();
  }
};