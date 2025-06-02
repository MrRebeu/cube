#include "cube3d.h"

int	set_weapon_positions(t_game *game)
{
	int	y;
	int	x;
	int	weapon_index;

	if (game->num_weapon_pickup == 0)
		return (1);
	weapon_index = 0;
	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (is_weapon_cell(game->map.matrix[y][x])
				&& weapon_index < game->num_weapon_pickup)
				setup_weapon_at_position(game, x, y, &weapon_index);
			x++;
		}
		y++;
	}
	return (1);
}
