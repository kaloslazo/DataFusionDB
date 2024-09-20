#include "SequentialFile.cpp"

using namespace std;

int main(){
    SequentialFile file("data.dat", "aux.dat", sizeof(Record));
    // file.destroy();
    // auto vector_records = read_from_csv("data/data.csv");
    // for (auto record : vector_records){
    //     record.print();
    //     file.insert(record);
    // }
    file.description();
}