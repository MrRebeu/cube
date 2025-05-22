/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:09:42 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/22 20:30:45 by abkhefif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"
int door_hit_check(double x, double y, char cell_type);

int is_door_transparent_at_point(t_game *game, double x, double y);

int is_door_texture_transparent_at_position(t_game *game, double hit_y, char door_type)
{
    // Calculer tex_x et tex_y pour cette position
    int tex_x = TILE_SIZE / 2; // Centre horizontal (thin wall)
    int tex_y = ((int)hit_y) % TILE_SIZE;
    
    // Clamp les valeurs
    if (tex_y < 0) tex_y = 0;
    else if (tex_y >= TILE_SIZE) tex_y = TILE_SIZE - 1;
    
    // Choisir la texture selon le type
    t_img *texture;
    if (door_type == 'd')
        texture = &game->map.door_shooted_texture;
    else
        texture = &game->map.door_texture;
    
    // Lire le pixel de la texture
    char *tex_addr = texture->addr
        + (tex_y * texture->line_length
           + tex_x * (texture->bits_per_pixel / 8));
    unsigned int color = *(unsigned int *)tex_addr;
    
    // Vérifier si c'est rouge (transparent)
    unsigned int red = (color >> 16) & 0xFF;
    unsigned int green = (color >> 8) & 0xFF;
    unsigned int blue = color & 0xFF;
    
    return (red > 200 && green < 50 && blue < 50);
}

int ray_intersects_vertical_line(double ray_start_x, double ray_start_y, 
                                 double ray_angle, double line_x, 
                                 double *hit_x, double *hit_y)
{
    // Si le rayon est parfaitement horizontal, pas d'intersection possible
    if (fabs(cos(ray_angle)) < 0.000001)
        return (0);
    
    // Calculer le point d'intersection
    *hit_x = line_x;
    *hit_y = ray_start_y + (line_x - ray_start_x) * tan(ray_angle);
    
    // Vérifier que l'intersection est dans la bonne direction
    double dx = *hit_x - ray_start_x;
    if ((cos(ray_angle) > 0 && dx < 0) || (cos(ray_angle) < 0 && dx > 0))
        return (0);
    
    return (1);
}

int is_door_transparent_at_point(t_game *game, double x, double y)
{
    int map_x = (int)(x / TILE_SIZE);
    int map_y = (int)(y / TILE_SIZE);
    
    if (map_x < 0 || map_x >= game->map.width || map_y < 0 || map_y >= game->map.height)
        return (0);
    
    char cell_type = game->map.matrix[map_y][map_x];
    if (cell_type != 'D' && cell_type != 'd')
        return (0);
    
    // Calculer les coordonnées de texture comme dans render_door
    int tex_x, tex_y;
    double rel_x = x / TILE_SIZE - map_x;
    double rel_y = y / TILE_SIZE - map_y;
    
    tex_x = (int)(rel_x * TILE_SIZE);
    tex_y = (int)(rel_y * TILE_SIZE);
    
    // Clamp les valeurs
    if (tex_x < 0) tex_x = 0;
    else if (tex_x >= TILE_SIZE) tex_x = TILE_SIZE - 1;
    if (tex_y < 0) tex_y = 0;
    else if (tex_y >= TILE_SIZE) tex_y = TILE_SIZE - 1;
    
    // Lire le pixel de la texture
    char *tex_addr = game->map.door_texture.addr
        + (tex_y * game->map.door_texture.line_length
           + tex_x * (game->map.door_texture.bits_per_pixel / 8));
    unsigned int color = *(unsigned int *)tex_addr;
    
    // Vérifier si c'est transparent (noir)
    unsigned int red = (color >> 16) & 0xFF;
    unsigned int green = (color >> 8) & 0xFF;
    unsigned int blue = color & 0xFF;
    
    return (red < 10 && green < 10 && blue < 10); // Retourne 1 si transparent
}

t_intersect	v_intersection(int x_player, int y_player, double radiant_angle)
{
	t_intersect	v;

	double first_x, first_y;
	v.x = x_player;
	v.y = y_player;
	v.step_x = 0;
	v.step_y = 0;
	if (fabs(cos(radiant_angle)) < 0.000001)
		return (v);
	if (cos(radiant_angle) > 0)
	{
		first_x = floor(x_player / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
		v.step_x = TILE_SIZE;
	}
	else
	{
		first_x = floor(x_player / TILE_SIZE) * TILE_SIZE;
		v.step_x = -TILE_SIZE;
		first_x -= 0.001;
	}
	first_y = y_player + (first_x - x_player) * tan(radiant_angle);
	v.step_y = tan(radiant_angle) * v.step_x;
	v.x = first_x;
	v.y = first_y;
	return (v);
}

t_intersect	h_intersection(int x_player, int y_player, double radiant_angle)
{
	t_intersect	h;

	double first_x, first_y;
	h.x = x_player;
	h.y = y_player;
	h.step_x = 0;
	h.step_y = 0;
	if (fabs(sin(radiant_angle)) < 0.000001)
		return (h);
	if (sin(radiant_angle) > 0)
	{
		first_y = floor(y_player / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
		h.step_y = TILE_SIZE;
	}
	else
	{
		first_y = floor(y_player / TILE_SIZE) * TILE_SIZE;
		h.step_y = -TILE_SIZE;
		first_y -= 0.001;
	}
	if (fabs(tan(radiant_angle)) > 0.000001)
		first_x = x_player + (first_y - y_player) / tan(radiant_angle);
	else
		first_x = x_player;
	h.step_x = h.step_y / tan(radiant_angle);
	h.x = first_x;
	h.y = first_y;
	return (h);
}

int is_not_wall(t_map *map, double x, double y)
{
    int map_x;
    int map_y;
    char cell_type;

    map_x = (int)(x / TILE_SIZE);
    map_y = (int)(y / TILE_SIZE);
    
    if (map_x < 0 || map_x >= map->width || map_y < 0 || map_y >= map->height)
        return (0);
        
    cell_type = map->matrix[map_y][map_x];
    
    if (cell_type == 'd')  // Seulement 'd' = thin wall
        return (1);
    
    // 'D' traité comme mur normal
    if (cell_type == '1' || cell_type == 'P' || cell_type == 'i' || cell_type == 'D')
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
int door_hit_check(double x, double y, char cell_type)
{
	(void)y;
    if (cell_type != 'D' && cell_type != 'd')
        return (0);
        
    // Position relative dans la cellule (0.0 à 1.0)
    int map_x = (int)(x / TILE_SIZE);
    double rel_x = x / TILE_SIZE - map_x;
    
    // Ouverture au centre : de 0.3 à 0.7 (40% d'ouverture)
    if (rel_x >= 0.3 && rel_x <= 0.7)
        return (0); // Pas de collision = rayon passe à travers
    else
        return (1); // Collision avec le cadre de la porte
}

char get_hit_type(t_map *map, double x, double y)
{
    int map_x;
    int map_y;
    char cell_type;

    map_x = (int)(x / TILE_SIZE);
    map_y = (int)(y / TILE_SIZE);
    
    if (map_x >= 0 && map_x < map->width && map_y >= 0 && map_y < map->height)
    {
        cell_type = map->matrix[map_y][map_x];
        
        // Vérification spéciale pour les portes
        if (cell_type == 'D' || cell_type == 'd')
        {
            // Si le rayon frappe la partie centrale, retourner le type de porte
            if (door_hit_check(x, y, cell_type))
                return cell_type;
            else
                return '0'; // Sinon, considérer comme un espace vide
        }
        
        return cell_type;
    }
    else
        return '1';
}

void	store_ray_info(t_game *game, int column_x, double distance,
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
    double      epsilon;
    int         max_iterations;
    int         iter;
    double      dist_v;
    double      dist_h;
    char        hit_type_v;
    char        hit_type_h;
    
    // Variables pour thin walls
    double      thin_wall_dist = INFINITY;
    double      thin_wall_hit_x = 0;
    double      thin_wall_hit_y = 0;
    char        thin_wall_type = '0';
    int         thin_wall_found = 0;

    radiant_angle = normalize_angle(radiant_angle);
    
    // OPTIMISATION 1: Augmenter le step_size et réduire max_distance
    double current_x = game->player.x;
    double current_y = game->player.y;
    double step_size = 4.0; // CHANGÉ: 1.0 → 4.0 (4x plus rapide)
    double max_search_distance = 800.0; // CHANGÉ: 2000.0 → 800.0 (2.5x plus rapide)
    
    // OPTIMISATION 2: Limiter le nombre d'itérations
    int max_thin_wall_iterations = 200; // Maximum 200 itérations au lieu de 2000
    int thin_wall_iter = 0;
    
    for (double d = step_size; d < max_search_distance && thin_wall_iter < max_thin_wall_iterations; d += step_size)
    {
        thin_wall_iter++;
        
        current_x = game->player.x + cos(radiant_angle) * d;
        current_y = game->player.y + sin(radiant_angle) * d;
        
        int cell_x = (int)(current_x / TILE_SIZE);
        int cell_y = (int)(current_y / TILE_SIZE);
        
        if (cell_x >= 0 && cell_x < game->map.width && 
            cell_y >= 0 && cell_y < game->map.height)
        {
            char cell_type = game->map.matrix[cell_y][cell_x];
            
            if (cell_type == 'D' || cell_type == 'd')
            {
                double door_center_x = cell_x * TILE_SIZE + TILE_SIZE / 2.0;
                double hit_x, hit_y;
                
                if (ray_intersects_vertical_line(game->player.x, game->player.y,
                                               radiant_angle, door_center_x, &hit_x, &hit_y))
                {
                    // Vérifier que l'intersection est dans les limites de la cellule
                    if (hit_y >= cell_y * TILE_SIZE && hit_y <= (cell_y + 1) * TILE_SIZE)
                    {
                        // OPTIMISATION 3: Test de transparence plus rapide
                        // Au lieu de lire la texture pixel par pixel, approximation
                        int tex_y_approx = ((int)hit_y) % TILE_SIZE;
                        int is_transparent = 0;
                        
                        // Zone transparente approximative (centre vertical)
                        if (tex_y_approx >= TILE_SIZE/3 && tex_y_approx <= 2*TILE_SIZE/3)
                        {
                            is_transparent = 1;
                        }
                        
                        if (is_transparent)
                        {
                            // Transparent ! Le rayon continue
                            continue;
                        }
                        else
                        {
                            // Opaque ! C'est un vrai hit
                            double distance = sqrt(pow(hit_x - game->player.x, 2) + 
                                                 pow(hit_y - game->player.y, 2));
                            
                            thin_wall_dist = distance;
                            thin_wall_hit_x = hit_x;
                            thin_wall_hit_y = hit_y;
                            thin_wall_type = cell_type;
                            thin_wall_found = 1;
                            break; // Arrêter la recherche
                        }
                    }
                }
            }
        }
    }
    
    // Phase 2: Raycasting classique pour les murs pleins (INCHANGÉ)
    v = v_intersection(game->player.x, game->player.y, radiant_angle);
    h = h_intersection(game->player.x, game->player.y, radiant_angle);
    epsilon = 0.00001;
    max_iterations = 1000;
    iter = 0;

    while (is_not_wall(&game->map, v.x, v.y) && iter < max_iterations)
    {
        v.x += v.step_x;
        v.y += v.step_y;
        iter++;
    }
    iter = 0;
    while (is_not_wall(&game->map, h.x, h.y) && iter < max_iterations)
    {
        h.x += h.step_x;
        h.y += h.step_y;
        iter++;
    }

    dist_v = sqrt(pow(v.x - game->player.x, 2) + pow(v.y - game->player.y, 2));
    dist_h = sqrt(pow(h.x - game->player.x, 2) + pow(h.y - game->player.y, 2));
    hit_type_v = get_hit_type(&game->map, v.x, v.y);
    hit_type_h = get_hit_type(&game->map, h.x, h.y);

    // Phase 3: Déterminer le hit le plus proche (INCHANGÉ)
    double final_distance;
    double final_x, final_y;
    int final_vertical;
    char final_hit_type;
    
    double min_classic_dist = fmin(dist_v, dist_h);
    
    if (thin_wall_found && thin_wall_dist < min_classic_dist)
    {
        // Thin wall opaque est plus proche
        final_distance = thin_wall_dist;
        final_x = thin_wall_hit_x;
        final_y = thin_wall_hit_y;
        final_vertical = 1;
        final_hit_type = thin_wall_type;
    }
    else
    {
        // Intersection classique plus proche
        if (fabs(dist_v - dist_h) < epsilon)
        {
            final_distance = dist_h;
            final_x = h.x;
            final_y = h.y;
            final_vertical = 0;
            final_hit_type = hit_type_h;
        }
        else if (dist_v < dist_h)
        {
            final_distance = dist_v;
            final_x = v.x;
            final_y = v.y;
            final_vertical = 1;
            final_hit_type = hit_type_v;
        }
        else
        {
            final_distance = dist_h;
            final_x = h.x;
            final_y = h.y;
            final_vertical = 0;
            final_hit_type = hit_type_h;
        }
    }

    store_ray_info(game, column_x, final_distance, final_x, final_y, final_vertical, final_hit_type);
    return final_distance;
}


double	no_fish_eye(double min_distance, double radiant_angle,
		double player_angle)
{
	double	angle_diff;
	double	corrected_dist;

	angle_diff = radiant_angle - player_angle;
	while (angle_diff > M_PI)
		angle_diff -= 2 * M_PI;
	while (angle_diff < -M_PI)
		angle_diff += 2 * M_PI;
	corrected_dist = min_distance * (cos(angle_diff));
	return (corrected_dist);
}
int	calc_wall_height(double corrected_dist)
{
	double	distance_to_projection_plane;
	double	wall_height;

	distance_to_projection_plane = (DISPLAY_WIDTH / 2.0) / tan(FOV / 2.0);
	corrected_dist = fmax(corrected_dist, 0.1);
	wall_height = (TILE_SIZE / corrected_dist) * distance_to_projection_plane;
	return ((int)fmin(wall_height, DISPLAY_HEIGHT * 3));
}
