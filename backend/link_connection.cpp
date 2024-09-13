#include "Record.hpp"
#include "SQLParser.hpp"
#include <string>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>

using namespace std;
namespace py = pybind11;

// modularization - invoque parser for execute query
SQLParser sql_parser;
vector<Record> execute_query(const string &query) {
  return sql_parser.execute_query(query);
};

// handle python cpp interconection by pybind11
PYBIND11_MODULE(link_connection, m) {
  py::class_<Record>(m, "Record")
    .def(py::init<string, string>())
    .def_readwrite("name", &Record::name)
    .def_readwrite("id", &Record::id);

  m.def("execute_query", &execute_query, "Function for handle SQL queries");
};  
