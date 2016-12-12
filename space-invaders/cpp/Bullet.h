class Bullet {
public:
    Coordinates* coordinates;
	Rotation* rotation;
    bool isAirborne;
    Bullet(bool isAirborne, float x, float y, float z);
};

Bullet::Bullet(bool isAirborne, float x, float y, float z) {
  this->coordinates = new Coordinates(x, y, z);
  this->rotation = new Rotation(0, 0, 0, 0);
  this->isAirborne = isAirborne;
}
