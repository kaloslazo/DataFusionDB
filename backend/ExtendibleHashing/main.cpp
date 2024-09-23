#include "ExtendibleHashing.cpp"

using namespace std;

int main() {
  // example instance with global depth 1 and bucket size 3, 2 rows in directory (0, 1)
  ExtendibleHashing<EHRecordA, string> eh(1, 3);

  eh.Create("./data/file_data.dat", "./data/file_buckets.dat");
  eh.Load_csv("../../data/spotify_data_clean.csv");
  // eh.Print();
  // eh.Search("MILLION DOLLAR BABY");



  return 0;
};
