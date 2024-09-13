#include "Record.hpp"
using namespace std;

Record::Record() : name(""), id("") {};

Record::Record(string name, string id) : name(name), id(id) {};

Record::~Record() {};
