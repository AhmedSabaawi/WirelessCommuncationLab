// #include <avr/io.h>
// #include <util/delay.h>
// #include <string.h>
// #include <stdlib.h>
// #include <avr/interrupt.h>



// #ifndef F_CPU
// #define F_CPU 16000000UL // Set CPU frequency to 16 MHz
// #endif // F_CPU
// #ifndef BAUD
// #define BAUD 9600
// #endif // !BAUD

// #define MYUBRR ((F_CPU / 16 / BAUD) - 1)



// #define MAX_FRAME_LENGTH 512  // Define max expected frame length, adjust as necessary

// volatile uint8_t buffer[MAX_FRAME_LENGTH];
// // index for the buffer
// volatile int bufferIndex = 0;

// // flag for unread data
// volatile uint8_t unreadData = 0;
// // flag for start add data to buffer
// volatile uint8_t startAddData = 0;







// void USART_Init(unsigned int ubrr) {
//     UBRR0H = (uint8_t)(ubrr >> 8);
//     UBRR0L = (uint8_t)ubrr;
//     UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Enable receiver, transmitter and RX complete interrupt
//     UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data, no parity, 1 stop bit
//     sei(); // Enable global interrupts
// }


// void USART_Transmit(uint8_t data) {
//     while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
//     UDR0 = data; // Send data
// }

// uint8_t USART_Receive(void) {
//     while (!(UCSR0A & (1 << RXC0))); // Wait for data to be received
//     return UDR0; // Get and return received data from buffer
// }

// ISR(USART_RX_vect) {
//     // Read the received data
//     uint8_t receivedData = USART_Receive();

//     // check if ther iis unread data do nothing
//     if (unreadData) {
//         return;
//     }

//     // check if the received data is the start delimiter reset the buffer
//     if (receivedData == 0x7E) {
//         bufferIndex = 0;
//         return;
//     }

//     if (receivedData == 0x02) {
//         startAddData = 1;
//         return;
//     }
//     if (startAddData && receivedData != 0x03) {
//         buffer[bufferIndex++] = receivedData;

//     }
//     else if (receivedData == 0x03) {
//         startAddData = 0;
//         unreadData = 1;
//     }


// }


// void USART_PrintString(const char* str) {
//     while (*str) {
//         USART_Transmit(*str++);
//     }
// }

// uint8_t hexCharToByte(char c) {
//     if (c >= '0' && c <= '9') return c - '0';
//     if (c >= 'A' && c <= 'F') return 10 + c - 'A';
//     if (c >= 'a' && c <= 'f') return 10 + c - 'a';
//     return 0; // Not a valid hex digit
// }

// void hexStringToBytes(const char* hexString, uint8_t* bytes) {
//     while (*hexString) {
//         *(bytes++) = hexCharToByte(*hexString) << 4 | hexCharToByte(*(hexString + 1));
//         hexString += 2;
//     }
// }

// void USART_SendFrame(const char* data, const char* receiver) {
//     const uint8_t header[] = {
//         0x10, // Frame type: Transmit Request
//         0x01, // Frame ID (set to 0 if no ACK is needed)
//     };
//     uint8_t receiverBytes[8];
//     hexStringToBytes(receiver, receiverBytes);

//     const uint8_t footer[] = {
//         0xFF, 0xFE, // 16-bit destination network address
//         0x00, // Broadcast radius
//         0x00,  // Options
//     };

//     USART_Transmit(0x7E); // Start delimiter


//     // Calculate frame length (header length + receiver bytes + footer length + string length)
//     uint8_t frameLength = sizeof(header) + sizeof(receiverBytes) + sizeof(footer) + strlen(data) + 2;

//     // Length
//     USART_Transmit((frameLength >> 8) & 0xFF); // Length high byte
//     USART_Transmit(frameLength & 0xFF);        // Length low byte

//     // Transmit header
//     uint8_t checksum = 0;
//     for (uint8_t i = 0; i < sizeof(header); ++i) {
//         USART_Transmit(header[i]);
//         checksum += header[i];
//     }

//     // Transmit receiver address and update checksum
//     for (uint8_t i = 0; i < sizeof(receiverBytes); ++i) {
//         USART_Transmit(receiverBytes[i]);
//         checksum += receiverBytes[i];
//     }

//     // Transmit footer
//     for (uint8_t i = 0; i < sizeof(footer); ++i) {
//         USART_Transmit(footer[i]);
//         checksum += footer[i];
//     }

//     // send satrt text frame
//     USART_Transmit(0x02);
//     checksum += 0x02;
//     // Transmit data and calculate checksum
//     while (*data) {
//         uint8_t c = *data++;
//         USART_Transmit(c);
//         checksum += c;
//     }
//     USART_Transmit(0x03);
//     checksum += 0x03;
//     // Final checksum
//     USART_Transmit(0xFF - checksum);
// }



// int main(void) {
//     USART_Init(MYUBRR);

//     // Example message and receiver address
//     const char message[] = "send from one";

//     const  char receiverAddress[] = "0013A2004214B618"; // You can set this dynamically
//     // const char receiverAddress[] = "0013A2004214B5CE"; // You can set this dynamically
//     // 

//     while (1) {
//         USART_SendFrame(message, receiverAddress); // Send the message

//         _delay_ms(2000); // Wait for 1 second before sending the next message

//         // // check if there is unread data
//         // if (unreadData) {
//         //     // Print the received data
//         //     for (int i = 0; i < bufferIndex; ++i) {
//         //         USART_Transmit(buffer[i]);
//         //     }
//         //     // USART_Transmit("\n");
//         //     // Reset the unread data flag
//         //     unreadData = 0;
//         // }
//     }


//     return 0;
// }