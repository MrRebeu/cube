#include "cube3d.h"

void	check_interact_door(t_game *game)
{
	double	check_distance;
	double	check_x;
	double	check_y;
	int		cell_x;
	int		cell_y;

	check_distance = TILE_SIZE * 1.0;
	check_x = game->player.x + cos(game->player.angle) * check_distance;
	check_y = game->player.y + sin(game->player.angle) * check_distance;
	cell_x = (int)(check_x / TILE_SIZE);
	cell_y = (int)(check_y / TILE_SIZE);
	if (cell_x < 0 || cell_x >= game->map.width || cell_y < 0 
		|| cell_y >= game->map.height)
			return ;
	if (game->map.matrix[cell_y][cell_x] == 'D')// || game->map.matrix[cell_y][cell_x] == 'd')
	{
		game->map.matrix[cell_y][cell_x] = 'O';
	}
}
