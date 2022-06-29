/* work with test.py in pyserial tutorial */

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
    DDRB |= (1 << PB7);

    Serial.begin(1000000);

    char sample_message[] = {0x02, 0x41, 0x42, 0x03};

    for (;;) {
        while (Serial.available()) {
            char u = Serial.read();
            parse_message(u);
        }

        /* check output on putty (all is good) */
        // Serial.write(sample_message);
        // delay(4000);

        /* self generated to check finite state machine */
        // static uint8_t i = 0;
        // parse_message(sample_message[i++]);
        // if (i > 3) {
        //     i = 0;
        // }
        // delay(1000);
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
                // blink();
            }
            break;
        case PARSE_FIRST:
            if (payload == 'A' || payload == 'B') {
                packet.first = payload;
                state = PARSE_SECOND;
                // blink();
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
                // blink();
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
                blink();
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

void blink() {
    PORTB ^= (1 << PB7);
}