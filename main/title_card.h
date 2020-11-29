#include <TM1637Display.h>

const constexpr uint8_t chess[] {
    SEG_A | SEG_F | SEG_E | SEG_D,
    SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
};


const constexpr uint8_t tmer[] {
    SEG_A | SEG_F | SEG_E,
    SEG_A | SEG_E | SEG_C,
    SEG_E | SEG_D,
    SEG_G | SEG_E,
};

const constexpr uint8_t by[] {
    SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
    SEG_F | SEG_G | SEG_B | SEG_C | SEG_D,
    0,
    0
};


const constexpr uint8_t liam[] {
    SEG_F | SEG_E | SEG_D,
    SEG_F | SEG_E,
    SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_E | SEG_C
};


void showTitleCard(TM1637Display &leftDisplay, TM1637Display &rightDisplay) {
    leftDisplay.setSegments(chess);
    rightDisplay.setSegments(tmer);
    delay(2000);
    leftDisplay.setSegments(by);
    rightDisplay.setSegments(liam);
    delay(2000);


    leftDisplay.clear();
    rightDisplay.clear();
}