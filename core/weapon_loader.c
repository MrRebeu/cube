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
    game->num_weapon_pickup = count_weapons_in_map(game);
    
    // ✅ Si pas d'armes, c'est OK !
    if (game->num_weapon_pickup == 0)
    {
        game->weapon_pickup = NULL;
        return (1); // ← Succès, pas d'échec
    }
    
    game->weapon_pickup = malloc(sizeof(t_weapon_pickup) * game->num_weapon_pickup);
    if (!game->weapon_pickup)
        return (0); // ← Ça c'est un vrai échec (malloc failed)
    
    // Initialiser les armes
    int i = 0;
    while (i < game->num_weapon_pickup)
    {
        game->weapon_pickup[i].active = 0;
        game->weapon_pickup[i].sprite.ptr = NULL;
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
            if (game->map.matrix[y][x] == 'R' || game->map.matrix[y][x] == 'G' || game->map.matrix[y][x] == 'H')
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
    int y = 0;
    int x;
    int weapon_index = 0;

    // Si pas d'armes allouées, pas d'erreur
    if (game->num_weapon_pickup == 0)
        return (1);

    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            if ((game->map.matrix[y][x] == 'R' || game->map.matrix[y][x] == 'G' || game->map.matrix[y][x] == 'H' )&&
                weapon_index < game->num_weapon_pickup)
            {
                game->weapon_pickup[weapon_index].x = (x * TILE_SIZE) + (TILE_SIZE / 2);
                game->weapon_pickup[weapon_index].y = (y * TILE_SIZE) + (TILE_SIZE / 2);
                
                if (game->map.matrix[y][x] == 'R')
                {
                    game->weapon_pickup[weapon_index].weapon_type = RAYGUN;
                    if (!load_weapon_pickup_sprite(game, &game->weapon_pickup[weapon_index], 
                        "./texture/raygun_pickup.xpm"))
                    {
                        printf("Warning: raygun_pickup.xpm non trouvé\n");
                        // Continue quand même au lieu de faire échouer
                    }
                }
                else if (game->map.matrix[y][x] == 'G')
                {
                    game->weapon_pickup[weapon_index].weapon_type = PORTALGUN;
                    if (!load_weapon_pickup_sprite(game, &game->weapon_pickup[weapon_index], 
                        "./texture/portalgun_pickup.xpm"))
                    {
                        printf("Warning: portalgun_pickup.xpm non trouvé\n");
                        // Continue quand même au lieu de faire échouer
                    }
                }
                else if (game->map.matrix[y][x] == 'H')
                {
                    game->weapon_pickup[weapon_index].weapon_type = HEALGUN;
                    if (!load_weapon_pickup_sprite(game, &game->weapon_pickup[weapon_index], 
                        "./texture/healgun_pickup.xpm"))
                    {
                        printf("Warning: healgun_pickup.xpm non trouvé\n");
                        // Continue quand même au lieu de faire échouer
                    }
                }
                game->weapon_pickup[weapon_index].active = 1;
                weapon_index++;
            }
            x++;
        }
        y++;
    }
    
    // ✅ Toujours succès - même s'il n'y a pas d'armes
    return (1);
}
int	load_single_weapon_texture(void *mlx, t_img *tex, char *path)
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
    // ✅ CORRECTION : Réorganiser selon les nouveaux indices
    const char *paths[MAX_WEAPONS][3] = {
        {"./texture/hands.xpm", "./texture/hands.xpm", "./texture/hands.xpm"},           // HANDS = 0
        {"./texture/w_portalgun.xpm", "./texture/w_portalgun.xpm", "./texture/w_portalgun.xpm"}, // PORTALGUN = 1  
        {"./texture/w_raygun.xpm", "./texture/w_raygun_prefire.xpm", "./texture/w_raygun_fire.xpm"}, // RAYGUN = 2
        {"./texture/w_healgun.xpm", "./texture/w_healgun.xpm", "./texture/w_healgun.xpm"}  // HEALGUN = 3
    };
    
    int i = 0;
    while (i < 3)
    {
        if (!load_single_weapon_texture(mlx, &weapon_textures[i],
                (char *)paths[weapon_type][i]))
            return (0);
        i++;
    }
    return (1);
}
void disable_weapon_pickup_at_position(t_game *game, int map_x, int map_y, int weapon_type)
{
	int i = 0;
	
	while (i < game->num_weapon_pickup)
	{
		if (game->weapon_pickup[i].active)
		{
			// Calculer la position de la cellule de cette arme
			int weapon_map_x = (int)(game->weapon_pickup[i].x / TILE_SIZE);
			int weapon_map_y = (int)(game->weapon_pickup[i].y / TILE_SIZE);
			
			// Vérifier si c'est la même cellule et le même type
			if (weapon_map_x == map_x && weapon_map_y == map_y && 
				game->weapon_pickup[i].weapon_type == weapon_type)
			{
				game->weapon_pickup[i].active = 0;
				printf("✅ Arme désactivée: type=%d à [%d,%d]\n", weapon_type, map_x, map_y);
				return;
			}
		}
		i++;
	}
	printf("❌ Arme non trouvée: type=%d à [%d,%d]\n", weapon_type, map_x, map_y);
}

int count_open_doors_in_map(t_game *game)
{
    int count = 0;
    int y = 0;
    int x;
    
    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            if (game->map.matrix[y][x] == 'O')
                count++;
            x++;
        }
        y++;
    }
    return count;
}

int load_open_door_sprites(t_game *game)
{
    int width, height;
    
    game->num_open_doors = count_open_doors_in_map(game);
    
    if (game->num_open_doors == 0)
    {
        game->open_doors = NULL;
        return (1);
    }
    
    game->open_doors = malloc(sizeof(t_open_door) * game->num_open_doors);
    if (!game->open_doors)
        return (0);
    
    // Charger le sprite de porte ouverte
    int i = 0;
    while (i < game->num_open_doors)
    {
        game->open_doors[i].sprite.ptr = mlx_xpm_file_to_image(game->mlx, 
            "./texture/door_open.xpm", &width, &height);
        if (!game->open_doors[i].sprite.ptr)
        {
            printf("Erreur: door_open.xpm non trouvé\n");
            return (0);
        }
        
        game->open_doors[i].sprite.width = width;
        game->open_doors[i].sprite.height = height;
        game->open_doors[i].sprite.addr = mlx_get_data_addr(game->open_doors[i].sprite.ptr,
            &game->open_doors[i].sprite.bits_per_pixel,
            &game->open_doors[i].sprite.line_length,
            &game->open_doors[i].sprite.endian);
        
        game->open_doors[i].active = 0; // Sera activé par set_open_door_positions
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
	if (!load_healgun(game))  // ← AJOUTER CETTE LIGNE
		return (0);
	return (1);
}
