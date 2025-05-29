/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_move.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:44:16 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/20 23:20:06 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

void check_weapon_pickup(t_player *player)
{
	int player_map_x;
	int player_map_y;
	char cell_type;

	if (!player->game)
		return;
	player_map_x = (int)(player->x / TILE_SIZE);
	player_map_y = (int)(player->y / TILE_SIZE);
	if (player_map_x >= 0 && player_map_x < player->game->map.width &&
        player_map_y >= 0 && player_map_y < player->game->map.height)
	{
		cell_type = player->game->map.matrix[player_map_y][player_map_x];
		if (cell_type == 'R')
		{
			if (!player->has_weapon[RAYGUN])
			{
				player->has_weapon[RAYGUN] = true;
				player->game->map.matrix[player_map_y][player_map_x] = '0';
				player->current_weapon = RAYGUN;
				player->game->current_weapon = RAYGUN;
				disable_weapon_pickup_at_position(player->game, player_map_x, player_map_y, HEALGUN);
			}
		}
		if (cell_type == 'G')
		{
			if (!player->has_weapon[PORTALGUN])
			{
				player->has_weapon[PORTALGUN] = true;
				player->current_weapon = PORTALGUN;
				player->game->current_weapon = PORTALGUN;
				disable_weapon_pickup_at_position(player->game, player_map_x, player_map_y, HEALGUN);
			}
		}
		if (cell_type == 'R')
		{
			if (!player->has_weapon[RAYGUN])
			{
				player->has_weapon[RAYGUN] = true;
				player->game->map.matrix[player_map_y][player_map_x] = '0';
				player->current_weapon = RAYGUN;
				player->game->current_weapon = RAYGUN;
				disable_weapon_pickup_at_position(player->game, player_map_x, player_map_y, HEALGUN);	
			}
		}
		if (cell_type == 'H')
		{
			if (!player->has_weapon[HEALGUN])
            {
                // ✅ PREMIÈRE FOIS : collecter le pistolet
                player->has_weapon[HEALGUN] = true;
                player->current_weapon = HEALGUN;
                player->game->current_weapon = HEALGUN;
                player->healgun_ammo = 1; // Commence avec 1 munition
                player->healgun_is_loaded = 1;
                player->game->map.matrix[player_map_y][player_map_x] = '0'; // Enlever de la carte
                printf("💉 HEAL GUN COLLECTÉ ! (1 munition)\n");
            }
            else
            {
                // ✅ DEUXIÈME FOIS ET + : collecter une seringue
                player->healgun_ammo++;
                player->game->map.matrix[player_map_y][player_map_x] = '0'; // Enlever de la carte
                printf("💉 Seringue collectée ! (%d munitions)\n", player->healgun_ammo);
                
                // Si le healgun était vide, le charger
                if (!player->healgun_is_loaded)
                {
                    player->healgun_is_loaded = 1;
                    printf("🔋 Heal Gun rechargé !\n");
                }
            }
		}
	}
}

int	key_release(int keycode, t_player *player)
{
	if (keycode == W)
		player->key_up = false;
	if (keycode == S)
		player->key_down = false;
	if (keycode == A)
		player->key_left = false;
	if (keycode == D)
		player->key_right = false;
	if (keycode == Q)
		player->left_rotate = false;
	if (keycode == E)
		player->right_rotate = false;
	if (keycode == R)
		player->turn_back = false;
	if (keycode == LEFT)
		player->left = false;
	if (keycode == RIGHT)
		player->right = false;
	return (0);
}
int	key_press(int keycode, t_player *player)
{
	if (keycode == ESC)
		close_window(NULL);
	if (keycode == W)
		player->key_up = true;
	if (keycode == S)
		player->key_down = true;
	if (keycode == A)
		player->key_left = true;
	if (keycode == D)
		player->key_right = true;
	if (keycode == Q)
		player->left_rotate = true;
	if (keycode == E)
		player->right_rotate = true;
	if (keycode == R)
		player->turn_back = true;
	if (keycode == LEFT)
		player->left = true;
	if (keycode == RIGHT)
		player->right = true;
	
	// ✅ CORRECTION : Vérifier si le joueur possède l'arme
	if (keycode == 48) // Touche 0 pour les mains
	{
		if (player->has_weapon[HANDS])
		{
			player->current_weapon = HANDS;
			if (player->game)
				player->game->current_weapon = HANDS;
			printf("✅ Arme changée : Mains\n");
		}
	}
	if (keycode == 49) // Touche 1 pour raygun
	{
		if (player->has_weapon[RAYGUN])
		{
			player->current_weapon = RAYGUN;
			if (player->game)
				player->game->current_weapon = RAYGUN;
			printf("✅ Arme changée : Ray Gun\n");
		}
		else
		{
			printf("❌ Ray Gun non collecté !\n");
		}
	}
	if (keycode == 50) // Touche 2 pour portal gun
	{
		if (player->has_weapon[PORTALGUN])
		{
			player->current_weapon = PORTALGUN;
			if (player->game)
				player->game->current_weapon = PORTALGUN;
			printf("✅ Arme changée : Portal Gun\n");
		}
		else
		{
			printf("❌ Portal Gun non collecté !\n");
		}
	}
	if (keycode == 51) // Touche 3 pour heal gun
	{
		if (player->has_weapon[HEALGUN])
		{
			player->current_weapon = HEALGUN;
			if (player->game)
				player->game->current_weapon = HEALGUN;
			printf("✅ Arme changée : Heal Gun\n");
		}
		else
		{
			printf("❌ Heal Gun non collecté !\n");
		}
	}
	if (keycode == F)
	{
		open_door(player->game);
	}
	return (0);
}
int	mouse_move(int x, int y, t_game *game)
{
	int		center_x;
	int		center_y;
	int		delta_x;
	int		delta_y;
	double	sensibility;

	center_x = DISPLAY_WIDTH / 2;
	center_y = DISPLAY_HEIGHT / 2;
	if (x == center_x && y == center_y)
		return (0);
	delta_x = x - center_x;
	delta_y = y - center_y;
	sensibility = 0.003;
	game->player.angle += delta_x * sensibility;
	game->player.angle = normalize_angle(game->player.angle);
	game->pitch -= delta_y;
	if (game->pitch > 300)
		game->pitch = 300;
	if (game->pitch < -300)
		game->pitch = -300;
	mlx_mouse_move(game->mlx, game->win, center_x, center_y);
	return (0);
}


// int	is_wall(t_game *game, float x, float y)
// {
// 	int	map_x;
// 	int	map_y;

// 	map_x = (int)(x / TILE_SIZE);
// 	map_y = (int)(y / TILE_SIZE);
// 	if (map_x < 0 || map_x >= game->map.width || map_y < 0
// 		|| map_y >= game->map.height)
// 		return (1);
// 	if (game->map.matrix[map_y][map_x] == '1'
// 		|| game->map.matrix[map_y][map_x] == 'D')
// 		return (1);
// 	return (0);
// }

int	is_wall(t_game *game, float x, float y)
{
	int	map_x;
	int	map_y;

	map_x = (int)(x / TILE_SIZE);
	map_y = (int)(y / TILE_SIZE);
	if (map_x < 0 || map_x >= game->map.width || map_y < 0
		|| map_y >= game->map.height)
		return (1);
	return (!is_not_wall_for_movement(&game->map, x, y));
}

void	move_player(t_player *player)
{
	float	angle_speed = 0.05;
	float	speed = 10.0;
	float	new_x;
	float	new_y;
	float	strafe_angle;

	// Rotazione
	if (player->left_rotate)
		player->angle -= angle_speed;
	if (player->right_rotate)
		player->angle += angle_speed;
	if (player->turn_back)
		player->angle += M_PI;

	// Normalizzazione angolo
	while (player->angle < 0)
		player->angle += 2 * M_PI;
	while (player->angle >= 2 * M_PI)
		player->angle -= 2 * M_PI;

	update_camera_vectors(player); // ⚠️ aggiorna dir_ e plane_

	// Avanti
	if (player->key_up)
	{
		new_x = player->x + player->dir_x * speed;
		new_y = player->y + player->dir_y * speed;
		if (player->game && is_not_wall_for_movement(&player->game->map, new_x, player->y))
			player->x = new_x;
		if (player->game && is_not_wall_for_movement(&player->game->map, player->x, new_y))
			player->y = new_y;
	}
	// Indietro
	if (player->key_down)
	{
		new_x = player->x - player->dir_x * speed;
		new_y = player->y - player->dir_y * speed;
		if (player->game && is_not_wall(&player->game->map, new_x, player->y))
			player->x = new_x;
		if (player->game && is_not_wall(&player->game->map, player->x, new_y))
			player->y = new_y;
	}
	// Strafe a sinistra
	if (player->key_left)
	{
		strafe_angle = player->angle - M_PI / 2;
		new_x = player->x + cos(strafe_angle) * speed;
		new_y = player->y + sin(strafe_angle) * speed;
		if (player->game && is_not_wall(&player->game->map, new_x, player->y))
			player->x = new_x;
		if (player->game && is_not_wall(&player->game->map, player->x, new_y))
			player->y = new_y;
	}
	// Strafe a destra
	if (player->key_right)
	{
		strafe_angle = player->angle + M_PI / 2;
		new_x = player->x + cos(strafe_angle) * speed;
		new_y = player->y + sin(strafe_angle) * speed;
		if (player->game && is_not_wall(&player->game->map, new_x, player->y))
			player->x = new_x;
		if (player->game && is_not_wall(&player->game->map, player->x, new_y))
			player->y = new_y;
	}
	check_portal_teleportation(player->game);
	check_weapon_pickup(player);
}

