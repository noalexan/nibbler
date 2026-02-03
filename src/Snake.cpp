#include <nibbler/Board.hpp>

Board::Snake::SnakeBlock::SnakeBlock(Board &board, const Coordinates &_coord)
    : _coordinates(_coord), _tile(board.getTile(_coord))
{
	_tile = TileTypes::Snake;
}

Board::Snake::SnakeBlock::~SnakeBlock() { _tile = TileTypes::Empty; }

Board::Snake::Snake(Board &board) : _board(board), _snakeDirectionDelay(_snakeDirection)
{
	Coordinates base_snake_head(board.getWidth() / 2, board.getHeight() / 2);
	for (int i = 0; i < DEFAULT_SNAKE_SIZE; i++, base_snake_head.first--)
		_snakeBlocks.push_back(std::make_unique<SnakeBlock>(_board, base_snake_head));
}

Board::Snake::~Snake() {}

void Board::Snake::changeDirection(enum SnakeDirections direction)
{
	if ((_snakeDirection & 2) ^ (direction & 2))
		_snakeDirectionDelay = direction;
}

void Board::Snake::feed() {
	_snakeBlocks.push_back(nullptr);
}

void Board::Snake::update()
{
	Coordinates coord = getHead();

	switch ((_snakeDirection = _snakeDirectionDelay)) {
	case SnakeDirections::Up:
		coord.second--;
		break;

	case SnakeDirections::Down:
		coord.second++;
		break;

	case SnakeDirections::Left:
		coord.first--;
		break;

	case SnakeDirections::Right:
		coord.first++;
		break;
	}

	switch (_board.getTile(coord)) {
	case TileTypes::GreenApple:
		feed();
		break;
	case TileTypes::RedApple:
		_snakeBlocks.pop_back();
		if (_snakeBlocks.size() == 0)
			_isDead = true;
		break;
	case TileTypes::Wall:
	case TileTypes::Snake:
		_isDead = true;
		break;
	default:
		break;
	}

	_snakeBlocks.push_front(std::make_unique<SnakeBlock>(_board, coord));
	_snakeBlocks.pop_back();
}
