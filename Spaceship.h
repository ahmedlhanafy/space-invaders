class Spaceship {
public:
  Coordinates* coordinates;
  Rotation* rotation;
  std::vector<Coordinates> bullets;
    Spaceship(float tx, float ty, float tz, float angle, float rx, float ry, float rz);
};

Spaceship::Spaceship(float tx, float ty, float tz, float angle, float rx, float ry, float rz) {
    coordinates = new Coordinates(tx, ty, tz);
    rotation = new Rotation(angle, rx, ry, rz);
}
