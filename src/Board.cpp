#include <algorithm>
#include <functional>
#include <iostream>
#include <nibbler.hpp>
#include <queue>
#include <random>

Board::Board(size_t width, size_t height) : _width(width), _height(height)
{
	assert((DEFAULT_SNAKE_SIZE * 2) <= width && width <= 256);
	assert((DEFAULT_SNAKE_SIZE * 2) <= height && height <= 256);

	_board.resize(width * height);

	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			_board[y * width + x] = (y == 0 || y == height - 1 || x == 0 || x == width - 1)
			                            ? TileTypes::Wall
			                            : TileTypes::Empty;

	size_t bes = sizeof(_board.front()); // Board Element Size
	size_t bs  = _board.size() * bes;    // Board Size

	std::cout << "Width: " << width << ", height: " << height << std::endl;
	std::cout << "Board size: " << bs / 1000 << "Kb (" << bs << ")" << std::endl;

	_snake = std::make_unique<Snake>(*this);
}

Board::~Board() { std::cout << "Goodbye." << std::endl; }

void Board::spawnTile(enum TileTypes tile)
{
	size_t empty_tiles_count = std::count(_board.begin(), _board.end(), TileTypes::Empty);

	if (empty_tiles_count) {
		std::vector<std::reference_wrapper<enum TileTypes>> empty_tiles_refs;
		empty_tiles_refs.reserve(empty_tiles_count);

		for (auto &i : _board) {
			if (i == TileTypes::Empty) {
				empty_tiles_refs.push_back(i);
			}
		}

		std::random_device              rd;
		std::mt19937                    gen(rd());

		std::uniform_int_distribution<> distr(0, empty_tiles_refs.size() - 1);

		int                             randomIndex = distr(gen);

		empty_tiles_refs[randomIndex].get()         = tile;
	}
}

template <typename T, typename... Args> bool is_any_of(const T &val, Args... args)
{
	return ((val == args) || ...);
}

void Board::update()
{
	for (auto &i : _board) {
		if (i == TileTypes::Way) {
			i = TileTypes::Empty;
		}
	}

	_snake->update();

	while (std::count(_board.begin(), _board.end(), TileTypes::GreenApple) < DEFAULT_GREEN_APPLES)
		spawnTile(TileTypes::GreenApple);

	while (std::count(_board.begin(), _board.end(), TileTypes::RedApple) < DEFAULT_RED_APPLES)
		spawnTile(TileTypes::RedApple);

	// Path finding
	constexpr int                         dx[]      = {0, 0, -1, 1};
	constexpr int                         dy[]      = {-1, 1, 0, 0};

	constexpr Coordinates                 unvisited = {-1, -1}, obstacle = {-2, -2};

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

	Coordinates foundApple          = unvisited;
	bool        success             = false;

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

	// if (is_any_of(at(head.first, head.second - 1), TileTypes::Way, TileTypes::GreenApple))
	// 	_snake->changeDirection(SnakeDirections::Up);
	// else if (is_any_of(at(head.first, head.second + 1), TileTypes::Way, TileTypes::GreenApple))
	// 	_snake->changeDirection(SnakeDirections::Down);
	// else if (is_any_of(at(head.first - 1, head.second), TileTypes::Way, TileTypes::GreenApple))
	// 	_snake->changeDirection(SnakeDirections::Left);
	// else if (is_any_of(at(head.first + 1, head.second), TileTypes::Way, TileTypes::GreenApple))
	// 	_snake->changeDirection(SnakeDirections::Right);
}
