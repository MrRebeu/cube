#include "cube3d.h"

double	calculate_distance_to_portal(t_game *game, double portal_x,
			double portal_y)
{
	double	dx;
	double	dy;
	double	distance;

	dx = game->player.x - portal_x;
	dy = game->player.y - portal_y;
	distance = sqrt(dx * dx + dy * dy);
	return (distance);
}
