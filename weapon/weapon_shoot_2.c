#include "cube3d.h"

void	calculate_shoot(t_game *game)
{
	int		center_ray_index;
	t_ray	*center_ray;

	center_ray_index = DISPLAY_WIDTH / 2;
	center_ray = &game->rays[center_ray_index];
	if (game->current_weapon == PORTALGUN)
		handle_portal_shot(game, center_ray);
	else if (game->current_weapon == RAYGUN)
		handle_raygun_shot(game, center_ray);
}
