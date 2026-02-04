#include <nibbler.hpp>

Board::Snake::SnakeBlock::SnakeBlock(Board &board, const Coordinates &_coord)
    : _coordinates(_coord), _tile(board.getTile(_coord))
{
	_tile = TileTypes::SnakeBody;
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

void Board::Snake::update()
{
	Coordinates coord                                   = getHead();
	_snakeDirection                                     = _snakeDirectionDelay;
	(_snakeDirection & 2 ? coord.first : coord.second) += _snakeDirection & 1 ? 1 : -1;

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
