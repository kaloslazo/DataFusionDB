#include "ExtendibleHashing.cpp"

using namespace std;

int main() {
  // example instance with global depth 1 and bucket size 3, 2 rows in directory (0, 1)
  ExtendibleHashing eh(1, 3);

  eh.Create("file_data.dat", "file_buckets.dat");

  return 0;
};
