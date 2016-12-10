class Rotation {
public:
    float angle, x, y, z;
    Rotation(float angle, float x, float y, float z);
};

Rotation::Rotation(float angle, float x, float y, float z) {
    this->angle = angle;
    this->x = x;
    this->y = y;
    this->z = z;
}
