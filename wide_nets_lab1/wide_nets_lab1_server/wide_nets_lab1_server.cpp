
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <bitset>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


std::string worddecode(std::string binword) {
  int control = 5;
  int len = 15;
  int codelen = 20;
  int array[20];
  int mistake[5]{ 0 };

  //std::string encodedString = "11110010001011110001";
  std::string encodedString = binword;

  for (int i = 0; i < codelen; i++) {
    array[i] = (int)encodedString[i]-48;        // Ошибка тут!!!
    std::cout << array[i];
  }

  for (int i = 0; i < codelen; i++) {
    std::bitset<5> b(i + 1);
    for (int j = 0; j < control; j++) {
      mistake[j] += b[j] * (array[i]);
    }
  }

  int error(0);
  std::cout << "\nматрица синдромов\n";
  for (int i = 0; i < control; i++) {
    mistake[i] %= 2;
    std::cout << mistake[i];
    error += mistake[i] * pow(2, i); 
  }
  std::cout << "\nОшибка " << error << "\n";

  //Проверка на чётность
  int evenbit((int)encodedString[codelen] - 48); // бит чётности
  for (int i = 0; i < codelen; i++) {
    evenbit += array[i];
  }
  evenbit %= 2;

  std::string res;
  if (evenbit) {
    res = "Единичная ошибка "+error;
  }
  else {
    if (error) {
      res = "Двойная ошибка ";
    }
    else {
      res = "Нет ошибок ";
    }
  }
  
  

 /* for (int i = 0; i < codelen; i++)
  {
    if (!(array[i] == 0 && res.size() == 0))
      res += std::to_string(array[i]);
  }*/
  //std::cout << "\n" << res;
  return res;
}


std::string textdecode(std::string bintext) {
  int codelen = 20;
  //int len = 15;
  //int control = 5;

  std::cout << "Декодирование сообщения" << std::endl;
  //std::string binaryString = "100100011000111011110110110011100101001";
  std::string binaryString = bintext;
  std::cout << binaryString << std::endl;
  std::cout << binaryString.size() << std::endl;

  std::string res = "b"; //убрать лишний символ

  //std::string a;// проба
  for (int k = 0; k < binaryString.size(); k += (codelen+1)) {
    std::string codeword = binaryString.substr(k, (codelen+1));
    std::cout << codeword << std::endl;

    std::cout << "\nДекодирование слова\n";
    res += worddecode(codeword) + " ";
    std::cout << "\nРезультат слова\n" << res << "\n";

  //  for (int i = 0; i < control; i++) {
  //    int p = pow(2, i) - 1;
  //    word.insert(p, std::string("0"));
  //  }
 
  }

  return res;
}


int __cdecl main(void)
{
  setlocale(LC_ALL, "Russian");
  WSADATA wsaData;
  int iResult;

  SOCKET ListenSocket = INVALID_SOCKET;
  SOCKET ClientSocket = INVALID_SOCKET;

  struct addrinfo* result = NULL;
  struct addrinfo hints;

  int iSendResult;
  char recvbuf[DEFAULT_BUFLEN];
  int recvbuflen = DEFAULT_BUFLEN;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed with error: %d\n", iResult);
    return 1;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
  if (iResult != 0) {
    printf("getaddrinfo failed with error: %d\n", iResult);
    WSACleanup();
    return 1;
  }

  // Create a SOCKET for connecting to server
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    printf("socket failed with error: %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    printf("listen failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    printf("accept failed with error: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  }

  // No longer need server socket
  closesocket(ListenSocket);
  const char* sendbuf;

  // Receive until the peer shuts down the connection
  do {

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

    recvbuf[iResult] = '\0';
    std::cout << "From the node: " << recvbuf << std::endl; // проба
    if (iResult > 0) {
      printf("Bytes received: %d\n", iResult);
    
    //std::string get  recvbuf; // Как полученный массив символов преобразовать в строку или это не нужно?
    //Декодирование сообщения 
      std::string encoded = recvbuf;
      std::string res_flag = textdecode("100100011000111011101011011001110010100111011100110000000101001111000011001011011101000000111010000001");
      //std::string res_flag = textdecode(encoded);
      std::cout << "\nРезультат декодирования слова\n" << res_flag << "\n";

      //sendbuf = res_flag.c_str();
      sendbuf = recvbuf;

      // Echo the buffer back to the sender
      iSendResult = send(ClientSocket, sendbuf, iResult, 0);
      if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
      }
      printf("Bytes sent: %d\n", iSendResult);
    }
    else if (iResult == 0)
      printf("Connection closing...\n");
    else {
      printf("recv failed with error: %d\n", WSAGetLastError());
      closesocket(ClientSocket);
      WSACleanup();
      return 1;
    }
    system("PAUSE"); // Добавлено, чтобы не закрывался
  } while (iResult > 0);

  // shutdown the connection since we're done
  iResult = shutdown(ClientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
    return 1;
  }

  

  // cleanup
  closesocket(ClientSocket);
  WSACleanup();

  return 0;
}