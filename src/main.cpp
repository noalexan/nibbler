#include <iostream>
#include <unistd.h>

#define DEFAULT_HEIGHT 64
#define DEFAULT_WIDTH  64

int width  = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;

int main(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "w:h:")) != -1) {
		switch (opt) {
		case 'w':
			width = std::stoi(optarg);
			break;
		case 'h':
			height = std::stoi(optarg);
			break;
		default: /* '?' */
			std::cerr << std::endl
			          << "Usage: " << argv[0] << " -w <width> -h <height>" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	std::cout << "Width: " << width << ", height: " << height << std::endl;

	return 0;
}
