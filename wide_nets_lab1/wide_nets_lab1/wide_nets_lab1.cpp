﻿
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <bitset>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

std::string wordencode(std::string binword) {
  int control = 5;
  int len = 15;
  int codelen = 20;
  int matr[5]{ 0 };
  int array[20];

  //std::string encodedString = "00100010001011100001";
  std::string encodedString = binword;
  //std::cout << " Symbol Array\n";
  for (int i = 0; i < codelen; i++) {
    array[i] = (int)encodedString[i] - 48;
    //std::cout << array[i];
  }
  std::cout << "\n";

  for (int i = 0; i < codelen; i++) {
    std::bitset<5> b(i + 1);
    //std::cout << b.to_string() << "\n";
    for (int j = 0; j < 5; j++) {
      matr[j] += b[j] * (array[i]);
    }
  }

  for (int i = 0; i < control; i++) {
    matr[i] %= 2;
    int p = pow(2, i) - 1;
    //std::cout << matr[i] << " " << p << "\n";
    array[p] = matr[i];
  }


  std::string res;

  for (int i = 0; i < codelen; i++)
  {
    if (!(array[i] == 0 && res.size() == 0))
      res += std::to_string(array[i]);
  }

  int evenbit(0); // бит чётности
  for (int i = 0; i < codelen; i++) {
    evenbit += array[i];
  }
  res += std::to_string(evenbit%2);


  //std::cout << "\n" << res;
  return res;
}



std::string textencode(std::string binword) {
  int len = 15;
  int control = 5;

  std::string s = "Long Text";
  // Кодирование сообщения
  std::string binaryString = "";
  for (char& _char : s) {
    binaryString += std::bitset<8>(_char).to_string();
  }
  std::cout << binaryString << std::endl;

  std::string res = ""; //убрать лишний символ

  std::string a;// проба
  for (int k = 0; k < binaryString.size(); k += len) {
    std::string word = binaryString.substr(k, len);
    std::cout << word << std::endl;


    if (word.size() < len) {
      int extr = len - word.size();
      //std::cout << extr << std::endl;
      for (int i = 0; i < extr; i++) {
        word += "0";
      }
    }

    std::cout << word << std::endl;
    
    for (int i = 0; i < control; i++) {
      int p = pow(2, i) - 1;
      word.insert(p, std::string("0"));
    }

    //std::cout << word << std::endl;

    a = word;

    std::cout << "\nКодирование слова\n";
    res += wordencode(a);
    std::cout << "\nРезультат слова\n" << res << "\n";
  }

  return res;
}


int __cdecl main(int argc, char** argv)
{
  setlocale(LC_ALL, "Russian");
  WSADATA wsaData;
  SOCKET ConnectSocket = INVALID_SOCKET;
  struct addrinfo* result = NULL,
    * ptr = NULL,
    hints;
  //const char* sendbuf = "this is a test Интересно";

  std::string s = "Long Text";
  // Кодирование сообщения
  std::cout << "\nКодирование сообщения\n";
  std::string coderes = textencode(s);
  std::cout << coderes;
  std::cout << "\nРезультат\n" << coderes << "\n";
  std::cout << "\n\n";

  const char* sendbuf = coderes.c_str();

  char recvbuf[DEFAULT_BUFLEN];
  int iResult;
  int recvbuflen = DEFAULT_BUFLEN;

  if (argc == 1) { // если в аргументах только имя программы
    std::cout << "no arguments!" << std::endl; // выводим, что нет аргументов
  }
  if (argc == 2) {
    // иначе выводим все аргументы, которые переданы
    for (int i = 1; i < argc; i++) {
      std::cout << "argv[" << i << "] - " << argv[i] << std::endl;
    }
  }

  // Validate the parameters
  if (argc != 2) {
    printf("usage: %s server-name\n", argv[0]);
    return 1;
  }

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed with error: %d\n", iResult);
    return 1;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
  if (iResult != 0) {
    printf("getaddrinfo failed with error: %d\n", iResult);
    WSACleanup();
    return 1;
  }

  // Attempt to connect to an address until one succeeds
  for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
      ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
      printf("socket failed with error: %ld\n", WSAGetLastError());
      WSACleanup();
      return 1;
    }

    // Connect to server.
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
      closesocket(ConnectSocket);
      ConnectSocket = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(result);

  if (ConnectSocket == INVALID_SOCKET) {
    printf("Unable to connect to server!\n");
    WSACleanup();
    return 1;
  }

  // Send an initial buffer
  iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
  if (iResult == SOCKET_ERROR) {
    printf("send failed with error: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
  }

  printf("Bytes Sent: %ld\n", iResult);

  // shutdown the connection since no more data will be sent
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
  }

  int re;// проба
  // Receive until the peer closes the connection
  do {

    re = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (re > 0) {
      printf("Bytes received: %d\n", re);
      recvbuf[re] = '\0';
      std::cout << "From the node: " << recvbuf << std::endl; // проба
     
    }
      
    else if (iResult == 0)
      printf("Connection closed\n");
    else
      printf("recv failed with error: %d\n", WSAGetLastError());

  } while (re > 0);


  //system("PAUSE");

  // cleanup
  closesocket(ConnectSocket);
  WSACleanup();


  return 0;
}