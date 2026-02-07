#pragma once

#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
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
enum TileTypes : uint8_t {
	Empty,
	Wall,
	GreenApple,
	RedApple,
	SnakeBody,
#ifndef NPATHFINDING
	Way
#endif
};

class Board {
public:
	class Snake {
	private:
		Board &_board;

		enum SnakeDirections         _snakeDirection, _snakeDirectionDelay;
		constexpr static Coordinates directions_vectors[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

		bool _isDead = false;

		class SnakeBlock {
		private:
			enum TileTypes &_tile;
			Coordinates     _coordinates;

		public:
			SnakeBlock(Board &board, const Coordinates &_coord);
			virtual ~SnakeBlock();

			inline const Coordinates &getCoordinates() const { return this->_coordinates; }
			inline enum TileTypes     getTile() const { return this->_tile; }
		};

		inline void die() { _isDead = true; }

		std::deque<std::unique_ptr<SnakeBlock>> _snakeBlocks;

	public:
		Snake(Board &board);
		Snake(Board &board, unsigned int base_snake_size);

		virtual ~Snake();

		inline bool isDead() const { return this->_isDead; }

		void changeDirection(enum SnakeDirections);

		inline size_t             length() { return _snakeBlocks.size(); }
		inline const Coordinates &getHead() const { return _snakeBlocks.front()->getCoordinates(); }
		inline const std::deque<std::unique_ptr<SnakeBlock>> &getBlocks() const
		{
			return _snakeBlocks;
		}

		void update();
	};

private:
	std::unique_ptr<Snake> _snake;

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

	virtual ~Board();

	inline size_t                getWidth() const { return _width; }
	inline size_t                getHeight() const { return _height; }
	inline const enum TileTypes *getRawBoard() const { return _board.data(); }
	inline Snake                *getSnake() const { return _snake.get(); }
	inline void                  stop() { _stopped = true; }
	inline bool                  isStopped() const { return _stopped; }

	inline enum TileTypes at(int x, int y) const { return _board.at(y * _width + x); }
	inline enum TileTypes at(const Coordinates &coord) const
	{
		return at(coord.first, coord.second);
	}

	void spawnTile(enum TileTypes);

	void update();
};
