#include "Record.hpp"
#include "SQLParser.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

SQLParser sql_parser;

vector<Record*> execute_query(const string &query) {
  return sql_parser.execute_query(query);
}

PYBIND11_MODULE(link_connection, m) {
  py::class_<Record, std::unique_ptr<Record, py::nodelete>>(m, "Record")
    .def("key", &Record::key)
    .def("Print", &Record::Print);

  py::class_<RecordA, Record, std::unique_ptr<RecordA, py::nodelete>>(m, "RecordA")
    .def(py::init<const std::string&, const std::string&, const std::string&, const std::string&, const std::string&>())
    .def_readwrite("id", &RecordA::id)
    .def_readwrite("name", &RecordA::name)
    .def_readwrite("album", &RecordA::album)
    .def_readwrite("album_id", &RecordA::album_id)
    .def_readwrite("artists", &RecordA::artists);

  py::class_<RecordB, Record, std::unique_ptr<RecordB, py::nodelete>>(m, "RecordB")
    .def(py::init<int, const std::string&, const std::string&, const std::string&>())
    .def_readwrite("year", &RecordB::year)
    .def_readwrite("make", &RecordB::make)
    .def_readwrite("model", &RecordB::model)
    .def_readwrite("vin", &RecordB::vin);

  m.def("execute_query", &execute_query, "Function to handle SQL queries", py::return_value_policy::reference_internal);
}
