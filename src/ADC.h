// #include <avr/io.h>

// #ifndef F_CPU // Check if F_CPU is not defined
// #define F_CPU 16000000UL // Define CPU frequency as 16MHz
// #endif
// #ifndef PRESCALER
// #define PRESCALER 4 // Define prescaler only if not previously defined
// #endif

// #define AREF REFS0 // Use AVCC with external capacitor at AREF pin

// // Function to set the ADC prescaler based on the defined PRESCALER value
// void set_prescaler(void) {
//     switch (PRESCALER) {
//     case 2:
//         ADCSRA |= (1 << ADPS0); // Set prescaler to 2
//         break;
//     case 4:
//         ADCSRA |= (1 << ADPS1); // Set prescaler to 4
//         break;
//     case 8:
//         ADCSRA |= (1 << ADPS1) | (1 << ADPS0); // Set prescaler to 8
//         break;
//     case 16:
//         ADCSRA |= (1 << ADPS2); // Set prescaler to 16
//         break;
//     case 32:
//         ADCSRA |= (1 << ADPS2) | (1 << ADPS0); // Set prescaler to 32
//         break;
//     case 64:
//         ADCSRA |= (1 << ADPS2) | (1 << ADPS1); // Set prescaler to 64
//         break;
//     case 128:
//         ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set prescaler to 128
//         break;
//     default:
//         // If an unsupported prescaler value is provided, do nothing
//         break;
//     }
// }

// // Initializes the ADC by setting the reference voltage and enabling the ADC
// void adc_init() {
//     ADCSRA = (1 << ADEN); // Enable the ADC
//     ADMUX = (1 << AREF); // Set reference voltage to AVCC

//     // Disable digital input buffers on ADC0-ADC5 to save power
//     DIDR0 |= (1 << ADC0D) | (1 << ADC1D) | (1 << ADC3D) | (1 << ADC4D) | (1 << ADC5D);
//     set_prescaler(); // Set the prescaler value for the ADC
// }

// // Configures the ADMUX to read from the correct pin
// void pin_to_mux(uint8_t pin) {
//     ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0)); // Clear MUX bits

//     // Set the ADMUX MUX bits according to the desired ADC input pin
//     switch (pin) {
//     case ADC0D:
//         // No bits need to be set for ADC0
//         break;
//     case ADC1D:
//         ADMUX |= (1 << MUX0); // Set MUX to ADC1
//         break;
//     case ADC2D:
//         ADMUX |= (1 << MUX1); // Set MUX to ADC2
//         break;
//     case ADC3D:
//         ADMUX |= (1 << MUX1) | (1 << MUX0); // Set MUX to ADC3
//         break;
//     case ADC4D:
//         ADMUX |= (1 << MUX2); // Set MUX to ADC4
//         break;
//     case ADC5D:
//         ADMUX |= (1 << MUX2) | (1 << MUX0); // Set MUX to ADC5
//         break;
//     default:
//         // If an invalid pin is provided, do nothing
//         break;
//     }
// }

// // Starts an ADC conversion
// void conversion_init(void) {
//     ADCSRA |= (1 << ADSC); // Start an ADC conversion by setting the ADSC bit in ADCSRA
// }

// // Reads the ADC value from the specified pin
// uint16_t read_adc(uint8_t pin) {
//     pin_to_mux(pin); // Set the correct ADC input channel
//     conversion_init(); // Start the conversion

//     // Wait for the conversion to complete
//     while (ADCSRA & (1 << ADSC));

//     return ADC; // Return the ADC conversion result
// }
