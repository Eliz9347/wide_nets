
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <bitset>

#include <ctime>
#include <cstdlib>

#include <fstream>
#include <cstring>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 5001
#define DEFAULT_PORT "27015"

// Функция кодирования слова кодом Хемминга
std::string wordencode(std::string binword) {
  int control = 7;
  int len = 71;
  int codelen = 78;
  int matr[7]{ 0 };
  int array[78]{0};

  //std::string encodedString = "00100010001011100001";
  std::string encodedString = binword;

  for (int i = 0; i < codelen; i++) {
    array[i] = (int)encodedString[i] - 48;
    //std::cout << array[i];
  }
 // std::cout << "\n";

  // Расчёт контрольных битов
  for (int i = 0; i < codelen; i++) {
    std::bitset<7> b(i + 1);                            // можно сделать лучше
    //std::cout << b.to_string() << "\n";
    for (int j = 0; j < control; j++) {
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
    res += std::to_string(array[i]);
  }

  //std::cout << "Закодировано" << std::endl;
  //std::cout << res << " длина " << res.size() << "\n";

  //std::cout << "Добавление бита чётности" << std::endl;
  int evenbit(0); // бит чётности
  for (int i = 0; i < codelen; i++) {
    evenbit += array[i];
  }
  res += std::to_string(evenbit%2);
  //std::cout << res << " длина " << res.size() << "\n";

  return res;
}

// Функция кодирования текста кодом Хемминга
std::string textencode(std::string binword) {
  int len = 71;
  int control = 7;

  std::cout << "Кодирование сообщения кодом Хемминга с длиной информационного слова 71" << std::endl;
  // Кодирование сообщения
  std::string binaryString = "";
  for (char& _char : binword) {
    binaryString += std::bitset<8>(_char).to_string();
  }
  //std::cout << binaryString << " длина "<< binaryString.size() << std::endl;
  std::cout << "Длина двоичного " << binaryString.size() << std::endl;

  // строка для закодированного сообщения
  std::string res = "";

  // Разбивка сообщения на слова длины 15
  //std::cout << "Разбивка сообщения\n";
  for (int k = 0; k < binaryString.size(); k += len) {
    std::string word = binaryString.substr(k, len);
    
    //std::cout << "\nИсходное информационное слово" << std::endl;
    //std::cout << word << " длина " << word.size() << std::endl;

    
    // Добавление нулей к последнему слову
    if (word.size() < len) {
      //std::cout << "Добавление нулей при необходимости" << std::endl;
      int extr = len - word.size();
      //std::cout << extr << std::endl;
      for (int i = 0; i < extr; i++) {
        word += "0";
      }
      //std::cout << word << " длина " << word.size() << std::endl;
    }
    
    // Добавление контрольных битов
    //std::cout << "Добавление нулевых контрольных битов" << std::endl;
    for (int i = 0; i < control; i++) {
      int p = pow(2, i) - 1;
      word.insert(p, std::string("0"));
    }

    //std::cout << word << " длина " << word.size() << std::endl;
    //std::cout << "Кодирование слова\n";
    res += wordencode(word);
    //std::cout << "Результат слова\n" << res << " длина " << res.size() << "\n";
  }
  std::cout << "Закодировано\n";
  //std::cout << "Длина двоичного закодированного " << res.size() << std::endl;

  // Закодировано для сжатия
  // Добавление недостающих нулей для преобразования в символы
 /* int rest = res.size() % 8;
  std::cout << "Остаток " << rest << std::endl;*/

  //if (rest != 0) {
  //  //std::cout << "Добавление нулей при необходимости" << std::endl;
  //  int extr = 8-rest;
  //  for (int i = 0; i < extr; i++) {
  //    res += "0";
  //  }
  //  std::cout << "Длина двоичного удлинённого" << res.size() << std::endl;
  //  std::cout << "Остаток " << res.size() % 8 << std::endl;
  //}


  //std::string text = "";
  //for (int i = 0; i < res.size(); i += 8) {
  //  std::string temp = res.substr(i, 8);
  //  std::bitset<8> mybs(temp);
  //  char ch = mybs.to_ullong();
  //  text += ch;
  //}

  //std::cout << "Длина двоичного закодированного " << text.size() << std::endl;
  return res;
}

// Функция добавления единичных ошибок в сообщение, возвращает количество ошибок
int addmistake(std::string &message, int freq) {
  
  int n = message.size() / freq;  // максимальное количество ошибок в сообщении
  int mistakes(0); // реальное количество добавленных ошибок в сообщение
  float probability = float(rand()); // сброс первого

  for (int i = 0; i < n; i++) {
    probability = float(rand()) / float(RAND_MAX);
    //std::cout << "Вероятность ошибки в слове " << probability << "\n";
    // добавление ошибки
    if (probability+0.2 > 0.5) {
      int position = i*freq+(int)(rand() % (freq - 1)); // диапазон равен от 0 до freq (длина слова) включительно
      //std::cout << "Позиция " << position << "\n";
      //std::cout << "старый символ " << message[position] << "\n";
      if (message[position] == '1') { // проверить работу
        message.replace(position, 1, "0");
      }
      else {
        message.replace(position, 1, "1");
      }
      //std::cout << "новый символ " << message[position] << "\n";
      mistakes++;
    }
  }

  //std::cout << "Число ошибок " << mistakes << "\n";
  //std::cout << "новое сообщение\n" << message<< "\n";
  return mistakes;
}

// Функция добавления множественных ошибок (не более n на слово) в сообщение, возвращает количество ошибок
int addnmistakes(std::string& message, int freq, int n) {
  int allmistakes(0);
  int tempmistakes(0);
  for (int i = 0; i < n; i++) {
    std::cout << "Добавление порции ошибок " << "\n";
    tempmistakes = addmistake(message, freq);
    allmistakes += tempmistakes;
  }
  return allmistakes;
}


int __cdecl main(int argc, char** argv)
{
  //setlocale(LC_ALL, "Russian");
  setlocale(LC_ALL, "ru_RU.UTF8");
  //SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
  //SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
  //char string[20];
  //std::cin >> string; // вводим строку, используя Кириллицу
  //std::cout << "\nвывод: " << string << std::endl; // ввывод строки
  WSADATA wsaData;
  SOCKET ConnectSocket = INVALID_SOCKET;
  struct addrinfo* result = NULL,
    * ptr = NULL,
    hints;

  srand(time(NULL));

  std::string strInput;
  std::string message;
  

  // окрываем файл для чтения
  std::ifstream in("f.txt");
  if (in.is_open())
  {
    while (getline(in, strInput))
    {
      message += strInput;
    }
  }
  in.close();

  /*const char* arr = message.c_str();

  std::cout << "\nСимвольный массив: \n";
  std::cout << arr << std::endl;*/

  std::cout << message << std::endl;
  setlocale(LC_ALL, "Russian");
  std::cout << "\nДлина сообщения: \n" << message.size() << std::endl;

  // Кодирование сообщения
  std::cout << "\nКодирование сообщения\n";
  std::cout << "Количество ошибок:\n" << 0 << "\n";
  std::string coderes = textencode(message);
  std::cout << "\nРезультат\n" << coderes.size() << "\n";
  
  std::string coderes_one_err = coderes;
  std::cout << "\nДобавление ошибок\n";
  int mistakes = addmistake(coderes_one_err, 79);
  std::cout << "Количество единичных ошибок:\n" << mistakes << "\n";
  //int mistakes = addnmistakes(coderes, 79, 2);
  //std::cout << "\nРезультат\n" << coderes_err << "\n";
  
  std::cout << "\n\n";
   

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


  const char* sendbuf;
  
  /*sendbuf = ("len"+std::to_string(3*coderes.length())).c_str();
  iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);*/
  

  int k(0);
  while (k < coderes_one_err.length()) {
    std::string ter = coderes_one_err.substr(k, 5000);
    sendbuf = ter.c_str();
    std::cout << "\n Часть сообщения\n" << std::endl;
    //std::cout << sendbuf << std::endl;
    k += 5000;

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
      printf("send failed with error: %d\n", WSAGetLastError());
      closesocket(ConnectSocket);
      WSACleanup();
      return 1;
    }
    printf("Bytes Sent: %ld\n", iResult);

  }

  //std::cout << "\n Отправка второго сообщения\n" << std::endl;
  //k = 0;
  //while (k < coderes_one_err.length()) {
  //  std::string ter = coderes_one_err.substr(k, 5000);
  //  sendbuf = ter.c_str();
  //  std::cout << "\n Часть сообщения\n" << std::endl;
  //  //std::cout << sendbuf << std::endl;
  //  k += 5000;

  //  iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
  //  if (iResult == SOCKET_ERROR) {
  //    printf("send failed with error: %d\n", WSAGetLastError());
  //    closesocket(ConnectSocket);
  //    WSACleanup();
  //    return 1;
  //  }
  //  printf("Bytes Sent: %ld\n", iResult);

  //}

  //std::cout << "\n Отправка третьего сообщения\n" << std::endl;
  //k = 0;
  //while (k < coderes_many_err.length()) {
  //  std::string ter = coderes_many_err.substr(k, 5000);
  //  sendbuf = ter.c_str();
  //  std::cout << "\n Часть сообщения\n" << std::endl;
  //  //std::cout << sendbuf << std::endl;
  //  k += 5000;

  //  iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
  //  if (iResult == SOCKET_ERROR) {
  //    printf("send failed with error: %d\n", WSAGetLastError());
  //    closesocket(ConnectSocket);
  //    WSACleanup();
  //    return 1;
  //  }
  //  printf("Bytes Sent: %ld\n", iResult);

  //}


  // shutdown the connection since no more data will be sent
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
  }

  std::string decoded;
  int re(0);// проба
  // Receive until the peer closes the connection
  do {

    re = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (re > 0) {
      printf("Bytes received: %d\n", re);
      recvbuf[re] = '\0';
      
      
      if (re > 50) {
        
        setlocale(LC_ALL, "ru_RU.UTF8");
        decoded = recvbuf;
        std::cout << "Length: \n" << decoded.length() << "\n";
        //std::cout << decoded << "\n";
        //Обработка статистической информации об ошибках
        int underline = decoded.find('_');
        int zero = stoi(decoded.substr(0, underline));
        decoded.erase(0, underline + 1);

        underline = decoded.find('_');
        int one = stoi(decoded.substr(0, underline));
        decoded.erase(0, underline + 1);

        underline = decoded.find('_');
        int many = stoi(decoded.substr(0, underline));
        decoded.erase(0, underline + 1);
        std::cout << "Текст:\n" << decoded << "\n";

        setlocale(LC_ALL, "Russian");
        std::cout << "Количество правильно доставленных слов: " << zero << "\n";
        std::cout << "Количество исправленных ошибок: " << one << "\n";
        std::cout << "Количество множественных ошибок: " << many << "\n";
       
      }
      else {
        std::cout << "From the node: " << recvbuf << std::endl;
      }
      
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