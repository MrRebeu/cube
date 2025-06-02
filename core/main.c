#include "cube3d.h"

int	main(int argc, char **argv)
{
	t_game	game;

	if (!init_game_mode(&game, argc, argv))
		return (1);
	init_rays(&game);
	loop_game(&game);
	return (0);
}
