
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

//#define DEFAULT_BUFLEN 512
#define DEFAULT_BUFLEN 5001
#define DEFAULT_PORT "27015"

// Функция декодирования слова, закодированного кодом Хемминга
int worddecode(std::string binword) {
  int control = 7;
  int len = 71;
  int codelen = 78;
  int array[78]{ 0 };
  int mistake[7]{ 0 };

  for (int i = 0; i < codelen; i++) {
    array[i] = (int)binword[i]-48;
    //std::cout << array[i];
  }

  for (int i = 0; i < codelen; i++) {
    std::bitset<7> b(i + 1);
    for (int j = 0; j < control; j++) {
      mistake[j] += b[j] * (array[i]);
    }
  }

  int error(0);
  //std::cout << "\nматрица синдромов\n";
  for (int i = 0; i < control; i++) {
    mistake[i] %= 2;
    //std::cout << mistake[i];
    error += mistake[i] * pow(2, i); 
  }
  //std::cout << "\nОшибка " << error << "\n";

  //Проверка на чётность
  int evenbit((int)binword[codelen] - 48); // бит чётности
  for (int i = 0; i < codelen; i++) {
    evenbit += array[i];
  }
  evenbit %= 2;

  int res;
  if (evenbit) {
    //std::cout << "Единичная ошибка в " << error << "\n";
    res = error+2;
    //std::cout << array[error] << std::endl;
    //binword.replace(error, 1, std::to_string(not array[error]));
    //std::cout << "Исправленное слово\n" << binword << "\n";
  }
  else {
    if (error) {
      std::cout << "Двойная ошибка" << "\n";
      res = 2;
    }
    else {
      //std::cout << "Нет ошибок " << "\n";
      res = 0;
    }
  } 
  //std::cout << res  << "\n";

  return res;
}

// Функция декодирования текста, закодированного кодом Хемминга
std::string textdecode(std::string bintext) {
  int codelen = 78; // длина кодового слова без бита чётности
  int control = 7;

  std::cout << "Декодирование сообщения" << std::endl;
  //std::cout << encodedtext << std::endl;
  //std::cout << " длина " << encodedtext.size() << std::endl;
  //std::string bintext = "";
  //for (char& _char : encodedtext) {
  //  bintext += std::bitset<8>(_char).to_string();
  //}
  //std::cout << bintext << std::endl;
  //std::string binaryString = "100100011000111011110110110011100101001";
  std::cout << " длина " << bintext.size() << std::endl;

  int res(0);

  //для сбора статистики по количеству ошибок
  int zero(0);
  int one(0);
  int many(0);
  // для преобразования сообщения в текст
  std::string symbols;

  // Разбивка на кодовые слова длиной 79
  std::cout << "Разбивка на кодовые слова\n";
  for (int k = 0; k < bintext.size(); k += (codelen+1)) {
    std::string codeword = bintext.substr(k, (codelen+1));

    //std::cout << "\nКодовое слово\n";
    //std::cout << codeword << " длина " << codeword.size() << std::endl;
    //std::cout << "Декодирование слова\n";
    res = worddecode(codeword);
    //std::cout << "Результат слова\n" << res << "\n";


    // обработка статистики по ошибкам
    if (res == 0) {
      zero++;
    }
    else if (res > 2){
      one++;
      //Исправление единичной ошибки
      int nsymbol = not (bool)codeword[res-3];
      //std::cout << "старый символ " << codeword[res - 3] << "\n";
      //std::cout << "новый символ " << nsymbol << "\n";
      codeword.replace(res-3, 1, std::to_string(nsymbol));
      //std::cout << "Исправленное слово\n" << codeword << "\n";
    }
    else {
      many++;
    }

    // Удаление контрольных битов
    //std::cout << "Удаление бита чётности и контрольных битов\n";
    codeword.erase(codelen, 1);
    for (int i = control - 1; i >= 0; i--) {
      int p = pow(2, i) - 1;
      codeword.erase(p, 1);
    }
    //std::cout << codeword << " длина " << codeword.size() << "\n";

    //std::cout << "Строка символов\n";
    symbols += codeword;
    //std::cout << symbols << " длина " << symbols.size() << "\n";

  }
  
  //std::cout << "Teкст\n";
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

  std::cout << "Teкст\n";

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
  std::string mess;
  //iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
  /*std::string vol = recvbuf;
  int volume = stoi(vol);*/
  // Receive until the peer shuts down the connection
  do {

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    //std::cout << "Vol: " << volume << std::endl; // проба
   /* volume -= iResult;*/
    recvbuf[iResult] = '\0';
    //std::cout << "From the node: " << recvbuf << std::endl; // проба
    if (iResult > 0) {
      printf("Bytes received: %d\n", iResult);


      mess += recvbuf;
      std::cout << mess.size() << " уже\n";

      if (mess.size() > 35000) {
        setlocale(LC_ALL, "ru_RU.UTF8");
        std::string res_flag = textdecode(mess);
        mess = "";
       
        sendbuf = res_flag.c_str();
        iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);

        if (iSendResult == SOCKET_ERROR) {
          printf("send failed with error: %d\n", WSAGetLastError());
          closesocket(ClientSocket);
          WSACleanup();
          return 1;
        }
        std::cout << "Test: " << sendbuf << std::endl; // проба
        //std::cout << "Test: " << std::endl; // проба

      }
      else {
        sendbuf = "ok";
        std::cout << mess.size() << std::endl;
        iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iSendResult == SOCKET_ERROR) {
          printf("send failed with error: %d\n", WSAGetLastError());
          closesocket(ClientSocket);
          WSACleanup();
          return 1;
        }
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

  } while (iResult > 0);
    //(volume > 0);

  Sleep(1000);
  // shutdown the connection since we're done
  iResult = shutdown(ClientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
    return 1;
  }
  system("PAUSE"); // Добавлено, чтобы не закрывался
  

  // cleanup
  closesocket(ClientSocket);
  WSACleanup();

  return 0;
}