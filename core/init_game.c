/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_game.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:43:46 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/18 18:53:06 by abkhefif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

int load_weapon_textures(void *mlx, t_img *weapon_textures)
{
    int width, height;
    char *texture_paths[] = {
        "./texture/w_raygun_neutre.xpm",    // Frame 0
        "./texture/w_raygun_prefire.xpm",   // Frame 1
        "./texture/w_raygun_fire.xpm"       // Frame 2
    };
    
    int i = 0;
    while (i < 3)
    {
        weapon_textures[i].ptr = mlx_xpm_file_to_image(mlx, texture_paths[i], &width, &height);
        
        // Vérifier si le chargement a réussi
        if (!weapon_textures[i].ptr)
        {
            printf("Erreur de chargement de la texture : %s\n", texture_paths[i]);
            return (0);
        }

        // Récupérer les informations de la texture
        weapon_textures[i].addr = mlx_get_data_addr(
            weapon_textures[i].ptr,
            &weapon_textures[i].bits_per_pixel,
            &weapon_textures[i].line_length,
            &weapon_textures[i].endian
        );

        // Stocker les dimensions
        weapon_textures[i].width = width;
        weapon_textures[i].height = height;

        i++;
    }

    return (1);
}

int	init_game(t_game *game, char *map_file)
{
	int	width;
	int	height;

	init_player(&game->player);
	//init_pnj(&game->pnj);
	// if (!set_pnj_pos(game))
	// {
	// 	printf("Erreur morty");
	// 	return (0);
	// }
	game->player.game = game;
	game->mlx = mlx_init();
	if (!game->mlx)
		return (0);
	game->win = mlx_new_window(game->mlx, DISPLAY_WIDTH, DISPLAY_HEIGHT,
			"Raycaster");
	mlx_mouse_hide(game->mlx, game->win);

	if (!game->win)
		return (0);
	game->screen.ptr = mlx_new_image(game->mlx, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	if (!game->screen.ptr)
		return (0);
	game->screen.addr = mlx_get_data_addr(game->screen.ptr,
			&game->screen.bits_per_pixel, &game->screen.line_length,
			&game->screen.endian);
	if (!read_map(map_file, game))
		return (0);
	if (!set_player_pos(game))
    {
        printf("Erreur: Aucune position de départ trouvée dans la carte!\n");
        return (0);
    }

	//
	game->map.wall_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./texture/wall.xpm",
			&width, &height);
	if (!game->map.wall_texture.ptr)
		return (0);
	game->map.wall_texture.width = width;
	game->map.wall_texture.height = height;
	game->map.wall_texture.addr = mlx_get_data_addr(game->map.wall_texture.ptr,
			&game->map.wall_texture.bits_per_pixel,
			&game->map.wall_texture.line_length,
			&game->map.wall_texture.endian);
	//
	game->current_weapon = PORTALGUN;

	
	game->map.wall_portal_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./texture/wall_portal.xpm", &width, &height);
	if (!game->map.wall_portal_texture.ptr)
		return (0);
	game->map.wall_portal_texture.width = width;
	game->map.wall_portal_texture.height = height;
		game->map.wall_portal_texture.addr = mlx_get_data_addr(game->map.wall_portal_texture.ptr,
			&game->map.wall_portal_texture.bits_per_pixel,
			&game->map.wall_portal_texture.line_length,
			&game->map.wall_portal_texture.endian);
	
	game->map.floor_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/floor.xpm", &width, &height);
	if (!game->map.floor_texture.ptr)
		return (0);
	game->map.floor_texture.width = width;
	game->map.floor_texture.height = height;
	game->map.floor_texture.addr = mlx_get_data_addr(game->map.floor_texture.ptr,
			&game->map.floor_texture.bits_per_pixel,
			&game->map.floor_texture.line_length,
			&game->map.floor_texture.endian);
	if (!load_weapon_textures(game->mlx, &game->weapons[RAYGUN]))
	{
		printf("Erreur de chargement des textures du Ray Gun\n");
		return (0);
	}
	// game->weapons[RAYGUN].ptr = mlx_xpm_file_to_image(game->mlx, "./texture/w_raygun.xpm",
	// 		&width, &height);
	// if (!game->weapons[RAYGUN].ptr)
	// 	return (0);
	// game->weapons[RAYGUN].width = width;
	// game->weapons[RAYGUN].height = height;
	// game->weapons[RAYGUN].addr = mlx_get_data_addr(game->weapons[RAYGUN].ptr,
	// 		&game->weapons[RAYGUN].bits_per_pixel, &game->weapons[RAYGUN].line_length,
	// 		&game->weapons[RAYGUN].endian);

	game->weapons[PORTALGUN].ptr = mlx_xpm_file_to_image(game->mlx, "./texture/w_portalgun.xpm",
			&width, &height);
	if (!game->weapons[PORTALGUN].ptr)
		return (0);
	game->weapons[PORTALGUN].width = width;
	game->weapons[PORTALGUN].height = height;
	game->weapons[PORTALGUN].addr = mlx_get_data_addr(game->weapons[PORTALGUN].ptr,
			&game->weapons[PORTALGUN].bits_per_pixel, &game->weapons[PORTALGUN].line_length,
			&game->weapons[PORTALGUN].endian);
			
	game->map.door_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./texture/door.xpm",
			&width, &height);
	if (!game->map.door_texture.ptr)
	{
		printf("problem avec door.xpm");
		return (0);
	}
	game->map.door_texture.width = width;
	game->map.door_texture.height = height;
	game->map.door_texture.addr = mlx_get_data_addr(game->map.door_texture.ptr,
			&game->map.door_texture.bits_per_pixel,
			&game->map.door_texture.line_length,
			&game->map.door_texture.endian);

	game->map.door_shooted_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./texture/door_shooted.xpm",
			&width, &height);
	if (!game->map.door_shooted_texture.ptr)
	{
		printf("problem avec door_shooted.xpm");
		return (0);
	}
	game->map.door_shooted_texture.width = width;
	game->map.door_shooted_texture.height = height;
	game->map.door_shooted_texture.addr = mlx_get_data_addr(game->map.door_shooted_texture.ptr,
			&game->map.door_shooted_texture.bits_per_pixel,
			&game->map.door_shooted_texture.line_length,
			&game->map.door_shooted_texture.endian);

			

	game->map.wall_shooted_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./texture/wall_shooted.xpm",
			&width, &height);
	if (!game->map.wall_shooted_texture.ptr)
	{
		printf("problem avec wall_shooted.xpm");
		return (0);
	}
	game->map.wall_shooted_texture.width = width;
	game->map.wall_shooted_texture.height = height;
	game->map.wall_shooted_texture.addr = mlx_get_data_addr(game->map.wall_shooted_texture.ptr,
			&game->map.wall_shooted_texture.bits_per_pixel,
			&game->map.wall_shooted_texture.line_length,
			&game->map.wall_shooted_texture.endian);
			
	game->portal_1.is_active = 0;
	game->portal_2.is_active = 0;
	game->portal_count = 0;
	//game->portal_pos.has_portal1 = 0;
	//game->portal_pos.has_portal2 = 0;
	init_minimap(game);
	init_health_bar(game);
	return (1);
}
