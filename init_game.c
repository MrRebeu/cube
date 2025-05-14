/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_game.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:43:46 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/14 23:57:19 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

int	init_game(t_game *game, char *map_file)
{
	int	width;
	int	height;

	init_player(&game->player);
	game->mlx = mlx_init();
	if (!game->mlx)
		return (0);
	game->win = mlx_new_window(game->mlx, DISPLAY_WIDTH, DISPLAY_HEIGHT,
			"Raycaster");
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
	//
	game->map.wall_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./wall.xpm",
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
	game->map.floor_texture.ptr = mlx_xpm_file_to_image(game->mlx,
			"./floor.xpm", &width, &height);
	if (!game->map.floor_texture.ptr)
		return (0);
	game->map.floor_texture.width = width;
	game->map.floor_texture.height = height;
	game->map.floor_texture.addr = mlx_get_data_addr(game->map.floor_texture.ptr,
			&game->map.floor_texture.bits_per_pixel,
			&game->map.floor_texture.line_length,
			&game->map.floor_texture.endian);
	game->weapons[0].ptr = mlx_xpm_file_to_image(game->mlx, "./arm_1.xpm",
			&width, &height);
	if (!game->weapons[0].ptr)
		return (0);
	game->weapons[0].width = width;
	game->weapons[0].height = height;
	game->weapons[0].addr = mlx_get_data_addr(game->weapons[0].ptr,
			&game->weapons[0].bits_per_pixel, &game->weapons[0].line_length,
			&game->weapons[0].endian);
	game->map.door_texture.ptr = mlx_xpm_file_to_image(game->mlx, "./door.xpm",
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
	game->player.x = 100;
	game->player.y = 100;
	game->player.angle = 0;
	game->player.fov = FOV;
	game->current_weapon = 0;
	return (1);
}
