class Biquad{

public:

    Biquad(float a0, float a1, float a2, float b0, float b1, float b2) {
        this->a0 = a0;
        this->a1 = a1;
        this->a2 = a2;
        this->b0 = b0;
        this->b1 = b1;
        this->b2 = b2;
        z1 = z2 = 0.0;
    }

    float filter(float in) {
        float out = z1 + b0 * in;
        z1 = z2 + b1 * in - a1 * out;
        z2 = b2 * in - a2 * out;
        return out;
    }

private:
    float a0, a1, a2, b0, b1, b2;
    float z1, z2;
};