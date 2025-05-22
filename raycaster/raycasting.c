/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:09:42 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/22 15:51:41 by abkhefif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"
int door_hit_check(double x, double y, char cell_type);

int is_door_transparent_at_point(t_game *game, double x, double y);

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
    
    // Vérification spéciale pour les portes
    if (cell_type == 'D' || cell_type == 'd')
    {
        return !door_hit_check(x, y, cell_type);
    }
    
    // Pour les autres types de cellules, comportement normal
    if (cell_type == '1' || cell_type == 'P' || cell_type == 'i')
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
        return (0); // Pas une porte
        
    // Calculer position relative dans la cellule
    int map_x = (int)(x / TILE_SIZE);
    //int map_y = (int)(y / TILE_SIZE);
    double rel_x = x / TILE_SIZE - map_x;
    //double rel_y = y / TILE_SIZE - map_y;
    
    // Pattern de porte avec barreaux (détection uniquement sur les barreaux)
    // Par exemple, détecter seulement si on est sur un barreau tous les 0.2 unités
    double bar_width = 0.02; // Largeur d'un barreau (2% de la cellule)
    double bar_spacing = 0.2; // Espacement entre barreaux (20% de la cellule)
    
    // Vérifier si on est sur un barreau vertical
    for (double bar_pos = 0.1; bar_pos < 0.9; bar_pos += bar_spacing) {
        if (fabs(rel_x - bar_pos) < bar_width)
            return (1); // Collision avec un barreau
    }
    
    return (0); // Pas de collision, le rayon traverse entre les barreaux
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

    // Initialiser le second hit
    game->rays[column_x].has_second_hit = 0;

    radiant_angle = normalize_angle(radiant_angle);
    v = v_intersection(game->player.x, game->player.y, radiant_angle);
    h = h_intersection(game->player.x, game->player.y, radiant_angle);
    epsilon = 0.00001;
    max_iterations = 1000;
    iter = 0;

    // Premier raycasting normal
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

    // Déterminer le premier hit
    double first_distance;
    double first_x, first_y;
    int first_vertical;
    char first_hit_type;
    
    if (fabs(dist_v - dist_h) < epsilon)
    {
        first_distance = dist_h;
        first_x = h.x;
        first_y = h.y;
        first_vertical = 0;
        first_hit_type = hit_type_h;
    }
    else if (dist_v < dist_h)
    {
        first_distance = dist_v;
        first_x = v.x;
        first_y = v.y;
        first_vertical = 1;
        first_hit_type = hit_type_v;
    }
    else
    {
        first_distance = dist_h;
        first_x = h.x;
        first_y = h.y;
        first_vertical = 0;
        first_hit_type = hit_type_h;
    }

    // Stocker le premier hit
    store_ray_info(game, column_x, first_distance, first_x, first_y, first_vertical, first_hit_type);

    // Si c'est une porte et qu'elle est transparente, lancer un second rayon
    if ((first_hit_type == 'D' || first_hit_type == 'd') && 
        is_door_transparent_at_point(game, first_x, first_y))
    {
        // Calculer le point de départ du second rayon (légèrement après la porte)
        double offset = 1.0; // 1 pixel après la porte
        double second_start_x = first_x + cos(radiant_angle) * offset;
        double second_start_y = first_y + sin(radiant_angle) * offset;
        
        // Lancer le second rayon depuis ce point
        t_intersect v2 = v_intersection(second_start_x, second_start_y, radiant_angle);
        t_intersect h2 = h_intersection(second_start_x, second_start_y, radiant_angle);
        
        iter = 0;
        while (is_not_wall(&game->map, v2.x, v2.y) && iter < max_iterations)
        {
            v2.x += v2.step_x;
            v2.y += v2.step_y;
            iter++;
        }
        iter = 0;
        while (is_not_wall(&game->map, h2.x, h2.y) && iter < max_iterations)
        {
            h2.x += h2.step_x;
            h2.y += h2.step_y;
            iter++;
        }
        
        double dist_v2 = sqrt(pow(v2.x - game->player.x, 2) + pow(v2.y - game->player.y, 2));
        double dist_h2 = sqrt(pow(h2.x - game->player.x, 2) + pow(h2.y - game->player.y, 2));
        
        // Stocker le second hit
        game->rays[column_x].has_second_hit = 1;
        if (dist_v2 < dist_h2)
        {
            game->rays[column_x].second_distance = dist_v2;
            game->rays[column_x].second_wall_hit_x = v2.x;
            game->rays[column_x].second_wall_hit_y = v2.y;
            game->rays[column_x].second_hit_vertical = 1;
            game->rays[column_x].second_hit_type = get_hit_type(&game->map, v2.x, v2.y);
        }
        else
        {
            game->rays[column_x].second_distance = dist_h2;
            game->rays[column_x].second_wall_hit_x = h2.x;
            game->rays[column_x].second_wall_hit_y = h2.y;
            game->rays[column_x].second_hit_vertical = 0;
            game->rays[column_x].second_hit_type = get_hit_type(&game->map, h2.x, h2.y);
        }
    }

    return first_distance;
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
