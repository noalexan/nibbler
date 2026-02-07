#pragma once

#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <random>
#include <unistd.h>
#include <vector>

#define DEFAULT_HEIGHT       10
#define DEFAULT_WIDTH        10
#define DEFAULT_SNAKE_SIZE   3
#define DEFAULT_GREEN_APPLES 2
#define DEFAULT_RED_APPLES   1

using Coordinates = std::pair<size_t, size_t>;

enum SnakeDirections : uint8_t { Up, Down, Left, Right };
enum TileTypes : uint8_t { Empty, Wall, GreenApple, RedApple, SnakeBody, Way };

class Board {
public:
	class Snake {
	private:
		Board *_board;
		friend class Board;

		enum SnakeDirections         _snakeDirection, _snakeDirectionDelay;
		constexpr static Coordinates directions_vectors[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

		unsigned int _base_snake_size;

		bool _isDead = false;

		inline void die() { _isDead = true; }

		std::deque<Coordinates> _tail;

	public:
		Snake(Board *board);
		Snake(Board *board, unsigned int base_snake_size);
		Snake(const Snake &) = default;

		virtual ~Snake();

		void init();

		inline bool isDead() const { return this->_isDead; }

		void changeDirection(enum SnakeDirections);

		inline size_t                         length() const { return _tail.size(); }
		inline const Coordinates             &getHead() const { return _tail.front(); }
		inline const std::deque<Coordinates> &getTail() const { return _tail; }

		void update();
	};

private:
	Snake _snake;

	std::vector<enum TileTypes> _board;

	std::mt19937_64 _rng;

	size_t _width;
	size_t _height;

	unsigned int _how_many_green_apples;
	unsigned int _how_many_red_apples;

	bool _stopped = false;

	inline size_t randomInt(size_t max)
	{
		std::uniform_int_distribution<size_t> dist(0, max - 1);
		return dist(_rng);
	}

	inline enum TileTypes &getTile(const Coordinates &coord)
	{
		return _board[coord.second * _width + coord.first];
	}

public:
	Board();
	Board(size_t width, size_t height);

	Board(size_t width, size_t height, unsigned int how_many_green_apples,
	      unsigned int how_many_red_apples);

	Board(size_t width, size_t height, unsigned int how_many_green_apples,
	      unsigned int how_many_red_apples, unsigned int base_snake_size);

	Board(const Board &);

	virtual ~Board();

	inline size_t                getWidth() const { return _width; }
	inline size_t                getHeight() const { return _height; }
	inline const enum TileTypes *getRawBoard() const { return _board.data(); }
	inline void                  stop() { _stopped = true; }
	inline bool                  isStopped() const { return _stopped; }

	inline const Snake &getSnake() const { return _snake; }
	inline Snake       &getSnake() { return _snake; }

	inline enum TileTypes at(int x, int y) const { return _board.at(y * _width + x); }
	inline enum TileTypes at(const Coordinates &coord) const
	{
		return at(coord.first, coord.second);
	}

	void spawnTile(enum TileTypes);

	void update();
};
