#include <cstdlib>
#include <nibbler.hpp>

Board::Snake::SnakeBlock::SnakeBlock(Board &board, const Coordinates &_coord)
    : _coordinates(_coord), _tile(board.getTile(_coord))
{
	_tile = TileTypes::SnakeBody;
}

Board::Snake::SnakeBlock::~SnakeBlock() { _tile = TileTypes::Empty; }

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

Board::Snake::Snake(Board &board, unsigned int base_snake_size) : _board(board)
{
	Coordinates base_snake_head(rand() % (board.getWidth() - 2), rand() % (board.getHeight() - 2));
	const int   idx = rand() % 4;

	_snakeDirection      = static_cast<enum SnakeDirections>(idx ^ 1);
	_snakeDirectionDelay = _snakeDirection;

	for (; base_snake_size--; base_snake_head += directions_vectors[idx])
		_snakeBlocks.push_back(std::make_unique<SnakeBlock>(_board, base_snake_head));
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

	switch (_board.getTile(coord)) {
	case TileTypes::Wall:
	case TileTypes::SnakeBody:
		die();
		break;

	case TileTypes::RedApple:
		if (_snakeBlocks.size() & ~1) {
			_snakeBlocks.pop_back();
		} else {
			die();
			break;
		}

	default:
		_snakeBlocks.pop_back();

	case TileTypes::GreenApple:
		_snakeBlocks.push_front(std::make_unique<SnakeBlock>(_board, coord));
		break;
	}
}
