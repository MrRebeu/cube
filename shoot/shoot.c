#include "../cube3d.h"

void calculate_shoot(t_game *game)
{
	double impact_x;
	double impact_y;
	int	map_x;
	int map_y;

    int center_ray_index = DISPLAY_WIDTH / 2;
    t_ray *center_ray = &game->rays[center_ray_index];
	if (game->current_weapon == PORTALGUN)
	{

		if (center_ray->hit_type == '1')
		{
			if (game->portal_count < 2)
			{
				impact_y = center_ray->wall_hit_y;
				impact_x = center_ray->wall_hit_x;
				map_x = (int)(impact_x / TILE_SIZE);
				map_y = (int)(impact_y / TILE_SIZE);
				game->map.matrix[map_y][map_x] = 'P';
				game->portal_count++;
			}
		}
	}
	else if (game->current_weapon == RAYGUN)
	{
		if (center_ray->hit_type == '1')
		{
			impact_y = center_ray->wall_hit_y;		
			impact_x = center_ray->wall_hit_x;
			map_x = (int)(impact_x / TILE_SIZE);
			map_y = (int)(impact_y / TILE_SIZE);
			game->map.matrix[map_y][map_x] = 'i';
		}
		// else if (center_ray->hit_type == 'D')
		// {
		// 	impact_y = center_ray->wall_hit_y;		
		// 	impact_x = center_ray->wall_hit_x;
		// 	map_x = (int)(impact_x / TILE_SIZE);
		// 	map_y = (int)(impact_y / TILE_SIZE);
		// 	game->map.matrix[map_y][map_x] = 'd';
		// }
		// 	printf("Tir Ray Gun! Type touché: %c, Distance: %f\n", 
        //        center_ray->hit_type, center_ray->distance);
        
        // if (center_ray->hit_type == '1')
        // {
			
        //     printf("La porte a été endommagée!\n");
        // }
        // else if (center_ray->hit_type == 'M')
        // {
        //     printf("Ennemi touché!\n");
        // }
	}
}

int	mouse_button(int button, int x, int y, t_game *game)
{
	(void)x;
	(void)y;
	if (button == 1)
	{
		if (game->player.fire_cooldown <= 0)
		{
			game->player.is_firing = 1;
			game->player.fire_cooldown = 10;
			calculate_shoot(game);
		}
	}
	if (button == 4 || button == 5)
	{
		if (game->current_weapon == RAYGUN)
			game->current_weapon = PORTALGUN;
		else if (game->current_weapon == PORTALGUN)
			game->current_weapon = RAYGUN;
	}
	else if (button == 3 && game->current_weapon == PORTALGUN)
	{
		remove_all_portals(game);
	}

	return (0);
}
