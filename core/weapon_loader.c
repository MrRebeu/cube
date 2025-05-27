/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   weapon_loader.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 22:15:06 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/26 22:48:32 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

int load_weapon_pickup_sprites(t_game *game)
{
	int i;

	i = 0;
	game->num_weapon_pickup = count_weapons_in_map(game);
	if (game->num_weapon_pickup == 0)
		return (1);
	game->weapon_pickup = malloc(sizeof(t_weapon_pickup) * game->num_weapon_pickup);
	if (!game->weapon_pickup)
		return (0);
	while (i < game->num_weapon_pickup)
	{
		game->weapon_pickup[i].active = 1;
		i++;
	}	
	return (1);
}

int count_weapons_in_map(t_game *game)
{
    int count = 0;
    int y = 0;
    int x;
    
    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            if (game->map.matrix[y][x] == 'R' || game->map.matrix[y][x] == 'G')
                count++;
            x++;
        }
        y++;
    }
    return (count);
}

int load_weapon_pickup_sprite(t_game *game, t_weapon_pickup *pickup, char *path)
{
    int width, height;
    
    pickup->sprite.ptr = mlx_xpm_file_to_image(game->mlx, path, &width, &height);
    if (!pickup->sprite.ptr)
        return (0);
        
    pickup->sprite.width = width;
    pickup->sprite.height = height;
    pickup->sprite.addr = mlx_get_data_addr(pickup->sprite.ptr,
        &pickup->sprite.bits_per_pixel, &pickup->sprite.line_length,
        &pickup->sprite.endian);
    return (1);
}

int set_weapon_positions(t_game *game)
{
	int y;
	int x;
	int weapon_index;

	y = 0;
	weapon_index = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if ((game->map.matrix[y][x] == 'R' || game->map.matrix[y][x] == 'G') &&
				weapon_index < game->num_weapon_pickup)
			{
				game->weapon_pickup[weapon_index].x = (x * TILE_SIZE) + (TILE_SIZE / 2);
                game->weapon_pickup[weapon_index].y = (y * TILE_SIZE) + (TILE_SIZE / 2);
                if (game->map.matrix[y][x] == 'R')
					game->weapon_pickup[weapon_index].weapon_type = RAYGUN;
                    load_weapon_pickup_sprite(game, &game->weapon_pickup[weapon_index], 
                        "./texture/raygun_pickup.xpm"); 
			}
			else // 'G'
            {
                game->weapon_pickup[weapon_index].weapon_type = PORTALGUN;
                load_weapon_pickup_sprite(game, &game->weapon_pickup[weapon_index], 
                "./texture/portalgun_pickup.xpm"); // ← Ton sprite de portal gun au sol
            }
			game->weapon_pickup[weapon_index].active = 1;
			weapon_index++;
		}
		x++;
	y++;
	}
	return (weapon_index > 0);
}

static int	load_single_weapon_texture(void *mlx, t_img *tex, char *path)
{
	int	width;
	int	height;

	tex->ptr = mlx_xpm_file_to_image(mlx, path, &width, &height);
	if (!tex->ptr)
	{
		printf("Erreur de chargement de la texture : %s\n", path);
		return (0);
	}
	tex->addr = mlx_get_data_addr(tex->ptr, &tex->bits_per_pixel,
			&tex->line_length, &tex->endian);
	tex->width = width;
	tex->height = height;
	return (1);
}

int	load_weapon_textures(void *mlx, t_img weapon_textures[3], int weapon_type)
{
    const char *paths[MAX_WEAPONS][3] = {
        {"./texture/hands.xpm", "./texture/hands.xpm", "./texture/hands.xpm"},
        {"./texture/w_raygun.xpm", "./texture/w_raygun_prefire.xpm", "./texture/w_raygun_fire.xpm"},
        {"./texture/w_portalgun.xpm", "./texture/w_portalgun.xpm", "./texture/w_portalgun.xpm"}};
	int			i;

	i = 0;
	while (i < 3)
	{
		if (!load_single_weapon_texture(mlx, &weapon_textures[i],
				(char *)paths[weapon_type][i]))
			return (0);
		i++;
	}
	return (1);
}

int load_hands(t_game *game)
{
    int width, height;
    
    game->weapons[HANDS][0].ptr = mlx_xpm_file_to_image(game->mlx, 
        "./texture/hand.xpm", &width, &height);
    if (!game->weapons[HANDS][0].ptr)
    {
        printf("Erreur de chargement de la texture des mains\n");
        return (0);
    }
    game->weapons[HANDS][0].width = width;
    game->weapons[HANDS][0].height = height;
    game->weapons[HANDS][0].addr = mlx_get_data_addr(game->weapons[HANDS][0].ptr,
        &game->weapons[HANDS][0].bits_per_pixel,
        &game->weapons[HANDS][0].line_length,
        &game->weapons[HANDS][0].endian);
    game->weapons[HANDS][1] = game->weapons[HANDS][0];
    game->weapons[HANDS][2] = game->weapons[HANDS][0];
    
    return (1);
}

int	load_raygun(t_game *game)
{
	if (!load_weapon_textures(game->mlx, game->weapons[RAYGUN], RAYGUN))
	{
		printf("Erreur de chargement des textures du Ray Gun\n");
		return (0);
	}
	return (1);
}

int	load_portalgun(t_game *game)
{
	if (!load_weapon_textures(game->mlx, game->weapons[PORTALGUN], PORTALGUN))
	{
		printf("Erreur de chargement des textures du Portal Gun\n");
		return (0);
	}
	return (1);
}

int	load_all_weapons(t_game *game)
{
	if (!load_hands(game))
        return (0);
	if (!load_raygun(game))
		return (0);
	if (!load_portalgun(game))
		return (0);
	return (1);
}
