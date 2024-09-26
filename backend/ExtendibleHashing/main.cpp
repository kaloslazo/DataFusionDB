#include <ctime>
#include "ExtendibleHashing.cpp"

using namespace std;

int main() {
  // example instance with global depth 1 and bucket size 3, 2 rows in directory
  // (0, 1)
  ExtendibleHashing<EHRecordA, string> eh(1, 10);
  bool is_ok = true;

  cout << "[!] Creating files" << endl;

  eh.Create("../data/file_data.dat", "../data/file_buckets.dat");

  cout << "------------------------------------------\n";

  cout << "[!] Opening files and inserting records" << endl;
  clock_t start = clock();
  eh.Load_csv("../data/spotify_data_clean.csv");
  clock_t end = clock();
  // eh.Print();

  cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  cout << "[!] n_access: " << eh.n_access << endl;

  cout << "------------------------------------------\n";

  cout << "[!] Getting all records" << endl;
  eh.n_access = 0;
  start = clock();
  vector<EHRecordA> records = eh.Get_all();
  end = clock();

  cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  cout << "[!] n_access: " << eh.n_access << endl;

  cout << "------------------------------------------\n";
  cout << "[!] Searching all records" << endl;
  eh.n_access = 0;
  start = clock();
  for (auto& record : records) {
    optional<EHRecordA> result = eh.Search(record.key());
    if (result == nullopt) {
      cerr << "[!] Record not found" << endl;
      cout << "[!] serching key: " << record.key() << endl;
      eh.Close();
      exit(0);
    }
  }
  end = clock();
  cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  cout << "[!] n_access: " << eh.n_access << endl;
  cout << "------------------------------------------\n";
  cout << "[!] Removing all records" << endl;
  eh.n_access = 0;
  start = clock();
  for (auto& record : records) {
    is_ok = eh.Remove(record.key());
    if (!is_ok) {
      cerr << "[!] Error removing record with key: " << record.key() << endl;
    }
  }

  end = clock();
  cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  cout << "[!] n_access: " << eh.n_access << endl;

  cout << "------------------------------------------\n";

  return 0;
};
