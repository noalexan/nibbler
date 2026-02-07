#include <nibbler.hpp>

Coordinates operator+(const Coordinates &a, const Coordinates &b)
{
	return {a.first + b.first, a.second + b.second};
}

Coordinates &operator+=(Coordinates &a, const Coordinates &b)
{
	a.first  += b.first;
	a.second += b.second;
	return a;
}

Board::Snake::Snake(Board *board) : Snake(board, DEFAULT_SNAKE_SIZE) {}

Board::Snake::Snake(Board *board, unsigned int base_snake_size)
    : _board(board), _base_snake_size(base_snake_size)
{
}

void Board::Snake::init()
{
	Coordinates base_snake_head = {_board->randomInt(_board->getWidth() - 2) + 1,
	                               _board->randomInt(_board->getHeight() - 2) + 1};

	std::vector<int> valid_directions;
	if (base_snake_head.first > _base_snake_size - 1)
		valid_directions.push_back(SnakeDirections::Left);
	if (base_snake_head.first + _base_snake_size - 1 < _board->getWidth() - 1)
		valid_directions.push_back(SnakeDirections::Right);
	if (base_snake_head.second > _base_snake_size - 1)
		valid_directions.push_back(SnakeDirections::Up);
	if (base_snake_head.second + _base_snake_size - 1 < _board->getHeight() - 1)
		valid_directions.push_back(SnakeDirections::Down);

	int idx              = valid_directions[_board->randomInt(valid_directions.size())];
	_snakeDirection      = static_cast<enum SnakeDirections>(idx ^ 1);
	_snakeDirectionDelay = _snakeDirection;

	for (unsigned int i = _base_snake_size; i--; base_snake_head += directions_vectors[idx]) {
		_board->getTile(base_snake_head) = TileTypes::SnakeBody;
		_tail.push_back(base_snake_head);
	}
}

Board::Snake::~Snake() {}

void Board::Snake::changeDirection(enum SnakeDirections direction)
{
	if ((_snakeDirection & 2) ^ (direction & 2))
		_snakeDirectionDelay = direction;
}

void Board::Snake::update()
{
	_snakeDirection   = _snakeDirectionDelay;
	Coordinates coord = getHead() + directions_vectors[_snakeDirection];

	switch (_board->getTile(coord)) {
	case TileTypes::Wall:
	case TileTypes::SnakeBody:
		die();
		break;

	case TileTypes::RedApple:
		if (_tail.size() & ~1) {
			_board->getTile(_tail.back()) = TileTypes::Empty;
			_tail.pop_back();
		} else {
			die();
			break;
		}

	default:
		_board->getTile(_tail.back()) = TileTypes::Empty;
		_tail.pop_back();

	case TileTypes::GreenApple:
		_board->getTile(coord) = TileTypes::SnakeBody;
		_tail.push_front(coord);
		break;
	}
}
