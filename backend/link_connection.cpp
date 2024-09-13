#include <pybind11/pybind11.h>
using namespace std;

// -> funciones para generar conexion con python frontend
int insert_record(const string& name, int song_id) {
  return 0;
};

// -> funcion python para conexion con frontend
PYBIND11_MODULE(link_connection, m) {
  m.def("insert_record", &insert_record, "A function to insert a record");
}
