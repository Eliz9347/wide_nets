
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


int worddecode(std::string binword) {
  int control = 5;
  int len = 15;
  int codelen = 20;
  int array[20];
  int mistake[5]{ 0 };

  for (int i = 0; i < codelen; i++) {
    array[i] = (int)binword[i]-48;        // Ошибка тут!!!
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
  int evenbit((int)binword[codelen] - 48); // бит чётности
  for (int i = 0; i < codelen; i++) {
    evenbit += array[i];
  }
  evenbit %= 2;

  int res;
  if (evenbit) {
    std::cout << "Единичная ошибка в" << error << "\n";
    res = 1;
                                                        // Исправление ошибки нужно дописать
  }
  else {
    if (error) {
      std::cout << "Двойная ошибка" << "\n";
      res = 2;
    }
    else {
      std::cout << "Нет ошибок " << "\n";
      res = 0;
    }
  } 
  std::cout << res  << "\n";

  return res;
}


std::string textdecode(std::string bintext) {
  int codelen = 20; // длина кодового слова без бита чётности
  int control = 5;

  std::cout << "Декодирование сообщения" << std::endl;
  //std::string binaryString = "100100011000111011110110110011100101001";
  std::cout << bintext << " длина " << bintext.size() << std::endl;

  int res(0); //убрать лишний символ

  //для сбора статистики по количеству ошибок
  int zero(0);
  int one(0);
  int many(0);
  // для преобразования сообщения в текст
  std::string symbols;

  // Разбивка на кодовые слова длиной 21
  std::cout << "Разбивка на кодовые слова\n";
  for (int k = 0; k < bintext.size(); k += (codelen+1)) {
    std::string codeword = bintext.substr(k, (codelen+1));

    std::cout << "\nКодовое слово\n";
    std::cout << codeword << " длина " << codeword.size() << std::endl;
    std::cout << "Декодирование слова\n";
    res = worddecode(codeword);
    std::cout << "Результат слова\n" << res << "\n";

    // обработка статистики по ошибкам
    if (res == 0) {
      zero++;
    }
    else if (res == 1){
      one++;
    }
    else {
      many++;
    }

    // Удаление контрольных битов
      std::cout << "Удаление бита чётности и контрольных битов\n";
    codeword.erase(codelen, 1);
    for (int i = control - 1; i >= 0; i--) {
      int p = pow(2, i) - 1;
      codeword.erase(p, 1);
    }
    std::cout << codeword << " длина " << codeword.size() << "\n";

    std::cout << "Строка символов\n";
    symbols += codeword;
    std::cout << codeword << " длина " << symbols.size() << "\n";

  }

  std::cout << "Teкст\n";
  std::string text = "";
  for (int i = 0; i < symbols.size(); i += 8) {
    std::string temp = symbols.substr(i, 8);
    std::bitset<8> mybs(temp);
    char ch = mybs.to_ullong();
    text += ch;
  }
  std::cout << text << "\n";

  // Обработка статистической информации
  std::cout << "Статистика\n";
  std::string statistics = std::to_string(zero) + '_' + std::to_string(one) + '_' + std::to_string(many)+'_';
  std::cout << statistics << "\n";

  return statistics+text;
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
    
    //Декодирование сообщения 
      std::string encoded = recvbuf;
      std::string res_flag = textdecode(encoded);
      std::cout << "\nРезультат декодирования слова\n" << res_flag << "\n";

      sendbuf = res_flag.c_str();
      //sendbuf = recvbuf;

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