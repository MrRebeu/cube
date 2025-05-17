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
			impact_y = center_ray->wall_hit_y;
			impact_x = center_ray->wall_hit_x;
			map_x = (int)(impact_x / TILE_SIZE);
			map_y = (int)(impact_y / TILE_SIZE);
			game->map.matrix[map_y][map_x] = 'P';
        }
	}
	else if (game->current_weapon == RAYGUN)
	{
			printf("Tir Ray Gun! Type touché: %c, Distance: %f\n", 
               center_ray->hit_type, center_ray->distance);
        
        if (center_ray->hit_type == 'D')
        {
			
            printf("La porte a été endommagée!\n");
        }
        else if (center_ray->hit_type == 'M')
        {
            printf("Ennemi touché!\n");
        }
	}
    // printf("Tir! Type touché: %c, Distance: %f\n", 
    //        center_ray->hit_type, center_ray->distance);
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
	return (0);
}
