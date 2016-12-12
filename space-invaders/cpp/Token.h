class Token {
public:
    Coordinates* coordinates;
	Rotation* rotation;
    int type;
	bool isAirborne;
    Token(bool isAirborne, int type, float x, float y, float z);
};

Token::Token(bool isAirborne, int type, float x, float y, float z) {
  this->coordinates = new Coordinates(x, y, z);
  this->rotation = new Rotation(0, 0, 0, 0);
  this->type = type;
  this->isAirborne = isAirborne;
}
