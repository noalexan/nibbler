#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <memory>
#include <nibbler/init.hpp>

class NibblerSDL : public NibblerGUI {
private:
	SDL_Window           *_window;
	SDL_Renderer         *_renderer;
	SDL_Texture          *_texture;

	std::vector<uint32_t> buffer;

public:
	NibblerSDL(Board *board) : NibblerGUI(board), buffer(_board->getWidth() * _board->getHeight())
	{
		_window   = SDL_CreateWindow("nibbler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500,
		                             500, SDL_WINDOW_SHOWN);
		_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
		_texture =
		    SDL_CreateTexture(_renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_XRGB8888,
		                      SDL_TEXTUREACCESS_STREAMING, _board->getWidth(), _board->getHeight());
	}

	~NibblerSDL()
	{
		SDL_DestroyTexture(_texture);
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
	}

	void poll_inputs()
	{
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				_board->stop();
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					_board->getSnake()->changeDirection(SnakeDirections::Up);
					break;

				case SDL_SCANCODE_A:
					_board->getSnake()->changeDirection(SnakeDirections::Left);
					break;

				case SDL_SCANCODE_S:
					_board->getSnake()->changeDirection(SnakeDirections::Down);
					break;

				case SDL_SCANCODE_D:
					_board->getSnake()->changeDirection(SnakeDirections::Right);
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
	}

	void render()
	{
		const enum TileTypes *raw = _board->getRawBoard();

		for (uint32_t &value : buffer) {
			switch (*(raw++)) {
			case TileTypes::Empty:
				value = 0xFFa1a1a1;
				break;
			case TileTypes::GreenApple:
				value = 0xFF12dd12;
				break;
			case TileTypes::RedApple:
				value = 0xFFdd1212;
				break;
			case TileTypes::Snake:
				value = 0xFFAA6666;
				break;
			case TileTypes::Wall:
				value = 0xFFFFFFFF;
				break;
			}
		}

		SDL_UpdateTexture(_texture, nullptr, buffer.data(), _board->getWidth() * sizeof(uint32_t));
		SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
		SDL_RenderPresent(_renderer);
	}
};

std::shared_ptr<NibblerSDL> gui;

nibbler_init
{
	SDL_Init(SDL_INIT_VIDEO);
	gui = std::make_shared<NibblerSDL>(_board);
}

nibbler_deinit
{
	gui.reset();
	SDL_Quit();
}

nibbler_get_gui { return gui.get(); }
