#ifndef RECORD_HPP
#define RECORD_HPP

// size 4 + 10 + 4 + 4 = 22 bytes
struct Record {
  int key;
  char name[10];
  int age;
  int term;

  Record();
  Record(int _key, const char* _name, int _age, int _term);

  void Print();
};

#endif // RECORD_HPP
