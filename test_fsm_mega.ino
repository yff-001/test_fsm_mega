#include "proto.h"

enum message_state_t {
    PARSE_START,
    PARSE_FIRST,
    PARSE_SECOND,
    PARSE_END
};

struct message_packet_t {
    char first;
    char second;
};

void setup() {
    Serial.begin(1000000);
    Serial3.begin(115200);

    uint8_t message[10] = {0};
    dtostrf(-4095/121.0, 5, 3, message);                 // the value could be an ADC reading
    if (message[0] != 0x2D) {                           // if first position is not '-'
        uint8_t temp = 0;
        for (uint8_t i=sizeof(message)/sizeof(message[0]);i>0;i--) {
            message[i] = message[i-1];                  // right shift entire string by one position
        }
        message[0] = 0x2B;                              // put '+' at first position
    } 
    strcat(message, "V");
    Serial.write(message, 10);

    // for (;;) {
    //     while (Serial3.available()) {
    //         char u = Serial3.read();
    //         parse_message(u);
    //     }
    // }
    
    for (auto i=33;i<127;i++) {
        Serial3.write(i);
        while (!Serial3.available());                   // block until response comes in
        char u = Serial3.read();                        //
        Serial.write(u);
        PORTB ^= (1<<PB7);
        delay(100);
    }
}

void loop() {
}

void parse_message(char payload) {
    static enum message_state_t state = PARSE_START;
    static struct message_packet_t packet = {'\0', '\0'};

    switch (state) {
        case PARSE_START:
            if (payload == STX) {
                state = PARSE_FIRST;
            }
            break;
        case PARSE_FIRST:
            if (payload == 'A' || payload == 'B') {
                packet.first = payload;
                state = PARSE_SECOND;
            }
            else {
                state = PARSE_START;
                packet.first = '\0';
                packet.second = '\0';
            }
            break;
        case PARSE_SECOND:
            if (payload == 'A' || payload == 'B') {
                packet.second = payload;
                state = PARSE_END;
            }
            else {
                state = PARSE_START;
                packet.first = '\0';
                packet.second = '\0';
            }
            break;
        case PARSE_END:
            if (payload == ETX) {
                // do something with packet
                Serial.write(packet.first);
                Serial.write(packet.second);
                Serial.write("\r\n");
                state = PARSE_START;
            }
            else {
                state = PARSE_START;
                packet.first = '\0';
                packet.second = '\0';
            }
            break;
        default:
            state = PARSE_START;
            packet.first = '\0';
            packet.second = '\0';
            break;
    }
}