# Print-A-Sketch
This is an Arduino library for Print-A-Sketch, an open-source handheld printer prototype for sketching circuits and sensors. This library provides the basic functionality of the printer, which can be achieved solely with an Arduino. The functions provided by the addition of the RGB camera which require external processing on a PC are not included here, but we are open to questions for those interested to explore more.  
For more information on the project, visit our [website](https://hci.cs.uni-saarland.de/print-a-sketch/).

### Hardware

- 1x Xaar 128/80 W printhead
- 1x FFC to DIP adapter

Power Circuit:
- 1x 3.7V Li-Ion  rechargable battery
- 1x DC-DC Switching Boost Converter (>35V)
- 2x 1K resistor
- 2x 200 resistor
- 2x 1uF capacitor 50V
- 1x 220uF capacitor 50V
- 2x 1N4004 diode
- 2x 1N4740 diode
- 2x IRF9540 transistor
- 2x BC547 transistor

Positioning:
- 1x Optical Flow Sensor PMW3360

Interaction with the device:
- 1x Monochrome 0.96" 128x64 OLED Graphic Display
- 2x Push Button

The code can be modified for interacting with the device without the display interface.

### Documentation

- `print_line()` - is used for printing lines of indefinite length with chosen width
- `print_image()` - is used for printing an image which is a hardcoded bitmap in `pixel_data.h` 

Xaar 128 Guide to Operation - [Unoffical](https://d1.amobbs.com/bbs_upload782111/files_36/ourdev_619324HXZTX0.pdf)
