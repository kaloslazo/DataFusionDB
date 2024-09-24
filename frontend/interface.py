import sys
import link_connection
from PyQt6 import QtWidgets

class MainWindow(QtWidgets.QWidget):
    def __init__(self):
        super().__init__();
        self.registers = [];
        self.init_ui();

    def init_ui(self):
        self.setWindowTitle("DataFusionDB");
        self.setGeometry(100, 100, 400, 200)

        # prompt for user sql query
        self.label_sql_query = QtWidgets.QLabel("SQL Query");
        self.entry_sql_query = QtWidgets.QLineEdit();

        # button to execute sql query
        self.boton_sql_query = QtWidgets.QPushButton("Execute");
        self.boton_sql_query.clicked.connect(self.execute_sql_query);

        # table with sql query results
        self.label_sql_execution = QtWidgets.QLabel("SQL Execution");
        self.table_sql_execution = QtWidgets.QTableWidget();

        layout = QtWidgets.QVBoxLayout();
        layout.addWidget(self.label_sql_query);
        layout.addWidget(self.entry_sql_query);
        layout.addWidget(self.boton_sql_query);
        layout.addWidget(self.label_sql_execution);
        layout.addWidget(self.table_sql_execution);

        self.setLayout(layout);
        self.show()

    def fill_table(self):
        if not self.registers:
            return

        # Determinar el tipo de registro
        if isinstance(self.registers[0], link_connection.RecordA):
            headers = ["ID", "Name", "Album", "Album ID", "Artists"]
        elif isinstance(self.registers[0], link_connection.RecordB):
            headers = ["Year", "Make", "Model", "VIN"]
        else:
            print("Unknown record type")
            return

        self.table_sql_execution.setColumnCount(len(headers))
        self.table_sql_execution.setRowCount(len(self.registers))
        self.table_sql_execution.setHorizontalHeaderLabels(headers)

        for row, register in enumerate(self.registers):
            if isinstance(register, link_connection.RecordA):
                self.table_sql_execution.setItem(row, 0, QtWidgets.QTableWidgetItem(str(register.id)))
                self.table_sql_execution.setItem(row, 1, QtWidgets.QTableWidgetItem(str(register.name)))
                self.table_sql_execution.setItem(row, 2, QtWidgets.QTableWidgetItem(str(register.album)))
                self.table_sql_execution.setItem(row, 3, QtWidgets.QTableWidgetItem(str(register.album_id)))
                self.table_sql_execution.setItem(row, 4, QtWidgets.QTableWidgetItem(str(register.artists)))
            elif isinstance(register, link_connection.RecordB):
                self.table_sql_execution.setItem(row, 0, QtWidgets.QTableWidgetItem(str(register.year)))
                self.table_sql_execution.setItem(row, 1, QtWidgets.QTableWidgetItem(str(register.make)))
                self.table_sql_execution.setItem(row, 2, QtWidgets.QTableWidgetItem(str(register.model)))
                self.table_sql_execution.setItem(row, 3, QtWidgets.QTableWidgetItem(str(register.vin)))

        header = self.table_sql_execution.horizontalHeader()
        header.setSectionResizeMode(QtWidgets.QHeaderView.ResizeMode.ResizeToContents)

    def execute_sql_query(self):
        query = self.entry_sql_query.text()
        try:
            query_result = link_connection.execute_query(query)
            if query_result:
                self.registers = query_result
            self.fill_table()
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Error", str(e))

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    main_window = MainWindow()
    sys.exit(app.exec())
