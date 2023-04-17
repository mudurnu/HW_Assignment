#include <stdio.h>
#include <mosquitto.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char mBuffer[30];
int prevLen = 0;
struct mosquitto * mosq;


bool init(const char *client_id, const char * host, 
	int port, int keepalive) {
    mosquitto_lib_init();
    mosq = mosquitto_new(client_id, true, NULL);
    int rc = mosquitto_connect(mosq, host, port, keepalive);
    if(rc != MOSQ_ERR_SUCCESS) {
        printf("Client could not connect to broker! ");
        mosquitto_destroy(mosq);
        return false;
    }
    return true;
}

void publish(const char * topic, const char * payload) {
    mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, 0, false);
}

void closeMqtt() {
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

int openSerialPort(const char *device_path, int baudrate){

    int fd;
    struct termios options;

    // Open the UART port
    fd = open(device_path, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

// Clear the non-blocking flag, so the device will block on reads
    fcntl(fd, F_SETFL, 0);

    // Get the current options for the port
    tcgetattr(fd, &options);

    // Set the baud rate
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    // Set the data format to 8N1
    options.c_cflag &= ~PARENB;    // no parity
    options.c_cflag &= ~CSTOPB;    // 1 stop bit
    options.c_cflag &= ~CSIZE;     // clear data size bits
    options.c_cflag |= CS8;        // 8 data bits

    // Set the options for the port
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}


char* combineMessages(char str[]){

	int len = strlen(str);

	if(str[0] == '$' && str[1] == '$'){
		prevLen = len;
		for (int i = 0; i < len; i++) {
			mBuffer[i] = str[i];
		}
	}
    
	if(len >= 2){
		if(str[len-1] == '*' && str[len-2] == '*'){
			for (int i = prevLen; i < (len+prevLen); i++) {
				mBuffer[i] = str[i-prevLen];
			}
			mBuffer[len+prevLen] = '\0';
			return mBuffer;
		}
	}
	return "";
}


bool checkCommas(char* str) {
    int commaCount = 0;
    int strLen = strlen(str);

    for (int i = 0; i < strLen; i++) {
        if (str[i] == ',') {
            commaCount++;
        }
    }

    if (commaCount == 3) {
        return true;
    } else {
        return false;
    }
}


char* removeDigits(char* str) {
    int len = strlen(str);
    if (len <= 4) {
        return NULL;  // String too short to remove digits
    }

    char* newStr = malloc(len - 3);  // Allocate memory for new string
    if (newStr == NULL) {
        return NULL;  // Allocation failed
    }

    // Copy middle of string into new string
    strncpy(newStr, str + 2, len - 4);
    newStr[len - 4] = '\0';  // Null-terminate new string

    return newStr;
}




char** checkMessage(char* str){
    char** tokens = malloc(4 * sizeof(char*));
    bool res = checkCommas(str);
    
    char* strModified = removeDigits(str);
    
    if(res){
        int i=0;
        char* tok = strtok(strModified, ",");
        while (tok != NULL && i < 4) {
            tokens[i++] = tok;
            tok = strtok(NULL, ",");
        }

        for (i = 0; i < 4; i++) {
        	if(i == 0){
        		publish("SystemState",tokens[i]);
        	}else if(i == 1){
        		publish("LedState1", tokens[i]);
        	}else if(i == 2){
        		publish("LedState0", tokens[i]);
        	}else{
        		publish("ButtonPressCount", tokens[i]);
        	}
        }
        return tokens;
    }
    
    // Return NULL if checkCommas fails
    return NULL;
}





int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: ./main /dev/ttyACMx\n");
        return 1;
    }
    
    printf("Port: %s\n", argv[1]);
    
    
    const char *device_path = argv[1];
    int baud_rate = B115200;
    int sp = openSerialPort(device_path, baud_rate);
    
    const char *client_id = "publisher-test";
    const char *host = "localhost";
    int port = 1883;
    int keepalive = 60;

    if (!init(client_id, host, port, keepalive)) {
        return -1;
    }

	while(1){
	     // Read and write to the UART port
	    char buffer[30];
	    int n;
	    while (1) {
		n = read(sp, buffer, 30);
		if (n > 0) {
			//endcheck
			//bool res = check(buffer);
			
		    buffer[n] = '\0';
		    //printf("Received  :%s:\n", buffer);
		    char* res = combineMessages(buffer);
		   
		    if(strlen(res) > 0){
		    		
		    	 printf("FULL MES :%s:\n", res);
		    	 char** tokens = checkMessage(res);
		    }
		    
		}
		usleep(100000);
	    }
	}
	
	close(sp);
	closeMqtt();
	return 0;
}
