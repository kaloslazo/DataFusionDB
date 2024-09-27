#include <ctime>
#include "ExtendibleHashing.hpp"

using namespace std;

vector<EHRecordB> leerCSVB(const string& filename) {
  ifstream file(filename);
  vector<EHRecordB> records;
  string line;

  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo.\n";
    return records;
  }

  // skip header line
  getline(file, line);

  while (getline(file, line)) {
    stringstream ss(line);
    string token;
    EHRecordB record;

    getline(ss, token, ',');
    record.year = stoi(token);

    getline(ss, token, ',');
    strncpy(record.make, token.c_str(), sizeof(record.make));
    record.make[sizeof(record.make) - 1] = '\0';

    getline(ss, token, ',');
    strncpy(record.model, token.c_str(), sizeof(record.model));
    record.model[sizeof(record.model) - 1] = '\0';

    getline(ss, token, '\n');
    strncpy(record.vin, token.c_str(), sizeof(record.vin));
    record.vin[sizeof(record.vin) - 1] = '\0';

    // getline(ss, token, ',');
    // strncpy(record.id, token.c_str(), sizeof(record.id));
    // record.id[sizeof(record.id) - 1] = '\0';

    // getline(ss, token, ',');
    // strncpy(record.name, token.c_str(), sizeof(record.name));
    // record.name[sizeof(record.name) - 1] = '\0';

    // getline(ss, token, ',');
    // strncpy(record.album, token.c_str(), sizeof(record.album));
    // record.album[sizeof(record.album) - 1] = '\0';

    // getline(ss, token, ',');
    // strncpy(record.album_id, token.c_str(), sizeof(record.album_id));
    // record.album_id[sizeof(record.album_id) - 1] = '\0';

    // getline(ss, token, '\n');
    // strncpy(record.artists, token.c_str(), sizeof(record.artists));
    // record.artists[sizeof(record.artists) - 1] = '\0';

    records.push_back(record);
    // cout << record.vin << endl;
  }

  file.close();
  return records;
}

int main() {
  // example instance with global depth 1 and bucket size 3, 2 rows in directory
  // (0, 1)
  ExtendibleHashing<EHRecordB, string> eh("../data/file");

  cout << "[!] Creating files" << endl;

  cout << "------------------------------------------\n";

  cout << "[!] Opening files and inserting records" << endl;
  clock_t start = clock();
  eh.loadCSV("../data/car_prices_clean.csv");
  clock_t end = clock();
  // eh.Print();

  cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  cout << "[!] n_access: " << eh.n_access << endl;

  cout << "------------------------------------------\n";

  cout << "[!] Getting all records" << endl;
  eh.n_access = 0;
  start = clock();
  vector<EHRecordB> records = leerCSVB("../data/car_prices_clean.csv");
  end = clock();

  cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  cout << "[!] n_access: " << eh.n_access << endl;

  cout << "------------------------------------------\n";
  cout << "[!] Searching all records" << endl;
  eh.n_access = 0;
  start = clock();
  for (auto& record : records) {
    vector<EHRecordB> result = eh.search(record.key());
    if (result.size() == 0) {
      cerr << "[!] Record not found" << endl;
      cout << "[!] serching key: " << record.key() << endl;
      exit(0);
    }
  }
  end = clock();
  cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  cout << "[!] n_access: " << eh.n_access << endl;
  cout << "------------------------------------------\n";
  // search 1 record
  eh.n_access = 0;
  start = clock();
  vector<EHRecordB> result = eh.search(records[0].key());
  end = clock();
  cout << "Time for search: " << (double)(end - start) / CLOCKS_PER_SEC << "\n";
  cout << "n_access: " << eh.n_access << "\n";

  // delete 1 record
  eh.n_access = 0;
  start = clock();
  eh.remove(records[9855].key());
  end = clock();
  cout << "Time for delete: " << (double)(end - start) / CLOCKS_PER_SEC << "\n";
  cout << "n_access: " << eh.n_access << "\n";

  // insert 1 record
  eh.n_access = 0;
  start = clock();
  eh.add(records[9855]);
  end = clock();
  cout << "Time for insert: " << (double)(end - start) / CLOCKS_PER_SEC << "\n";
  cout << "n_access: " << eh.n_access << "\n";
  // cout << "[!] Removing all records" << endl;
  // eh.n_access = 0;
  // start = clock();
  // for (auto& record : records) {
  //   is_ok = eh.Remove(record.key());
  //   if (!is_ok) {
  //     cerr << "[!] Error removing record with key: " << record.key() << endl;
  //   }
  // }

  // end = clock();
  // cout << "[!] Time: " << (double)(end - start) / CLOCKS_PER_SEC << endl;
  // cout << "[!] n_access: " << eh.n_access << endl;

  // cout << "------------------------------------------\n";

  eh.Close();

  return 0;
};
