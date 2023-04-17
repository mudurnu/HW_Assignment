#KAURTIS HW TECHNICAL ASSIGNMENT

Serial and Network Based Communication in Embedded Systems

#Installation of MQTT:

C libraries are downloaded from https://www.eclipse.org/paho/clients/c/ website and installed with 'make' command. Python libraries are installed via terminal by 'pip install paho-mqtt' command.

#Microcontroller:

STM32 Nucleo-144 microcontroller board is used for the assignment. Development ID is selected as STM32CubeIDE. The button and leds on the development board are used for the assignment, so there is no additional setup or circuit.

The serial port opened is 115200 baudrate, data bits, no parity bit, and 1 stop bit.

#MQTT Publisher:

The Publisher code is implemented in C with Makefile.


#MQTT Client:

The Client code is implemented in Python3.



#How to use:

1) write 'dmesg | grep ttyACM' to the terminal to find the port of the device. 


3) run mqtt client with 'python3 main.py'

4) run mqtt publisher with 'sudo ./main /dev/ttyACMx'. Please be careful that opening the UART port may require sudo permissions. Give the uart port as argument.
