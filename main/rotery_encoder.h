class RoteryEncoder : public IUpdate {
    uint8_t clk_;
    int lastState_;
    int counter_ = 0;
public :
    RoteryEncoder(uint8_t clk, uint8_t dt)
        :clk_(clk)
     {
        pinMode(clk, INPUT);
        pinMode(dt, INPUT);
        lastState_ = digitalRead(11);
    }

    void update() override {
        int state = digitalRead(11);
        if (state != lastState_) {
            if (digitalRead(12) != state) {
                rot.counter++;
            } else {
                rot.counter--;
            }

            Serial.print("Position: ");
            Serial.println(rot.counter);
        }

        lastState_ = state;
    }
};