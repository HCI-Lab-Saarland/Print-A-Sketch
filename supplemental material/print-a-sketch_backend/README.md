
# Print-A-Sketch (backend)

## Overview

This is the backend interface of [Print-A-Sketch]([https://hci.cs.uni-saarland.de/wp-content/uploads/projects/digital_fabrication_technologies/Print-A-Sketch/Pourjafarian_CHI22_Print-A-Sketch.pdf]). 
It serves as a link to the Arduino microcontroller by using a serial protocol to control the printhead, analyze the sensor data, and support adaptive printing. 

## Requirements
Assuming your hardware setup is working by following the instructions here in this [repository](https://github.com/HCI-Lab-Saarland/Print-A-Sketch) and the implementation section in the [paper](([https://hci.cs.uni-saarland.de/wp-content/uploads/projects/digital_fabrication_technologies/Print-A-Sketch/Pourjafarian_CHI22_Print-A-Sketch.pdf])), you are ready to install dependencies of the software. 

All dependencies are in  `Requirements.txt` file and you can install them by the following command:

```shell
pip install -r requirements.txt
```
## Project file description
- **`dataProcessing.py`** - where all data preparation and processing happens, e.g. connect to start point, connect to the endpoint, parallel line, etc.  
- **`imageProcessing.py`** - where all image processing happens and prepare blob data in real-time.
- **`main.py`** - where camera view is and the communication between software backened and the printer UI updates and the status of printer get recieved.
- **`port.py`** - where the port gets initialized and the data transfering and acknoledgment recieving happenes.
- **`printer.kv`**  - where the camera view with blob contour and printer primary states (On/Off, Sending data/Not sending data) are shown.
## Assets
Some printing samples can be accessed in the `res` folder.





