import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QLabel, QComboBox, QSpinBox
from PyQt5.QtCore import QTimer
import pyqtgraph as pg
import serial.tools.list_ports  # Add this line
from datetime import datetime, timedelta

class ArduinoPlotter(QMainWindow):
    def __init__(self, parent=None):
        super(ArduinoPlotter, self).__init__(parent)
        self.setWindowTitle("Arduino Serial Plotter")

        # Create a QWidget for the central widget
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)

        # Create a QVBoxLayout
        layout = QVBoxLayout(central_widget)

        # Create a QComboBox for port selection
        self.port_combobox = QComboBox()
        layout.addWidget(self.port_combobox)

        # Create a QComboBox for baud rate selection
        baud_rates = [300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880, 115200, 230400, 250000, 500000, 1000000, 2000000]
        self.baud_combobox = QComboBox()
        self.baud_combobox.addItems(map(str, baud_rates))
        self.baud_combobox.setCurrentText("115200")  # Default baud rate
        layout.addWidget(self.baud_combobox)

        # Create a PlotWidget from pyqtgraph
        self.plot_widget = pg.PlotWidget()
        layout.addWidget(self.plot_widget)

        # Create a QLabel for displaying data description
        self.description_label = QLabel()
        layout.addWidget(self.description_label)

        # Serial connection
        self.serial_port = None  # Will be initialized in setup_serial_port method

        # Set up timer for updating the plot
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(10)  # Update every 100 milliseconds

        # Initialize data lists
        self.max_data_points = 50  # Adjust the number of points to display
        self.time_data = []
        self.x_data = []
        self.y_data = []

        # Set up the plot
        self.curve_x = self.plot_widget.plot(pen='b', name='X')
        self.curve_y = self.plot_widget.plot(pen='r', name='Y')

        # Set up fixed Y-axis range
        self.fixed_y_min = -1500  # Adjust the minimum Y-axis value
        self.fixed_y_max = 1500   # Adjust the maximum Y-axis value
        self.plot_widget.getPlotItem().getViewBox().setRange(yRange=[self.fixed_y_min, self.fixed_y_max], disableAutoRange=True)

        # Set up ports and baud rates
        self.setup_serial_ports()
        self.port_combobox.currentIndexChanged.connect(self.setup_serial_port)

    def setup_serial_ports(self):
        # Populate available ports
        available_ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combobox.addItems(available_ports)

    def setup_serial_port(self):
        # Close existing serial port
        if self.serial_port and self.serial_port.isOpen():
            self.serial_port.close()

        # Initialize new serial port
        port_name = self.port_combobox.currentText()
        baud_rate = int(self.baud_combobox.currentText())  # Convert to integer
        self.serial_port = serial.Serial(port_name, baud_rate)

    def update_plot(self):
        if self.serial_port:
            line = self.serial_port.readline().decode('utf-8').strip()
            if ';' in line:
                try:
                    x, y = map(float, line.split(';'))

                    # Append new data points
                    current_time = datetime.now()
                    self.time_data.append(current_time)
                    self.x_data.append(x)
                    self.y_data.append(y)

                    # Remove old data points if exceeding the limit
                    if len(self.time_data) > self.max_data_points:
                        self.time_data.pop(0)
                        self.x_data.pop(0)
                        self.y_data.pop(0)

                    # Update the plot data
                    self.curve_x.setData(x=[t.timestamp() for t in self.time_data], y=self.x_data)
                    self.curve_y.setData(x=[t.timestamp() for t in self.time_data], y=self.y_data)

                    # Update the description label
                    self.description_label.setText(f"X: {x}, Y: {y}")

                except ValueError as e:
                    print(f"Error parsing data: {e}")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = ArduinoPlotter()
    window.setGeometry(100, 100, 800, 600)
    window.show()
    sys.exit(app.exec_())
