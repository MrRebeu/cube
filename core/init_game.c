/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_game.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:43:46 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/26 20:46:48 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

int	init_mlx_window(t_game *game)
{
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
	return (1);
}

int	init_map_and_player(t_game *game, char *map_file)
{
	if (!read_map(map_file, game))
		return (0);
	if (!set_player_pos(game))
	{
		printf("Erreur: Aucune position de départ trouvée dans la carte!\n");
		return (0);
	}
	return (1);
}

void init_portals(t_game *game)
{
    game->portal_1.is_active = 0;
    game->portal_1.orientation = 0;
    game->portal_2.is_active = 0;
    game->portal_2.orientation = 0;
    game->portal_count = 0;
}

void	init_ui_components(t_game *game)
{
	init_minimap(game);
	init_health_bar(game);
}

int set_open_door_positions(t_game *game)
{
    int y = 0;
    int x;
    int door_index = 0;

    // Si pas de portes allouées, pas d'erreur
    if (game->num_open_doors == 0)
        return (1);

    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            if (game->map.matrix[y][x] == 'O' && door_index < game->num_open_doors)
            {
                game->open_doors[door_index].x = (x * TILE_SIZE) + (TILE_SIZE / 2);
                game->open_doors[door_index].y = (y * TILE_SIZE) + (TILE_SIZE / 2);
                game->open_doors[door_index].active = 1;
                
                printf("🚪 Porte ouverte initialisée à [%d,%d] -> (%.1f, %.1f)\n", 
                       x, y, game->open_doors[door_index].x, game->open_doors[door_index].y);
                
                door_index++;
            }
            x++;
        }
        y++;
    }
    
    return (1);
}

int init_game(t_game *game, char *map_file)
{
    init_player(&game->player);
    game->player.game = game;
    if (!init_mlx_window(game))
        return (0);
    if (!init_map_and_player(game, map_file))
        return (0);
    game->current_weapon = HANDS;
    if (!load_all_textures(game))
        return (0);
    if (!load_all_weapons(game))
        return (0);
    if (!load_weapon_pickup_sprites(game)) // ← NOUVEAU
        return (0);
    if (!set_weapon_positions(game)) // ← NOUVEAU
        return (0);
    if (!load_open_door_sprites(game))  // ✅ NOUVEAU
        return (0);
    if (!set_open_door_positions(game)) // ✅ NOUVEAU
        return (0);
    if (!init_all_enemies(game))
        return (0);
    if (!init_lasers(game)) // ✅ NOUVEAU
        return (0);
    init_portals(game);
    init_ui_components(game);
    return (1);
}
