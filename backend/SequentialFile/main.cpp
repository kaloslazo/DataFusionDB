#include "SequentialFile.cpp"

using namespace std;

int main(){
    SequentialFile file("data.dat", "aux.dat", sizeof(Record));
    // file.destroy();
    // auto vector_records = read_from_csv("data/data.csv");
    // for (auto record : vector_records){
    //     // record.print();
    //     file.insert(record);
    // }



    // file.remove_record(625);
    // file.description();

    // auto records = file.range_search(1200, 3000);
    // for (auto record : records){
    //     record.print();
    // }

    auto record = file.search(7230);
    record.print();
}