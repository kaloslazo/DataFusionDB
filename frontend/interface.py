import sys
import link_connection
from PyQt6 import QtWidgets, QtCore

class MainWindow(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.registers = []
        self.init_ui()
        self.setStyleSheet("""
            QWidget {
                background-color: #001633;
                color: #FFFFFF;
                font-family: Arial, sans-serif;
            }
            #titleLabel {
                margin-top: 20px;
                font-size: 28px;
                font-weight: bold;
                color: #006FFD;
            }
            QLabel#creatorsLabel {
                font-size: 16px;
                color: #CCCCCC;
                margin-bottom: 20px;
                font-weight: normal;
            }
            QLabel {
                font-weight: bold;
                font-size: 14px;
            }
            QPushButton {
                background-color: #006FFD;
                color: white;
                padding: 8px 15px;
                border: 1px solid #006FFD;
                border-radius: 5px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #0050C0;
            }
            QLineEdit {
                padding: 8px;
                border: 1px solid #006FFD;
                border-radius: 5px;
                background-color: transparent;
                color: #FFFFFF;
                font-size: 14px;
            }
            QTableWidget {
                gridline-color: #006FFD;
                background-color: transparent;
                color: #FFFFFF;
                border: 1px solid #006FFD;
                border-radius: 5px;
            }
            QHeaderView::section {
                background-color: transparent;
                padding: 6px;
                border: 1px solid #006FFD;
                font-weight: bold;
                color: #FFFFFF;
            }
            QTableWidget::item {
                padding: 5px;
            }
        """)

    def init_ui(self):
        self.setWindowTitle("DataFusionDB")
        self.setGeometry(100, 100, 800, 600)

        # Title and creators
        self.title_label = QtWidgets.QLabel("DataFusion DB")
        self.title_label.setObjectName("titleLabel")
        self.title_label.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)

        self.creators_label = QtWidgets.QLabel("Proyecto 01: Base de Datos II (CS2042)")
        self.creators_label.setObjectName("creatorsLabel")
        self.creators_label.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)

        # Prompt for user SQL query
        self.label_sql_query = QtWidgets.QLabel("SQL Query:")
        self.entry_sql_query = QtWidgets.QLineEdit()

        # Button to execute SQL query
        self.boton_sql_query = QtWidgets.QPushButton("Execute")
        self.boton_sql_query.clicked.connect(self.execute_sql_query)

        # Table with SQL query results
        self.label_sql_execution = QtWidgets.QLabel("SQL Execution Results:")
        self.table_sql_execution = QtWidgets.QTableWidget()
        self.table_sql_execution.setStyleSheet("""
            QTableWidget {
                border: 1px solid #006FFD;
                border-radius: 10px;
                background-color: transparent;
            }
        """)

        # Layouts
        header_layout = QtWidgets.QVBoxLayout()
        header_layout.addWidget(self.title_label)
        header_layout.addWidget(self.creators_label)

        query_layout = QtWidgets.QHBoxLayout()
        query_layout.addWidget(self.label_sql_query)
        query_layout.addWidget(self.entry_sql_query)
        query_layout.addWidget(self.boton_sql_query)

        main_layout = QtWidgets.QVBoxLayout()
        main_layout.addLayout(header_layout)
        main_layout.addLayout(query_layout)
        main_layout.addWidget(self.label_sql_execution)
        main_layout.addWidget(self.table_sql_execution)

        self.setLayout(main_layout)
        self.show()

    def fill_table(self):
        if not self.registers:
            return

        # Determine the type of record
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
            error_dialog = QtWidgets.QMessageBox(self)
            error_dialog.setIcon(QtWidgets.QMessageBox.Icon.Critical)
            error_dialog.setText("Error")
            error_dialog.setInformativeText(str(e))
            error_dialog.setWindowTitle("Error")
            error_dialog.exec()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    main_window = MainWindow()
    sys.exit(app.exec())
