#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <nibbler.hpp>
#include <thread>
#include <unistd.h>

void input_polling_loop(GLFWwindow *window, Board *board)
{
	while (board->isStopped() == false && board->getSnake()->isDead() == false) {
		glfwPollEvents();

		if (glfwWindowShouldClose(window)) {
			board->stop();
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			board->getSnake()->changeDirection(SnakeDirections::Up);
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			board->getSnake()->changeDirection(SnakeDirections::Left);
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			board->getSnake()->changeDirection(SnakeDirections::Down);
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			board->getSnake()->changeDirection(SnakeDirections::Right);
	}
}

int main(int argc, char *argv[])
{
	size_t width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;

	int opt;
	while ((opt = getopt(argc, argv, "w:h:")) != -1) {
		switch (opt) {
		case 'w':
			width = std::stoi(optarg);
			break;
		case 'h':
			height = std::stoi(optarg);
			break;
		default:
			std::cerr << "\nUsage: " << argv[0] << " [-w <width>] [-h <height>]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	Board *board = new Board(width, height);

	if (!glfwInit()) {
		std::cerr << "Failed to init GLFW" << std::endl;
	}

	GLFWwindow *window = glfwCreateWindow(500, 500, "nibbler", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	glViewport(0, 0, bufferWidth, bufferHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, board->getWidth(), board->getHeight(), 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/**/

	std::thread inputPoller(input_polling_loop, window, board);

	constexpr auto frame_time = std::chrono::milliseconds(1000 / 240); // ~240fps

	while (board->isStopped() == false && board->getSnake()->isDead() == false) {
		auto start_time = std::chrono::high_resolution_clock::now();

		board->update();

		glfwMakeContextCurrent(window);

		int bufferWidth, bufferHeight;
		glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
		glViewport(0, 0, bufferWidth, bufferHeight);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		const enum TileTypes *raw    = board->getRawBoard();
		size_t                width  = board->getWidth();
		size_t                height = board->getHeight();

		glBegin(GL_QUADS);
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				TileTypes tile = board->at(x, y);

				if (tile == TileTypes::Empty)
					continue;

				switch (tile) {
				case TileTypes::GreenApple:
					glColor3f(0.2f, 0.8f, 0.2f);
					break;
				case TileTypes::RedApple:
					glColor3f(0.8f, 0.2f, 0.2f);
					break;
				case TileTypes::SnakeBody:
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

		glfwSwapBuffers(window);

		auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
		if (elapsed_time < frame_time) {
			std::this_thread::sleep_for(frame_time - elapsed_time);
		}
	}

	if (inputPoller.joinable()) {
		inputPoller.join();
	}

	/**/

	delete board;

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
