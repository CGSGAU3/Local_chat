#pragma once

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include "win.h"

class Client : public Win
{
private:
  HWND hTextBox;
  HWND hLabel;
  HWND hSend;
  SOCKET connection;
  bool isShift;

  static void messageHandler( Client *win );
  static LRESULT editProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
  void sendToServer( void );

  void init( void ) override;
  void resize( void ) override;
  void paint( HDC hDC ) override;
  void keyboard( bool isUp, UINT key ) override;
  void command( HWND handle, WORD id, WORD cmd ) override;

public:
  Client( const std::string &name ) : Win(name)
  {
  }

  ~Client( void )
  {
  }
};