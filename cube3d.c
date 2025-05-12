/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cube3d.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:09:42 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/12 17:12:57 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

t_intersect	v_intersection(int x_player, int y_player, double radiant_angle)
{
	t_intersect	v;

	if (cos(radiant_angle) > 0) // se il player guarda a destra
	{
		v.x = floor(x_player / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
		// arrotonda in difetto alla successiva tiles
		v.step_x = TILE_SIZE;
		// lo step successivo sull'asse orizzontale e' la prossima tilesize
	}
	else
	{
		v.x = floor(x_player / TILE_SIZE) * TILE_SIZE - 0.0001;
		// se guarda a sinistra
		v.step_x = -TILE_SIZE;
	}
	v.y = y_player + (v.x - x_player) * tan(radiant_angle);
	// il . di intersezione dir proporzionale alla tangente dell'angolo
	if (sin(radiant_angle) > 0)
		v.step_y = TILE_SIZE * tan(radiant_angle); // se guarda in alto
	else
		v.step_y = -TILE_SIZE * tan(radiant_angle); // in basso
	return (v);
}

t_intersect	h_intersection(int x_player, int y_player, double radiant_angle)
{
	t_intersect	h;

	if (sin(radiant_angle) > 0) // se guarda in alto
	{
		h.y = floor(y_player / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
		h.step_y = TILE_SIZE;
	}
	else // in basso
	{
		h.y = floor(y_player / TILE_SIZE) * TILE_SIZE - 0.0001;
		h.step_y = -TILE_SIZE;
	}
	h.x = x_player + (h.y - y_player) / tan(radiant_angle);
	if (cos(radiant_angle) > 0) // destra
		h.step_x = TILE_SIZE / tan(radiant_angle);
	else // sinistra
		h.step_x = -TILE_SIZE / tan(radiant_angle);
	return (h);
}

int	is_not_wall(t_map *map, double x, double y)
{
	int	map_x;
	int	map_y;

	map_x = (int)(x / TILE_SIZE);
	// riconduce x e y alle rispettive celle della mappa
	map_y = (int)(y / TILE_SIZE);
	if (map_x < 0 || map_x >= map->width || map_y < 0 || map_y >= map->height)
		return (0);
	if (map->matrix[map_y][map_x] == '1') // controlla se e' muro
		return (0);
	else
		return (1);
}

double	normalize_angle(double angle)
{
	while (angle < 0)
		angle = angle + (2 * M_PI);
	while (angle >= 2 * M_PI)
		angle = angle - (2 * M_PI);
	return (angle);
}

double	ray_casting(t_game *game, double radiant_angle)
{
	t_intersect	v;
	t_intersect	h;
	double		dist_v;
	double		dist_h;

	radiant_angle = normalize_angle(radiant_angle);
	v = v_intersection(game->player.x, game->player.y, radiant_angle);
	h = h_intersection(game->player.x, game->player.y, radiant_angle);
	while (is_not_wall(&game->map, v.x, v.y))
	{
		v.x = v.x + v.step_x;
		v.y = v.y + v.step_y;
	}
	while (is_not_wall(&game->map, h.x, h.y))
	{
		h.x = h.x + h.step_x;
		h.y = h.y + h.step_y;
	}
	dist_v = sqrt(pow(v.x - game->player.x, 2) + pow(v.y - game->player.y, 2));
	dist_h = sqrt(pow(h.x - game->player.x, 2) + pow(h.y - game->player.y, 2));
	return (fmin(dist_v, dist_h));
}

double	no_fish_eye(double min_distance, double radiant_angle,
		double player_angle)
{
	double	corrected_dist;
	double	angle_diff;

	angle_diff = radiant_angle - player_angle;
	angle_diff = normalize_angle(angle_diff);
	// calcola la distanza perpendicolare
	corrected_dist = min_distance * cos(angle_diff);
	return (corrected_dist);
}
int	calc_wall_height(double corrected_dist)
{
	double	distance_to_projection_plane;
	double	wall_height;

	distance_to_projection_plane = (DISPLAY_WIDTH / 2.0) / tan(FOV / 2.0);
	wall_height = (TILE_SIZE / fmax(corrected_dist, 1))
		* distance_to_projection_plane;
	return ((int)wall_height);
}
