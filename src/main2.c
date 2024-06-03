// #include <avr/io.h>
// #include <util/delay.h>

// // Define pins for software serial
// #define RX_PIN PB1 // Pin 9
// #define TX_PIN PB2 // Pin 10

// // Function to initialize the software serial
// void softSerialInit(void) {
//     // Set TX pin as output
//     DDRB |= (1 << TX_PIN);
//     // Set RX pin as input
//     DDRB &= ~(1 << RX_PIN);
//     // Enable pull-up resistor on RX pin
//     PORTB |= (1 << RX_PIN);
// }

// // Function to transmit a character via software serial
// void softSerialTransmit(char data) {
//     // Transmit start bit
//     PORTB &= ~(1 << TX_PIN);
//     _delay_us(104); // Adjust timing for 9600 baud rate

//     // Transmit data bits
//     for (int i = 0; i < 8; i++) {
//         if (data & (1 << i)) {
//             PORTB |= (1 << TX_PIN);
//         }
//         else {
//             PORTB &= ~(1 << TX_PIN);
//         }
//         _delay_us(104); // Adjust timing for 9600 baud rate
//     }

//     // Transmit stop bit
//     PORTB |= (1 << TX_PIN);
//     _delay_us(104); // Adjust timing for 9600 baud rate
// }

// // Function to receive a character via software serial
// char softSerialReceive(void) {
//     // Wait for start bit
//     while (PINB & (1 << RX_PIN));
//     _delay_us(104 / 2); // Half bit delay

//     // Read data bits
//     char data = 0;
//     for (int i = 0; i < 8; i++) {
//         _delay_us(104); // Adjust timing for 9600 baud rate
//         if (PINB & (1 << RX_PIN)) {
//             data |= (1 << i);
//         }
//     }

//     // Wait for stop bit
//     _delay_us(104); // Adjust timing for 9600 baud rate

//     return data;
// }

// int main(void) {
//     // Initialize software serial
//     softSerialInit();

//     while (1) {
//         // Uncomment the following lines to test reception
//         // char received = softSerialReceive();
//         // softSerialTransmit(received); // Echo received data

//         // For demonstration, transmit 'A' continuously every second
//         softSerialTransmit('A');
//         _delay_ms(1000);
//     }
// }
