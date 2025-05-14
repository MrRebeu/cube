/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cube3d.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:09:42 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/14 20:32:34 by abkhefif         ###   ########.fr       */
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

int is_not_wall(t_map *map, double x, double y)
{
    int map_x;
    int map_y;

    map_x = (int)(x / TILE_SIZE);
    map_y = (int)(y / TILE_SIZE);
    
    if (map_x < 0 || map_x >= map->width || map_y < 0 || map_y >= map->height)
        return (0);
    
    // Voici le point clé - actuellement '1' et 'D' sont traités pareil
    if (map->matrix[map_y][map_x] == '1' || map->matrix[map_y][map_x] == 'D')
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

/*Explanation of the Ray Casting Modification
This modification enhances the ray casting system to identify and differentiate between various map elements (walls, doors, etc.) for more versatile rendering.
Key Changes:

Element Type Detection: The ray casting now identifies what type of element (wall '1', door 'D', etc.) each ray hits by examining the character in the map matrix at the collision point.
Information Storage: We store this element type in the ray structure (hit_type field) for each column of the screen.
Code Modularization: The function has been divided into smaller, more focused helper functions:

get_hit_type: Determines what map element is at specific coordinates
store_ray_info: Handles saving all ray collision data
Main ray_casting: Orchestrates the overall process



Benefits:

Visual Diversity: Different elements (walls, doors, decorations) can be rendered with unique textures
Gameplay Enhancement: Enables interactive elements like doors or switches
Maintainability: Clearer code structure makes future modifications easier
Extensibility: New element types can be added simply by assigning new characters in the map

This change maintains the core raycasting algorithm while adding the capability to distinguish between different types of objects in the game world, creating a foundation for more complex environments and interactions.*/

char get_hit_type(t_map *map, double x, double y)
{
    int map_x = (int)(x / TILE_SIZE);
    int map_y = (int)(y / TILE_SIZE);
    
    if (map_x >= 0 && map_x < map->width && map_y >= 0 && map_y < map->height)
        return map->matrix[map_y][map_x];
    else
        return '1'; // Par défaut si hors limites
}

void store_ray_info(t_game *game, int column_x, double distance, 
                    double hit_x, double hit_y, int is_vertical, char hit_type)
{
    game->rays[column_x].distance = distance;
    game->rays[column_x].wall_hit_x = hit_x;
    game->rays[column_x].wall_hit_y = hit_y;
    game->rays[column_x].hit_vertical = is_vertical;
    game->rays[column_x].hit_type = hit_type;
}

double ray_casting(t_game *game, double radiant_angle, int column_x)
{
    t_intersect v;
    t_intersect h;
    double      dist_v;
    double      dist_h;
    char        hit_type_v;
    char        hit_type_h;

    radiant_angle = normalize_angle(radiant_angle);
    // Calcul des intersections
    v = v_intersection(game->player.x, game->player.y, radiant_angle);
    h = h_intersection(game->player.x, game->player.y, radiant_angle);
    // Avancer jusqu'aux murs
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
    // Récupérer les types d'éléments touchés
    hit_type_v = get_hit_type(&game->map, v.x, v.y);
    hit_type_h = get_hit_type(&game->map, h.x, h.y);
    // Calculer les distances
    dist_v = sqrt(pow(v.x - game->player.x, 2) + pow(v.y - game->player.y, 2));
    dist_h = sqrt(pow(h.x - game->player.x, 2) + pow(h.y - game->player.y, 2));
    // Stocker les infos du rayon le plus proche
    if (dist_v < dist_h)
    {
        store_ray_info(game, column_x, dist_v, v.x, v.y, 1, hit_type_v);
        return dist_v;
    }
    else
    {
        store_ray_info(game, column_x, dist_h, h.x, h.y, 0, hit_type_h);
        return dist_h;
    }
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
