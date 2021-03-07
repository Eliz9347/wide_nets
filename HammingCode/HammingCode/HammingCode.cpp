
#include <iostream>
#include <string>
#include <bitset>
#include <cmath>

std::string encode(std::string binword) {

  int control = 5;
  int len = 15;
  int codelen = 20;
  int matr[5]{ 0 };
  int array[20];

  for (int i = 0; i < codelen; i++) {
    std::bitset<5> b(i + 1);
    std::cout << b.to_string() << "\n";
    for (int j = 0; j < 5; j++) {
      matr[j] += b[j] * (array[i]);
    }
  }

  for (int i = 0; i < control; i++) {
    matr[i] %= 2;
    int p = pow(2, i) - 1;
    std::cout << matr[i] << " " << p << "\n";

    array[p] = matr[i];
  }

  for (int i = 0; i < codelen; i++) {
    std::cout << array[i];
  }

  std::string res;

  for (int i = 0; i < codelen; i++)
  {
    if (!(array[i] == 0 && res.size() == 0))
      res += std::to_string(array[i]);
  }
    std::cout << "\n" << res;
  return res;
}

int main()
{
  std::string s = "Long Text";
  /*for (std::size_t i = 0; i < s.size(); ++i)
  {
    std::cout << std::bitset<8>(s.c_str()[i]);
  }

  std::cout << '\n';*/

  std::string binaryString = "";
  for (char& _char : s) {
    binaryString += std::bitset<8>(_char).to_string();
  }
  std::cout << binaryString << std::endl;

  int control = 5;
  int bitarray[20]{ 0 };

  std::string a;// проба
  for (int k = 0; k < binaryString.size(); k+=15) {
    std::string word = binaryString.substr(k, 15);
    std::cout << word<< std::endl;
    
    for (int i = 0; i < control; i++) {
      int p = pow(2, i) - 1;
      word.insert(p, std::string("0"));
    }

    std::cout << word << std::endl;
    a = word;
  }

  int len = 15;
  int codelen = 20;

  std::cout << "Check" << std::endl;
  std::cout << a << std::endl;
  //if (a.size() < len) {
  //  for (int l = 0; l< len - a.size(); l++) {
  //    //std::cout << l << std::endl;
  //    //a += std::string("0");
  //    //a.insert(len + l, std::string("0"));
  //  }
  //}
  std::cout << a << std::endl;


  std::cout << "all \n"<< std::endl;
  binaryString = "100100101110001";
  
  std::string encodedString = "00100010001011100001";
  int array[20];
  std::cout << " Symbol Array\n";
  for (int i = 0; i < codelen; i++) {
    array[i] = (int)encodedString[i]-48;
    std::cout << array[i] << "\n";
  }
  std::cout <<"\n";

  //Кодирование
  int matr[5]{ 0 };

  for (int i = 0; i < codelen; i++) {
    std::bitset<5> b(i + 1);
    std::cout << b.to_string() << "\n";
    for (int j = 0; j < 5; j++) {
      matr[j] += b[j] * (array[i]);
    }
  }

  for (int i = 0; i < control; i++) {
    matr[i] %= 2;
    int p = pow(2, i)-1;
    std::cout << matr[i] <<" " << p << "\n";

    array[p] = matr[i];
  }

  for (int i = 0; i < codelen; i++) {
    std::cout << array[i];
  }

  std::cout << "\n Add mistakes";
  array[5] = 1;


  std::cout << "\n Decoding";
  // Раскодирование
  int mistake[5]{ 0 };

  for (int i = 0; i < codelen; i++) {
    std::bitset<5> b(i + 1);
    for (int j = 0; j < 5; j++) {
      mistake[j] += b[j] * (array[i]);
    }
  }

  for (int i = 0; i < 5; i++) {
    mistake[i] %= 2;
    std::cout << mistake[i] <<"\n";
  }



  /*std::bitset<8> c(s[0]);
  std::cout << "c=" << c << " char(c.to_ulong())=" << char(c.to_ulong()) << "\n";*/

  //char * buf = data.c_str; //buf is binary





    return 0;
}
