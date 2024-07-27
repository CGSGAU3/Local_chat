#include "server.h"

#pragma warning(disable: 4996)


int main( int argc, char *argv[] )
{
  Server serv;

  serv.listenConnections();
  system("pause");

  return 0;
}