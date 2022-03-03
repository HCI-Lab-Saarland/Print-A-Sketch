import serial.tools.list_ports
import statvar


class MySerial():
    canceled = False

    # Serial port
    ser = serial.Serial(port='/dev/cu.usbmodem143301', baudrate=115200, bytesize=8, parity='N', dsrdtr=True)

    def turn_on(self):
        self.ser.write(str('<').encode())  # This character turns the printer on
        print("serial port is ready...")

    def which_data(self):  # receiving the option from UI
        if self.ser.inWaiting() and self.ser.read(1) == b'B':  # If we get B, we are waiting for the option afterward

            var = self.ser.read(1)
            print(var)

            if var == b'\x01':  # If we receive 1 we should read 2 other bytes for the pattern and width
                width = self.ser.read(1)  # first byte
                pattern = self.ser.read(1)  # second byte
                data = [1, int.from_bytes(width, byteorder='big'), int.from_bytes(pattern, byteorder='big')]
                print("data:", data)
                return data

            elif var == b'\x02':
                return [2]

            elif var == b'\x03':
                index = self.ser.read(1)  # scan / print
                data = [3, int.from_bytes(index, byteorder='big')]
                return data

            elif var == b'\x04':
                index = self.ser.read(1)  # angle
                data = [4, int.from_bytes(index, byteorder='big')]
                return data

            elif var == b'\x05':
                data = [5]
                return data

            elif var == b'\x06':
                data = [6]
                return data

            elif var == b'\x07':
                data = [7]
                return data

            elif var == b'\x08':
                data = [8]
                return data

            elif var == b'\x09':
                data = [9]
                return data

            elif var == b'\x0a':
                data = [10]
                return data

            elif var == b'\x0b':
                data = [11]
                return data

            elif var == b'\x0c':
                data = [12]
                return data

            elif var == b'\x0d':
                data = [13]
                return data

            elif var == b'\x0e':
                data = [14]
                return data

            elif var == b'\x0f':
                data = [15]
                return data

            elif var == b'\x10':
                index = self.ser.read(1)  # angle
                data = [16, int.from_bytes(index, byteorder='big')]
                return data

    def send_data(self, data, blob):

        statvar.last_line = data  # always saving the last line of sending data
        self.ser.write(data)

        if blob:
            self.ser.write(b'\x01')
        else:
            self.ser.write(b'\x00')

        while True:
            if self.ser.inWaiting():
                var = self.ser.read(1)
                if var == b'A':  # Sending next line if we receive 'A'
                    break
                elif var == b'C':  # Cancel sending if we receive 'C' instead of the 'A'
                    self.canceled = True
                    print("**STOPPED**")
                    return

    def turn_off(self):
        self.ser.write(b'>')  # Sending this character turns the printer off
