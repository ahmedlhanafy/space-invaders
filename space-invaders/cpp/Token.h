class Token {
public:
    Coordinates* coordinates;
	Rotation* rotation;
    int type;
    Token(int type, float x, float y, float z);
};

Token::Token(int type, float x, float y, float z) {
  this->coordinates = new Coordinates(x, y, z);
  this->rotation = new Rotation(0, 0, 0, 0);
  this->type = type;
}
