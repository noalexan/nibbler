#include <chrono>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <nibbler/Board.hpp>
#include <nibbler/init.hpp>
#include <thread>
#include <unistd.h>

class NibblerABI {
private:
	void        *_linkedLibrary;
	const char  *_error;

	static void *load_library(const std::string &_library_path)
	{
		void       *linkedLibrary = dlopen(_library_path.c_str(), RTLD_LAZY);
		const char *error;

		if ((error = dlerror())) {
			throw UnableToLoadLibraryException(error);
		}

		return linkedLibrary;
	}

public:
	class UnableToLoadLibraryException : public std::runtime_error {
	public:
		UnableToLoadLibraryException(const char *_error) : runtime_error(_error) {}
	};

	class UnableToLoadFnException : public std::runtime_error {
	public:
		UnableToLoadFnException(const char *_error) : runtime_error(_error) {}
	};

	NibblerABI(const std::string &_library_path)
	    : _linkedLibrary(load_library(_library_path)),
	      init(load_fn<NibblerInitFn>(NIBBLER_INIT_FN_SYM)),
	      deinit(load_fn<NibblerDeinitFn>(NIBBLER_DEINIT_FN_SYM)),
	      get_gui(load_fn<NibblerGetGUIFn>(NIBBLER_GET_GUI_FN_SYM))
	{
	}

	~NibblerABI() { dlclose(_linkedLibrary); }

	NibblerInitFn           init;
	NibblerDeinitFn         deinit;
	NibblerGetGUIFn         get_gui;

	template <typename T> T load_fn(const char *symbol_name)
	{
		void *fn = dlsym(_linkedLibrary, symbol_name);

		if ((_error = dlerror())) {
			throw UnableToLoadFnException(_error);
		}

		return reinterpret_cast<T>(fn);
	}
};

void input_polling_loop(Board *board, NibblerGUI *gui)
{
	while (board->isStopped() == false && board->getSnake()->isDead() == false) {
		gui->poll_inputs();
	}
}

int main(int argc, char *argv[])
{
	size_t width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;

	int    opt;
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

	assert(optind == argc - 1); // Assert there is lib path after opts

	auto board = std::make_shared<Board>(width, height);

	/**/

	NibblerABI dflt(argv[optind]);

	dflt.init(board.get());

	NibblerGUI *gui = dflt.get_gui();

	std::thread inputPoller(input_polling_loop, board.get(), gui);

	constexpr auto        frame_time = std::chrono::milliseconds(1000 / 8); // ~8fps

	while (board->isStopped() == false && board->getSnake()->isDead() == false) {
		auto start_time = std::chrono::high_resolution_clock::now();

		gui->render();
		board->update();

		auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
		if (elapsed_time < frame_time) {
			std::this_thread::sleep_for(frame_time - elapsed_time);
		}

		elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
		std::cout << "\r\033[2Kfps: " << std::chrono::duration<double>(elapsed_time) / std::chrono::duration<double>(frame_time) * 8 << std::flush;
	}
	
	std::cout << "\n";
	std::this_thread::sleep_for(std::chrono::microseconds(30000));

	if (inputPoller.joinable()) {
		inputPoller.join();
	}

	dflt.deinit();

	/**/

	return 0;
}
