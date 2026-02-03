#pragma once

#include "Board.hpp"

#if __has_attribute(visibility)
# define NIBBLER_ABI __attribute__((visibility("default")))
#else
# define NIBBLER_ABI
#endif

#define STR(x)    #x
#define SYMSTR(x) STR(x)

class NibblerGUI {
protected:
	Board *_board;

public:
	NibblerGUI(Board *board) : _board(board) {}
	virtual ~NibblerGUI() {};

	virtual void render()      = 0;
	virtual void poll_inputs() = 0;
};

using NibblerInitFn = void (*)(Board *);
#define NIBBLER_INIT_FN_    nibbler_init_fn
#define NIBBLER_INIT_FN_SYM SYMSTR(NIBBLER_INIT_FN_)
#define nibbler_init        extern "C" NIBBLER_ABI void NIBBLER_INIT_FN_(Board *_board)

using NibblerDeinitFn = void (*)();
#define NIBBLER_DEINIT_FN_    nibbler_deinit_fn
#define NIBBLER_DEINIT_FN_SYM SYMSTR(NIBBLER_DEINIT_FN_)
#define nibbler_deinit        extern "C" NIBBLER_ABI void NIBBLER_DEINIT_FN_()

using NibblerGetGUIFn = NibblerGUI *(*)();
#define NIBBLER_GET_GUI_FN_    nibbler_get_gui_fn
#define NIBBLER_GET_GUI_FN_SYM SYMSTR(NIBBLER_GET_GUI_FN_)
#define nibbler_get_gui        extern "C" NIBBLER_ABI NibblerGUI *NIBBLER_GET_GUI_FN_()
