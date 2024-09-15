#ifndef RECORD_HPP
#define RECORD_HPP

struct Record {
  int key;
  char name[10];
  int age;
  int term;

  Record();
  Record(int _key, char _name[10], int _age, int _term);

  void show_record();
};

#endif // RECORD_HPP
