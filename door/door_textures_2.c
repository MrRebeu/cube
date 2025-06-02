#include "../cube3d.h"

int	load_door_shooted_texture(t_game *game)
{
	int	width;
	int	height;

	game->map.door_shooted_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/door_shooted.xpm", &width, &height);
	if (!game->map.door_shooted_texture.ptr)
		return (0);
	setup_texture_data(&game->map.door_shooted_texture, width, height);
	return (1);
}

int	load_open_door_shooted_texture(t_game *game)
{
	int	width;
	int	height;

	game->map.open_door_shooted_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/door_shooted_open.xpm", &width, &height);
	if (!game->map.open_door_shooted_texture.ptr)
		return (0);
	setup_texture_data(&game->map.open_door_shooted_texture, width, height);
	return (1);
}
