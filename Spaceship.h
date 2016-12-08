class Spaceship {
public:
  bool isHostile;
  Coordinates* coordinates;
  Rotation* rotation;
  std::vector<Bullet> bullets;
  int firingDelay;
    Spaceship(bool isHostile, float tx, float ty, float tz, float angle, float rx, float ry, float rz);
};

Spaceship::Spaceship(bool isHostile, float tx, float ty, float tz, float angle, float rx, float ry, float rz) {
    this->isHostile = isHostile;
    this->coordinates = new Coordinates(tx, ty, tz);
    this->rotation = new Rotation(angle, rx, ry, rz);
}
