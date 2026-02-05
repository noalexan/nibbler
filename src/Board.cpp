#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <nibbler.hpp>

#ifndef NPATHFINDING
# include <queue>
#endif

Board::Board() : Board(DEFAULT_WIDTH, DEFAULT_HEIGHT) {}

Board::Board(size_t width, size_t height)
    : Board(width, height, DEFAULT_GREEN_APPLES, DEFAULT_RED_APPLES)
{
}

Board::Board(size_t width, size_t height, unsigned int how_many_green_apples,
             unsigned int how_many_red_apples)
    : Board(width, height, how_many_green_apples, how_many_red_apples, DEFAULT_SNAKE_SIZE)
{
}

Board::Board(size_t width, size_t height, unsigned int how_many_green_apples,
             unsigned int how_many_red_apples, unsigned int base_snake_size)
{
	_width                 = width;
	_height                = height;
	_how_many_green_apples = how_many_green_apples;
	_how_many_red_apples   = how_many_red_apples;

	assert((base_snake_size * 2) <= _width && _width <= 256);
	assert((base_snake_size * 2) <= _height && _height <= 256);

	srand(time(0));

	_board.resize(_width * _height, TileTypes::Empty);

	for (size_t y = 0; y < _height; y++)
		for (size_t x = 0; x < _width; x++)
			if (y == 0 || y == _height - 1 || x == 0 || x == _width - 1)
				_board[y * _width + x] = TileTypes::Wall;

	_snake = std::make_unique<Snake>(*this, base_snake_size);
}

Board::~Board() {}

void Board::spawnTile(enum TileTypes tile)
{
	std::vector<std::reference_wrapper<enum TileTypes>> empty_tiles_refs;
	empty_tiles_refs.reserve(_board.size());

	for (auto &i : _board)
		if (i == TileTypes::Empty)
			empty_tiles_refs.push_back(i);

	if (!empty_tiles_refs.empty())
		empty_tiles_refs[rand() % empty_tiles_refs.size()].get() = tile;
}

#ifndef NPATHFINDING
template <typename T, typename... Args> static bool is_any_of(const T &val, Args... args)
{
	return ((val == args) || ...);
}
#endif

void Board::update()
{
#ifndef NPATHFINDING
	for (auto &i : _board)
		if (i == TileTypes::Way)
			i = TileTypes::Empty;
#endif

	_snake->update();

	for (unsigned int count = std::count(_board.begin(), _board.end(), TileTypes::GreenApple);
	     count < _how_many_green_apples; count++)
		spawnTile(TileTypes::GreenApple);

	for (unsigned int count = std::count(_board.begin(), _board.end(), TileTypes::RedApple);
	     count < _how_many_red_apples; count++)
		spawnTile(TileTypes::RedApple);

#ifndef NPATHFINDING
	constexpr int dx[] = {0, 0, -1, 1};
	constexpr int dy[] = {-1, 1, 0, 0};

	constexpr Coordinates unvisited = {-1, -1}, obstacle = {-2, -2};

	const Coordinates                    &head = _snake->getHead();
	std::vector<std::vector<Coordinates>> parent(_height,
	                                             std::vector<Coordinates>(_width, unvisited));

	for (const auto &part : _snake->getBlocks()) {
		const Coordinates &coord = part->getCoordinates();
		if (coord != head) {
			parent[coord.second][coord.first] = {-2, -2};
		}
	}

	std::queue<Coordinates> queue;
	queue.push(head);
	parent[head.second][head.first] = head;

	Coordinates foundApple = unvisited;
	bool        success    = false;

	do {
		Coordinates current = queue.front();
		queue.pop();

		if (at(current) == TileTypes::GreenApple) {
			foundApple = current;
			success    = true;
		}

		for (int i = 0; i < 4; i++) {
			int nx = current.first + dx[i];
			int ny = current.second + dy[i];

			if (0 <= nx && nx < _width && 0 <= nx && ny < _height) {
				if (parent[ny][nx] == unvisited) {
					if (is_any_of(at(nx, ny), TileTypes::Wall, TileTypes::SnakeBody,
					              TileTypes::RedApple)) {
						parent[ny][nx] = obstacle;
					} else {
						parent[ny][nx] = current;
						queue.push({nx, ny});
					}
				}
			}
		}
	} while (!success && !queue.empty());

	if (success) {
		for (Coordinates i = parent[foundApple.second][foundApple.first]; i != head;
		     i             = parent[i.second][i.first]) {
			getTile(i) = TileTypes::Way;
		}
	}

	if (is_any_of(at(head.first, head.second - 1), TileTypes::Way, TileTypes::GreenApple))
		_snake->changeDirection(SnakeDirections::Up);
	else if (is_any_of(at(head.first, head.second + 1), TileTypes::Way, TileTypes::GreenApple))
		_snake->changeDirection(SnakeDirections::Down);
	else if (is_any_of(at(head.first - 1, head.second), TileTypes::Way, TileTypes::GreenApple))
		_snake->changeDirection(SnakeDirections::Left);
	else if (is_any_of(at(head.first + 1, head.second), TileTypes::Way, TileTypes::GreenApple))
		_snake->changeDirection(SnakeDirections::Right);
#endif
}
