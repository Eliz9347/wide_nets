
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