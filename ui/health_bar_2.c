#include "cube3d.h"

void	draw_health_bar(t_game *game)
{
	draw_health_bar_border(game);
	draw_health_bar_background(game);
	draw_health_bar_fill(game);
}
