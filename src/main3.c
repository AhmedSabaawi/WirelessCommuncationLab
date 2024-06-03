#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "ADC.h"



#ifndef F_CPU
#define F_CPU 16000000UL // Set CPU frequency to 16 MHz
#endif // F_CPU
#ifndef BAUD
#define BAUD 9600 // Set baud rate to 4800 bps
#define Zigbee_BAUD 9600
#endif // !BAUD
#define bluetooth_MYUBRR ((F_CPU / 16 / BAUD) - 1)
#define Zigbee_MYUBRR ((F_CPU / 16 / Zigbee_BAUD) - 1)



#define MAX_FRAME_LENGTH 512  // Define max expected frame length, adjust as necessary

volatile uint8_t buffer[MAX_FRAME_LENGTH];

volatile uint8_t bluetoothBuffer[MAX_FRAME_LENGTH];

// recive data for bluetooth

volatile uint8_t  BluetoothIsrevingData = 0;
volatile uint8_t  BluetoothBufferIndex = 0;
volatile uint8_t  BluetoothUnreadData = 0;

// index for the buffer
volatile int bufferIndex = 0;

// flag for unread data
volatile uint8_t unreadData = 0;
// flag for start add data to buffer
volatile uint8_t startAddData = 0;


#define TX_PIN PB2 // Pin 10

void softSerialInit(void) {
    // Set TX pin as output
    DDRB |= (1 << TX_PIN);

}

void softSerialTransmit(char data) {
    // Transmit start bit
    PORTB &= ~(1 << TX_PIN);
    _delay_us(104); // Assuming 9600 baud rate

    // Transmit data
    for (int i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            PORTB |= (1 << TX_PIN);
        }
        else {
            PORTB &= ~(1 << TX_PIN);
        }
        _delay_us(104); // Assuming 9600 baud rate
    }

    // Transmit stop bit
    PORTB |= (1 << TX_PIN);
    _delay_us(104); // Assuming 9600 baud rate
}


void USART_Init(unsigned int ubrr) {
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Enable receiver, transmitter and RX complete interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data, no parity, 1 stop bit
    sei(); // Enable global interrupts
}


void USART_Transmit(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = data; // Send data
}

uint8_t USART_Receive(void) {
    while (!(UCSR0A & (1 << RXC0))); // Wait for data to be received
    return UDR0; // Get and return received data from buffer
}

ISR(USART_RX_vect) {
    // Read the received data
    uint8_t receivedData = USART_Receive();

    // print the received
    // USART_Transmit(receivedData);

    // New functionality to handle messages starting with 0x04 and ending with 0x05
    if (receivedData == 0x04) {
        BluetoothIsrevingData = 1;
        BluetoothBufferIndex = 0; // Reset buffer index to start fresh
        return;
    }
    else if (BluetoothIsrevingData) {
        if (receivedData == 0x05) {
            BluetoothIsrevingData = 0;
            BluetoothUnreadData = 1;
        }
        else {
            bluetoothBuffer[BluetoothBufferIndex++] = receivedData;
        }


    }




    // // check if ther iis unread data do nothing
    if (unreadData) {
        return;
    }
    // check if the received data is the start delimiter reset the buffer
    if (receivedData == 0x7E) {
        bufferIndex = 0;
        return;
    }
    if (receivedData == 0x02) {
        startAddData = 1;
        return;
    }
    if (startAddData && receivedData != 0x03) {
        buffer[bufferIndex++] = receivedData;

    }
    else if (receivedData == 0x03) {
        startAddData = 0;
        unreadData = 1;
    }




}


void USART_PrintString(const char* str) {
    while (*str) {
        USART_Transmit(*str++);
    }
}

uint8_t hexCharToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    return 0; // Not a valid hex digit
}

void hexStringToBytes(const char* hexString, uint8_t* bytes) {
    while (*hexString) {
        *(bytes++) = hexCharToByte(*hexString) << 4 | hexCharToByte(*(hexString + 1));
        hexString += 2;
    }
}

void USART_SendFrame(const char* data, const char* receiver) {
    const uint8_t header[] = {
        0x10, // Frame type: Transmit Request
        0x01, // Frame ID (set to 0 if no ACK is needed)
    };
    uint8_t receiverBytes[8];
    hexStringToBytes(receiver, receiverBytes);

    const uint8_t footer[] = {
        0xFF, 0xFE, // 16-bit destination network address
        0x00, // Broadcast radius
        0x00,  // Options
    };

    USART_Transmit(0x7E); // Start delimiter


    // Calculate frame length (header length + receiver bytes + footer length + string length)
    uint8_t frameLength = sizeof(header) + sizeof(receiverBytes) + sizeof(footer) + strlen(data) + 2;

    // Length
    USART_Transmit((frameLength >> 8) & 0xFF); // Length high byte
    USART_Transmit(frameLength & 0xFF);        // Length low byte

    // Transmit header
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < sizeof(header); ++i) {
        USART_Transmit(header[i]);
        checksum += header[i];
    }

    // Transmit receiver address and update checksum
    for (uint8_t i = 0; i < sizeof(receiverBytes); ++i) {
        USART_Transmit(receiverBytes[i]);
        checksum += receiverBytes[i];
    }

    // Transmit footer
    for (uint8_t i = 0; i < sizeof(footer); ++i) {
        USART_Transmit(footer[i]);
        checksum += footer[i];
    }

    // send satrt text frame
    USART_Transmit(0x02);
    checksum += 0x02;
    // Transmit data and calculate checksum
    while (*data) {
        uint8_t c = *data++;
        USART_Transmit(c);
        checksum += c;
    }
    USART_Transmit(0x03);
    checksum += 0x03;
    // Final checksum
    USART_Transmit(0xFF - checksum);
}


void Soft_SendFrame(const char* data, const char* receiver) {
    const uint8_t header[] = {
        0x10, // Frame type: Transmit Request
        0x01, // Frame ID (set to 0 if no ACK is needed)
    };
    uint8_t receiverBytes[8];
    hexStringToBytes(receiver, receiverBytes);

    const uint8_t footer[] = {
        0xFF, 0xFE, // 16-bit destination network address
        0x00, // Broadcast radius
        0x00,  // Options
    };

    softSerialTransmit(0x7E); // Start delimiter


    // Calculate frame length (header length + receiver bytes + footer length + string length)
    uint8_t frameLength = sizeof(header) + sizeof(receiverBytes) + sizeof(footer) + strlen(data) + 2;

    // Length
    softSerialTransmit((frameLength >> 8) & 0xFF); // Length high byte
    softSerialTransmit(frameLength & 0xFF);        // Length low byte

    // Transmit header
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < sizeof(header); ++i) {
        softSerialTransmit(header[i]);
        checksum += header[i];
    }

    // Transmit receiver address and update checksum
    for (uint8_t i = 0; i < sizeof(receiverBytes); ++i) {
        softSerialTransmit(receiverBytes[i]);
        checksum += receiverBytes[i];
    }

    // Transmit footer
    for (uint8_t i = 0; i < sizeof(footer); ++i) {
        softSerialTransmit(footer[i]);
        checksum += footer[i];
    }

    // send satrt text frame
    softSerialTransmit(0x02);
    checksum += 0x02;
    // Transmit data and calculate checksum
    while (*data) {
        uint8_t c = *data++;
        softSerialTransmit(c);
        checksum += c;
    }
    softSerialTransmit(0x03);
    checksum += 0x03;
    // Final checksum
    softSerialTransmit(0xFF - checksum);
}

// void pinsOutPut() {
//     // make pin 6 as output
//     DDRD |= (1 << PD6);
// }

// // procssing the bluetooth buffer data
// void processBluetoothData() {
//     // check the received data from the bluetooth
//     // if the received data is A toggle the led on pin 6
//     if (bluetoothBuffer[0] == 0x41) {
//         PORTD ^= (1 << PD6); // Toggle LED on pin 6
//     }
//     // if the received data is B read the ADC value
//     else if (bluetoothBuffer[0] == 0x42) {
//         // Read the ADC value from pin A2
//         uint16_t adcValue = adc_read(ADC2D);
//         // Send the ADC value back to the sender
//         // normalize the adc value to be between 0 and 255 
//         uint_fast16_t normalizedValue = (adcValue * 255) / 1023;
//         // convert the normalized value to char
//         char adcValueChar[4];
//         itoa(normalizedValue, adcValueChar, 10);
//         // put the value in message "the value of the adc is "
//         char message[] = "the value of the adc is ";
//         // send the message
//         Soft_SendFrame(strcat(message, adcValueChar), masterMAc);
//     }
// }


// use different 


int main(void) {
    USART_Init(bluetooth_MYUBRR); // Initialize USART with the defined baud rate
    softSerialInit();
    // Initialize the ADC
    // adc_init();
    // void pinsOutPut();


    // Example message and receiver address
    const char message[] = "send from one";

    const  char receiverAddress[] = "0013A2004214B618"; // You can set this dynamically
    // const char receiverAddress[] = "0013A2004214B5CE"; // You can set this dynamically
    // 

    while (1) {


        if (BluetoothUnreadData) {
            for (int i = 0; i < BluetoothBufferIndex; ++i) {
                if (bluetoothBuffer[i] == 0x61) {
                    // USART_Init(Zigbee_MYUBRR); // Initialize USART with the defined baud rate
                    _delay_ms(10); // Wait for 1 second before sending the next message

                    Soft_SendFrame(message, receiverAddress);
                    // USART_SendFrame(message, receiverAddress); // Send the message
                    _delay_ms(10); // Wait for 1 second before sending the next message
                    // USART_Init(bluetooth_MYUBRR); // Initialize USART with the defined baud rate
                }
            }
            BluetoothUnreadData = 0;
        }


        //     // check if there is unread data
        //     if (unreadData) {
        //         // Print the received data
        //         for (int i = 0; i < bufferIndex; ++i) {
        //             USART_Transmit(buffer[i]);
        //         }
        //         // USART_Transmit("\n");
        //         // Reset the unread data flag
        //         unreadData = 0;
        //     }
        // }
    }

    return 0;
}