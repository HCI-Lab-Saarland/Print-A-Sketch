import time

# OpenCV Module
import cv2
# UI Modules
from kivy.app import App
from kivy.clock import Clock
from kivy.config import Config
from kivy.core.image import Texture
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.floatlayout import FloatLayout
from kivy.uix.popup import Popup

# Local libraries
import dataProcessing
import imageProcessing
import statvar
from port import MySerial

# ignore multi-click on UI
# from src import mat

Config.set('input', 'mouse', 'mouse, multitouch_on_demand')


class LoadDialog(FloatLayout):
    load = ObjectProperty(None)
    cancel = ObjectProperty(None)


class MyLayout(BoxLayout):
    on = ObjectProperty(None)
    send = ObjectProperty(None)
    image = ObjectProperty(None)
    pattern_image = ObjectProperty(None)

    data = []  # Data Array

    is_on = False  # Printer ON/OFF
    is_sending = False  # Sending data y/n
    line_index = 0  # index of the data line that the printer is printing
    option = None  # Option that printer currently have
    loaded_pattern = False  # If an Image is already loaded
    loaded_pattern_bin = None  # Loaded binary image
    image_counter = 0  # Index of the saved image
    state = False

    global cap, blob, rect, mySerial
    cap = cv2.VideoCapture(0)
    mySerial = MySerial()
    blob = False  # If we have blob or not
    rect = [(0, 0, 0, 0)]  # List of all blobs

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1200)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1600)

    # UI Components

    def turn_on(self):
        self.on.background_color = (0, 1, 0, 1)
        self.on.text = 'ON'
        self.is_on = True
        self.send.disabled = False
        print("Printer is ON")
        mySerial.turn_on()

    def turn_off(self):
        self.not_sending()
        self.on.background_color = (1, 0, 0, 1)
        self.on.text = 'OFF'
        self.send.text = 'Send Data'
        self.send.disabled = True
        print("Printer is OFF")
        mySerial.turn_off()
        time.sleep(.5)
        self.is_on = False

    def sending(self):
        self.is_sending = True
        self.send.background_color = (0, 1, 0, 1)
        self.send.text = 'Sending Data...'
        print("Printer is sending data...")

    def not_sending(self):
        self.is_sending = False
        self.send.background_color = (1, 0, 0, 1)
        self.send.text = 'Stopped! Click to resume.'
        print("Printer stopped sending data!")

    def toggle_button(self):
        if self.on.state == 'down':
            self.turn_on()
        else:
            self.turn_off()

    def send_button(self):
        if self.send.state == 'down':
            self.sending()
        else:
            self.not_sending()

    def show_load_list(self):
        content = LoadDialog(load=self.load_list, cancel=self.dismiss_popup)
        self._popup = Popup(title="Load a file list", content=content, size_hint=(1, 1))
        self._popup.open()

    def load_list(self, path, filename):
        self.pattern_image.source = filename[0]
        if filename[0] != '':
            self.loaded_pattern = True

    def dismiss_popup(self):
        self._popup.dismiss()
        self.pattern_image.source = ""
        self.loaded_pattern = False

    def capture(self):
        self.image_counter += 1
        _, frame = cap.read()
        frame = cv2.rotate(frame, rotateCode=cv2.ROTATE_90_COUNTERCLOCKWISE)
        frame = frame[535:785, 100:500]  # 250*400

        frame = cv2.rotate(frame, rotateCode=cv2.ROTATE_90_COUNTERCLOCKWISE)
        frame = cv2.flip(frame, 0)
        path = f'../cap/{self.image_counter}.png'
        cv2.imwrite(path, frame)
        print(path)
        bin = dataProcessing.print_image(path)
        self.pattern_image.source = f'../cap/{self.image_counter}_bw.png'
        self.loaded_pattern = True
        self.loaded_pattern_bin = bin

        return frame, bin

    def load(self):
        content = LoadDialog(load=self.load_list, cancel=self.dismiss_popup)
        self._popup = Popup(title="Load a file list", content=content, size_hint=(1, 1))
        self._popup.open()

    def update_frame(self, arg):
        global blob, rect, mySerial, box
        ret, frame = cap.read()
        if ret:
            frame = cv2.rotate(frame, rotateCode=cv2.ROTATE_90_COUNTERCLOCKWISE)
            frame = frame[535:785, 100:500]
            frame = cv2.rotate(frame, rotateCode=cv2.ROTATE_180)
            rect, blob, frame = imageProcessing.processed_frame(frame)
            buf1 = cv2.flip(frame, 0)
            buf = buf1.tobytes()
            texture1 = Texture.create(size=(frame.shape[1], frame.shape[0]), colorfmt='bgr')
            texture1.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
            self.image.texture = texture1

    # ------------------------------------------------------------------------------------------------------------------------------

    def sending_data(self, arg):

        if self.is_on and self.is_sending and self.option is not None:

            options = self.option
            opt = options[0]

            if opt != 0:

                if opt == 1:  # Line

                    width = options[1]
                    pattern = options[2]

                    self.data = dataProcessing.simple_line(width, pattern, 2000)
                    self.option = [0]


                elif opt == 2:  # Image
                    self.data = dataProcessing.print_image("../res/Arrows.png")
                    self.option = [0]

                elif opt == 3:  # Scan/Print

                    if options[1] == 0:  # scan
                        _, bin = self.capture()
                        self.loaded_pattern_bin = bin
                        self.option = [0]

                    if options[1] == 1:  # print
                        self.data = self.loaded_pattern_bin
                        self.option = [0]

                elif opt == 4:  # Angle

                    if options[1] == 0:
                        self.data = dataProcessing.angle(statvar.interval, 270)
                        self.option = [0]

                    elif options[1] == 1:
                        self.data = dataProcessing.angle(statvar.interval, 90)
                        self.option = [0]

                    elif options[1] == 2:
                        self.data = dataProcessing.angle(statvar.interval, 315)
                        self.option = [0]

                    elif options[1] == 3:
                        self.data = dataProcessing.angle(statvar.interval, 45)
                        self.option = [0]

                    elif options[1] == 4:
                        self.data = dataProcessing.angle(statvar.interval, 225)
                        self.option = [0]

                    elif options[1] == 5:
                        self.data = dataProcessing.angle(statvar.interval, 300)
                        self.option = [0]

                elif opt == 5:  # Contact pad
                    self.data = dataProcessing.pad(statvar.interval)
                    self.option = [0]

                elif opt == 6 and blob:  # Connect to start-point (curley,simple)
                    self.data = dataProcessing.connect_to_startpoint(rect)
                    # self.data = dataProcessing.connect_to_start_point_curly(rect)
                    self.option = [0]

                elif opt == 7 and blob:  # Connect to end-point
                    self.data = dataProcessing.connect_to_endpoint(rect, statvar.interval)
                    self.option = [0]

                elif opt == 8 and blob:  # Route around the blob
                    self.data = dataProcessing.route_around(rect, statvar.interval)
                    self.option = [0]

                elif opt == 9 and blob:  # Continue the trace
                    self.data = dataProcessing.continue_trace(rect)
                    self.option = [0]

                elif opt == 11 and blob:  # parallel
                    self.data = dataProcessing.parallel_line(rect)
                    self.option = [0]

                elif opt == 12 and blob:  # Stop and don't continue :)
                    self.data = dataProcessing.stop_and_dont_continue(rect, statvar.interval)
                    self.option = [0]

                elif opt == 14:  # Resistor
                    self.data = dataProcessing.resistor(1)
                    self.option = [0]

                elif opt == 15:  # LED
                    self.data = dataProcessing.LED()
                    self.option = [0]

            # Check if we reached to the end of the data array or user canceled the printing
            data_arr_length = len(self.data)

            if data_arr_length == self.line_index + 1 or mySerial.canceled or self.data == []:
                self.data = []
                self.line_index = 0
                self.option = None
                mySerial.canceled = False

            # Sending one line of the data in each loop if we have data in data array
            else:
                data_line = self.data[self.line_index]
                statvar.last_line = data_line
                statvar.interval = dataProcessing.get_interval(data_line)
                mySerial.send_data(data_line, blob)
                self.line_index += 1
                print(self.line_index)

    def options(self, arg):  # Get the option from user frequently while we do not have any options
        if self.option is None:
            opt = mySerial.which_data()
            self.option = opt

    def blob_status(self, arg):  # Check if we have any blobs or not while the printer is OFF
        # print('state:  ', self.state)
        if not self.is_sending:
            if not self.is_on:
                if self.data == [] or mySerial.canceled is False:
                    mySerial.ser.write(b'B')
                    if blob:  # Sending 1 if we had a blob
                        mySerial.ser.write(b'1')
                        # print("blob + off")
                        # print("B1")
                    else:  # Sending 0 if we had a blob
                        mySerial.ser.write(b'0')
                        # print("no blob + off")

                        # print("B0")
            else:
                if self.data == [] or mySerial.canceled is False:
                    if blob and not self.state:  # Sending 2 if we have a blob
                        rand_data = dataProcessing.simple_line(1, 1, 1)[0]
                        mySerial.ser.write(rand_data)
                        mySerial.ser.write(b'\x02')
                        # print("blob + on")
                        # print(2)
                        self.state = True
                    if not blob and self.state:  # Sending 3 if we didn't have a blob
                        rand_data = dataProcessing.simple_line(1, 1, 1)[0]
                        mySerial.ser.write(rand_data)
                        mySerial.ser.write(b'\x03')
                        # print("not blob + on")
                        # print(3)
                        self.state = False


class Printer(App):
    def build(self):
        layout = MyLayout()
        Clock.schedule_interval(layout.update_frame, 1 / 30)  # updating the frame
        Clock.schedule_interval(layout.sending_data, 1 / 1000)  # sending data through port
        Clock.schedule_interval(layout.options, 1 / 30)  # updating the option
        Clock.schedule_interval(layout.blob_status, 1/30)  # checking the blob
        return layout


if __name__ == '__main__':
    Printer().run()
