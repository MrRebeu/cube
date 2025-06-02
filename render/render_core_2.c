#include "cube3d.h"

void	render_next_frame_enemies(t_game *game)
{
	int	j;

	j = 0;
	while (j < game->num_enemies)
	{
		if (game->enemies[j].active)
			render_enemy(game, &game->enemies[j]);
		j++;
	}
}
