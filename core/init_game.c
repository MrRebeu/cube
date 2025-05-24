/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_game.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:43:46 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/21 22:11:04 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

int count_enemies_in_map(t_game *game)
{
    int count = 0;
    int y = 0;
    
    while (y < game->map.height)
    {
        int x = 0;
        while (x < game->map.width)
        {
            if (game->map.matrix[y][x] == 'M')
                count++;
            x++;
        }
        y++;
    }
    return (count);
}

int load_shared_shoot_enemy_sprites(t_game *game, t_img shared_sprites[2])
{
    int width, height;
    
    // Charger sprite 0
    shared_sprites[0].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_shot.xpm", &width, &height);
    if (!shared_sprites[0].ptr)
        return (0);
        
    shared_sprites[0].width = width;
    shared_sprites[0].height = height;
    shared_sprites[0].addr = mlx_get_data_addr(shared_sprites[0].ptr,
        &shared_sprites[0].bits_per_pixel,
        &shared_sprites[0].line_length,
        &shared_sprites[0].endian);
        
    // Charger sprite 1
    shared_sprites[1].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_shot01.xpm", &width, &height);
    if (!shared_sprites[1].ptr)
        return (0);
        
    shared_sprites[1].width = width;
    shared_sprites[1].height = height;
    shared_sprites[1].addr = mlx_get_data_addr(shared_sprites[1].ptr,
        &shared_sprites[1].bits_per_pixel,
        &shared_sprites[1].line_length,
        &shared_sprites[1].endian);
        
    return (1);
}

int load_shared_enemy_sprites(t_game *game, t_img shared_sprites[2])
{
    int width, height;
    
    // Charger sprite 0
    shared_sprites[0].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_walk.xpm", &width, &height);
    if (!shared_sprites[0].ptr)
        return (0);
        
    shared_sprites[0].width = width;
    shared_sprites[0].height = height;
    shared_sprites[0].addr = mlx_get_data_addr(shared_sprites[0].ptr,
        &shared_sprites[0].bits_per_pixel,
        &shared_sprites[0].line_length,
        &shared_sprites[0].endian);
        
    // Charger sprite 1
    shared_sprites[1].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_walk01.xpm", &width, &height);
    if (!shared_sprites[1].ptr)
        return (0);
        
    shared_sprites[1].width = width;
    shared_sprites[1].height = height;
    shared_sprites[1].addr = mlx_get_data_addr(shared_sprites[1].ptr,
        &shared_sprites[1].bits_per_pixel,
        &shared_sprites[1].line_length,
        &shared_sprites[1].endian);
        
    return (1);
}

int	load_weapon_textures(void *mlx, t_img weapon_textures[3], int weapon_type)
{
	char	*texture_paths[MAX_WEAPONS][3] = {{"./texture/w_raygun.xpm",
			"./texture/w_raygun_prefire.xpm", "./texture/w_raygun_fire.xpm"},
			{"./texture/w_portalgun.xpm", "./texture/w_portalgun.xpm",
			"./texture/w_portalgun.xpm"}};
	int		i;

	int width, height;
	i = 0;
	while (i < 3)
	{
		weapon_textures[i].ptr = mlx_xpm_file_to_image(mlx,
				texture_paths[weapon_type][i], &width, &height);
		if (!weapon_textures[i].ptr)
		{
			printf("Erreur de chargement de la texture : %s\n",
				texture_paths[weapon_type][i]);
			return (0);
		}
		weapon_textures[i].addr = mlx_get_data_addr(weapon_textures[i].ptr,
				&weapon_textures[i].bits_per_pixel,
				&weapon_textures[i].line_length, &weapon_textures[i].endian);
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
	// init_pnj(&game->pnj);
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
	game->map.wall_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/wall.xpm", &width, &height);
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
	game->map.wall_portal_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/wall_portal.xpm", &width, &height);
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
	if (!load_weapon_textures(game->mlx, game->weapons[RAYGUN], RAYGUN))
	{
		printf("Erreur de chargement des textures du Ray Gun\n");
		return (0);
	}
	if (!load_weapon_textures(game->mlx, game->weapons[PORTALGUN], PORTALGUN))
	{
		printf("Erreur de chargement des textures du Portal Gun\n");
		return (0);
	}
	game->map.door_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/door.xpm", &width, &height);
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
	game->map.door_shooted_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/door_shooted.xpm", &width, &height);
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
	game->map.wall_shooted_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/wall_shooted.xpm", &width, &height);
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
	// game->portal_pos.has_portal1 = 0;
	// game->portal_pos.has_portal2 = 0;
	// enemy
	t_img shared_morty_sprites[2];
	if (!load_shared_enemy_sprites(game, shared_morty_sprites))
	{
		printf("Erreur: impossible de charger les sprites d'ennemis\n");
		return (0);
	}
	t_img shared_morty_shoot_sprites[2];
	if (!load_shared_shoot_enemy_sprites(game, shared_morty_shoot_sprites))
	{
		printf("Erreur: impossible de charger les sprites d'ennemis shoot\n");
		return (0);
	}
	game->num_enemies = count_enemies_in_map(game);

	game->enemies = malloc(sizeof(t_enemy) * game->num_enemies);
	if (!game->enemies)
	{
		printf("Errore malloc enemies");
		return (0);
	}

	for (int i = 0; i < game->num_enemies; i++)
	{
		game->enemies[i] = (t_enemy){.x = 0.0, .y = 0.0, .angle = 0.0,
			.health = 100, .state = IDLE, .speed = 0.05, .cooldown = 0,
			.distance_to_player = 0.0, .texture = &game->map.enemy_texture,
			.active = 1, .sees_player = 0};
		
		// Assigner les sprites partagés
		game->enemies[i].walk_morty[0] = shared_morty_sprites[0];
		game->enemies[i].walk_morty[1] = shared_morty_sprites[1];
		game->enemies[i].shoot_morty[0] = shared_morty_shoot_sprites[0];
		game->enemies[i].shoot_morty[1] = shared_morty_shoot_sprites[1];
		// Initialiser l'animation
		game->enemies[i].animation.current_frame = 0;
		game->enemies[i].animation.frame_counter = 0;
	}
	if (!set_enemy_pos(game))
        printf("Errore: nessun marker 'M' trovato nella mappa");
	
	init_minimap(game);
	init_health_bar(game);
	return (1);
}


int init_game_with_4_textures(t_game *game, char *map_file, char *north_texture, char *south_texture, char *east_texture, char *west_texture){
	int	width;
	int	height;

	init_player(&game->player);
	// init_pnj(&game->pnj);
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
	game->current_weapon = RAYGUN;
	game->map.wall_portal_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/wall_portal.xpm", &width, &height);
	if (!game->map.wall_portal_texture.ptr)
		return (0);
	game->map.wall_portal_texture.width = width;
	game->map.wall_portal_texture.height = height;
	game->map.wall_portal_texture.addr = mlx_get_data_addr(game->map.wall_portal_texture.ptr,
			&game->map.wall_portal_texture.bits_per_pixel,
			&game->map.wall_portal_texture.line_length,
			&game->map.wall_portal_texture.endian);

	
	if (!load_weapon_textures(game->mlx, game->weapons[RAYGUN], RAYGUN))
	{
		printf("Erreur de chargement des textures du Ray Gun\n");
		return (0);
	}
	if (!load_weapon_textures(game->mlx, game->weapons[PORTALGUN], PORTALGUN))
	{
		printf("Erreur de chargement des textures du Portal Gun\n");
		return (0);
	}
	game->map.door_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./texture/door.xpm", &width, &height);
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
	
	// texture N E S W

	// Remplacez ces lignes dans init_game_with_4_textures() :
	// game->map.wall_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./texture/wall.xpm", &width, &height);
	// Par :

	// Charger texture Nord
	game->map.north.ptr = mlx_xpm_file_to_image(game->mlx, north_texture, &width, &height);
	if (!game->map.north.ptr)
		return (0);
	game->map.north.width = width;
	game->map.north.height = height;
	game->map.north.addr = mlx_get_data_addr(game->map.north.ptr,
			&game->map.north.bits_per_pixel,
			&game->map.north.line_length,
			&game->map.north.endian);

	// Charger texture Sud
	game->map.south.ptr = mlx_xpm_file_to_image(game->mlx, south_texture, &width, &height);
	if (!game->map.south.ptr)
		return (0);
	game->map.south.width = width;
	game->map.south.height = height;
	game->map.south.addr = mlx_get_data_addr(game->map.south.ptr,
			&game->map.south.bits_per_pixel,
			&game->map.south.line_length,
			&game->map.south.endian);

	// Charger texture Est
	game->map.east.ptr = mlx_xpm_file_to_image(game->mlx, east_texture, &width, &height);
	if (!game->map.east.ptr)
		return (0);
	game->map.east.width = width;
	game->map.east.height = height;
	game->map.east.addr = mlx_get_data_addr(game->map.east.ptr,
			&game->map.east.bits_per_pixel,
			&game->map.east.line_length,
			&game->map.east.endian);

	// Charger texture Ouest
	game->map.west.ptr = mlx_xpm_file_to_image(game->mlx, west_texture, &width, &height);
	if (!game->map.west.ptr)
		return (0);
	game->map.west.width = width;
	game->map.west.height = height;
	game->map.west.addr = mlx_get_data_addr(game->map.west.ptr,
			&game->map.west.bits_per_pixel,
			&game->map.west.line_length,
			&game->map.west.endian);

	//
	game->portal_1.is_active = 0;
	game->portal_2.is_active = 0;
	game->portal_count = 0;

	t_img shared_morty_sprites[2];
	if (!load_shared_enemy_sprites(game, shared_morty_sprites))
	{
		printf("Erreur: impossible de charger les sprites d'ennemis\n");
		return (0);
	}
	t_img shared_morty_shoot_sprites[2];
	if (!load_shared_shoot_enemy_sprites(game, shared_morty_shoot_sprites))
	{
		printf("Erreur: impossible de charger les sprites d'ennemis shoot\n");
		return (0);
	}
	game->num_enemies = count_enemies_in_map(game);

	game->enemies = malloc(sizeof(t_enemy) * game->num_enemies);
	if (!game->enemies)
	{
		printf("Errore malloc enemies");
		return (0);
	}

	for (int i = 0; i < game->num_enemies; i++)
	{
		game->enemies[i] = (t_enemy){.x = 0.0, .y = 0.0, .angle = 0.0,
			.health = 100, .state = IDLE, .speed = 0.05, .cooldown = 0,
			.distance_to_player = 0.0, .texture = &game->map.enemy_texture,
			.active = 1, .sees_player = 0};
		
		// Assigner les sprites partagés
		game->enemies[i].walk_morty[0] = shared_morty_sprites[0];
		game->enemies[i].walk_morty[1] = shared_morty_sprites[1];
		game->enemies[i].shoot_morty[0] = shared_morty_shoot_sprites[0];
		game->enemies[i].shoot_morty[1] = shared_morty_shoot_sprites[1];
		// Initialiser l'animation
		game->enemies[i].animation.current_frame = 0;
		game->enemies[i].animation.frame_counter = 0;
	}
	if (!set_enemy_pos(game))
        printf("Errore: nessun marker 'M' trovato nella mappa");
	
	init_minimap(game);
	init_health_bar(game);
	return (1);
}
