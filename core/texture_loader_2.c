#include "cube3d.h"

int	load_all_textures(t_game *game)
{
	if (!load_basic_textures(game))
		return (0);
	if (!load_door_textures(game))
		return (0);
	if (!load_special_textures(game))
		return (0);
	return (1);
}
