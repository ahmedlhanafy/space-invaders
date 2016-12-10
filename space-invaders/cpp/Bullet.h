class Bullet {
public:
    Coordinates* coordinates;
    bool isAirborne;
    Bullet(bool isAirborne, float x, float y, float z);
};

Bullet::Bullet(bool isAirborne, float x, float y, float z) {
  this->coordinates = new Coordinates(x, y, z);
  this->isAirborne = isAirborne;
}
