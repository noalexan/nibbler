#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <nibbler/init.hpp>
#include <vector>

class NibblerOpenGL : public NibblerGUI {
private:
	GLFWwindow *_window;

public:
	NibblerOpenGL(Board *board) : NibblerGUI(board)
	{
		_window = glfwCreateWindow(500, 500, "nibbler", NULL, NULL);
		if (!_window) {
			std::cerr << "Failed to create GLFW window" << std::endl;
			return;
		}

		glfwMakeContextCurrent(_window);

		int bufferWidth, bufferHeight;
		glfwGetFramebufferSize(_window, &bufferWidth, &bufferHeight);
		glViewport(0, 0, bufferWidth, bufferHeight);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, _board->getWidth(), _board->getHeight(), 0.0f, -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	~NibblerOpenGL() { glfwDestroyWindow(_window); }

	void poll_inputs() override
	{
		glfwPollEvents();

		if (glfwWindowShouldClose(_window)) {
			_board->stop();
		}

		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
			_board->getSnake()->changeDirection(SnakeDirections::Up);
		else if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
			_board->getSnake()->changeDirection(SnakeDirections::Left);
		else if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
			_board->getSnake()->changeDirection(SnakeDirections::Down);
		else if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
			_board->getSnake()->changeDirection(SnakeDirections::Right);
	}

	void render() override
	{
		glfwMakeContextCurrent(_window);

		int bufferWidth, bufferHeight;
		glfwGetFramebufferSize(_window, &bufferWidth, &bufferHeight);
		glViewport(0, 0, bufferWidth, bufferHeight);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		const enum TileTypes *raw    = _board->getRawBoard();
		size_t                width  = _board->getWidth();
		size_t                height = _board->getHeight();

		glBegin(GL_QUADS);
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				TileTypes tile = _board->at(x, y);

				if (tile == TileTypes::Empty)
					continue;

				switch (tile) {
				case TileTypes::GreenApple:
					glColor3f(0.2f, 0.8f, 0.2f);
					break;
				case TileTypes::RedApple:
					glColor3f(0.8f, 0.2f, 0.2f);
					break;
				case TileTypes::Snake:
					glColor3f(0.8f, 0.5f, 0.3f);
					break;
				case TileTypes::Way:
					glColor3f(0.1f, 0.15f, 0.2f);
					break;
				case TileTypes::Wall:
					glColor3f(0.9f, 0.9f, 0.9f);
					break;
				default:
					break;
				}

				glVertex2f(x, y);
				glVertex2f(x + 1, y);
				glVertex2f(x + 1, y + 1);
				glVertex2f(x, y + 1);
			}
		}
		glEnd();

		glfwSwapBuffers(_window);
	}
};

std::shared_ptr<NibblerOpenGL> gui;

nibbler_init
{
	if (!glfwInit()) {
		std::cerr << "Failed to init GLFW" << std::endl;
	}
	gui = std::make_shared<NibblerOpenGL>(_board);
}

nibbler_deinit
{
	gui.reset();
	glfwTerminate();
}

nibbler_get_gui { return gui.get(); }
