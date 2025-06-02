#include "cube3d.h"

void	minimap(t_game *game)
{
	if (!game->minimap.show)
		return ;
	draw_minimap_background(game);
	draw_minimap_cells(game);
	draw_minimap_player(game);
}
