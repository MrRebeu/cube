/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:09:42 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/21 23:10:43 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

void render_open_door_ultra_thin(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
	// ✅ Trouver la porte correspondante
	int map_x = (int)(ray->wall_hit_x / TILE_SIZE);
	int map_y = (int)(ray->wall_hit_y / TILE_SIZE);
	
	t_open_door *door = find_door_at_position(game, map_x, map_y);
	if (!door || !door->sprite.ptr)
	{
		// Fallback : dessiner une ligne simple
		render_simple_door_line(game, column_x, renderer);
		return;
	}
	
	// ✅ Calculer la position dans la texture
	int tex_x = door->sprite.width / 2; // Centre de la texture pour être fin
	
	// ✅ Probabilité de rendu (pour être ultra-fin)
	int render_probability = 3; // Sur 10
	if ((column_x + (int)(ray->distance)) % 10 < render_probability)
	{
		// Dessiner la colonne
		for (int y = renderer->draw_start; y <= renderer->draw_end; y++)
		{
			if (y < 0 || y >= DISPLAY_HEIGHT)
				continue;
			
			int tex_y = ((y - renderer->draw_start) * door->sprite.height) / (renderer->draw_end - renderer->draw_start + 1);
			if (tex_y >= door->sprite.height) tex_y = door->sprite.height - 1;
			
			char *src = door->sprite.addr + tex_y * door->sprite.line_length + 
					   tex_x * (door->sprite.bits_per_pixel / 8);
			unsigned int color = *(unsigned int *)src;
			
			int red = (color >> 16) & 0xFF;
			int green = (color >> 8) & 0xFF;
			int blue = color & 0xFF;
			
			// Transparence - ne garder que les montants
			if (red >= 30 || green >= 30 || blue >= 30) // Pas complètement noir
			{
				if (!(red > 240 && green > 240 && blue > 240)) // Pas blanc
				{
					char *dst = game->screen.addr + y * game->screen.line_length + 
							   column_x * (game->screen.bits_per_pixel / 8);
					*(unsigned int *)dst = color;
				}
			}
		}
	}
}

// Fonction helper pour dessiner une ligne simple
void render_simple_door_line(t_game *game, int column_x, t_render *renderer)
{
	unsigned int door_color = 0x8B4513; // Marron pour les montants
	
	// Dessiner seulement sur certaines colonnes pour être ultra-fin
	if (column_x % 5 == 0) // 1 colonne sur 5
	{
		for (int y = renderer->draw_start; y <= renderer->draw_end; y++)
		{
			if (y < 0 || y >= DISPLAY_HEIGHT)
				continue;
			
			char *dst = game->screen.addr + y * game->screen.line_length + 
					   column_x * (game->screen.bits_per_pixel / 8);
			*(unsigned int *)dst = door_color;
		}
	}
}

// Helper pour trouver la porte
t_open_door *find_door_at_position(t_game *game, int map_x, int map_y)
{
	for (int i = 0; i < game->num_open_doors; i++)
	{
		int door_map_x = (int)(game->open_doors[i].x / TILE_SIZE);
		int door_map_y = (int)(game->open_doors[i].y / TILE_SIZE);
		
		if (door_map_x == map_x && door_map_y == map_y && game->open_doors[i].active)
			return &game->open_doors[i];
	}
	return NULL;
}

double check_open_door_on_ray(t_game *game, double radiant_angle)
{
	double ray_x = game->player.x;
	double ray_y = game->player.y;
	double ray_dx = cos(radiant_angle);
	double ray_dy = sin(radiant_angle);
	double step = 3.0; // Petit pas pour précision
	double closest_door = -1.0;
	
	// Marcher le long du rayon
	for (double distance = 0; distance < 800.0; distance += step)
	{
		ray_x += ray_dx * step;
		ray_y += ray_dy * step;
		
		int map_x = (int)(ray_x / TILE_SIZE);
		int map_y = (int)(ray_y / TILE_SIZE);
		
		// Vérifier les limites
		if (map_x < 0 || map_x >= game->map.width || 
			map_y < 0 || map_y >= game->map.height)
			break;
		
		// Si on touche un mur solide, arrêter
		if (game->map.matrix[map_y][map_x] == '1')
			break;
		
		// ✅ Si on trouve une cellule 'O' (porte ouverte)
		if (game->map.matrix[map_y][map_x] == 'O')
		{
			// Calculer la distance précise au centre de la cellule
			double cell_center_x = (map_x * TILE_SIZE) + (TILE_SIZE / 2);
			double cell_center_y = (map_y * TILE_SIZE) + (TILE_SIZE / 2);
			double door_dist = sqrt((cell_center_x - game->player.x) * (cell_center_x - game->player.x) + 
									(cell_center_y - game->player.y) * (cell_center_y - game->player.y));
			
			if (closest_door < 0 || door_dist < closest_door)
				closest_door = door_dist;
		}
	}
	
	return closest_door;
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

int	is_not_wall_for_movement(t_map *map, double x, double y)
{
	int	map_x;
	int	map_y;

	map_x = (int)(x / TILE_SIZE);
	map_y = (int)(y / TILE_SIZE);
	if (map_x < 0 || map_x >= map->width || map_y < 0 || map_y >= map->height)
		return (0);
	
	// ✅ Pour le mouvement : le joueur peut passer dans les portes ouvertes
	if (map->matrix[map_y][map_x] == '1' || map->matrix[map_y][map_x] == 'D'
		|| map->matrix[map_y][map_x] == 'P' || map->matrix[map_y][map_x] == 'i'
		|| map->matrix[map_y][map_x] == 'd' || map->matrix[map_y][map_x] == 'M'
		|| map->matrix[map_y][map_x] == 'L')
		return (0);
	
	// ✅ 'O' (porte ouverte) = le joueur PEUT passer
	return (1);
}
int is_not_wall(t_map *map, double x, double y)
{
    int map_x = (int)(x / TILE_SIZE);
    int map_y = (int)(y / TILE_SIZE);
    
    if (map_x < 0 || map_x >= map->width || map_y < 0 || map_y >= map->height)
        return (0);
    
    char cell = map->matrix[map_y][map_x];
    
    // ✅ LASER = TRANSPARENT TOTAL (pas de montants)
    if (cell == 'L')
        return (1); // ← Les rayons traversent complètement
    
    // ✅ PORTES OUVERTES = avec montants sur les bords
    if (cell == 'O' || cell == 'o')
    {
        double cell_x = fmod(x, TILE_SIZE);
        double frame_width = TILE_SIZE * 0.15;
        
        // ✅ Si on est dans la zone centrale, c'est libre
        if (cell_x > frame_width && cell_x < TILE_SIZE - frame_width)
            return (1); // Passage libre
        else
            return (0); // Montant solide
    }
    
    // ✅ Murs solides
    if (cell == '1' || cell == 'D' || cell == 'P' || cell == 'i' || cell == 'd' || cell == 'M') 
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

char	get_hit_type(t_map *map, double x, double y)
{
	int	map_x;
	int	map_y;

	map_x = (int)(x / TILE_SIZE);
	map_y = (int)(y / TILE_SIZE);
	if (map_x >= 0 && map_x < map->width && map_y >= 0 && map_y < map->height)
		return (map->matrix[map_y][map_x]);
	else
		return ('1');
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

void render_door_on_column(t_game *game, t_open_door *door, int column_x, double distance)
{
    if (!door->sprite.ptr)
        return;
    
    // ✅ Calculer la hauteur basée sur la distance RÉELLE
    double distance_to_projection = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    double door_height_world = TILE_SIZE * 1.5;
    int door_height_screen = (int)((door_height_world / distance) * distance_to_projection);
    
    if (door_height_screen < 1) door_height_screen = 1;
    if (door_height_screen > DISPLAY_HEIGHT * 2) door_height_screen = DISPLAY_HEIGHT * 2;
    
    // ✅ Position verticale centrée
    int door_top = (DISPLAY_HEIGHT / 2) - (door_height_screen / 2) + game->pitch;
    int door_bottom = door_top + door_height_screen;
    
    // ✅ Clamp aux limites d'écran
    if (door_top < 0) door_top = 0;
    if (door_bottom >= DISPLAY_HEIGHT) door_bottom = DISPLAY_HEIGHT - 1;
    
    // ✅ Utiliser le centre de la texture pour une porte fine
    int tex_x = door->sprite.width / 2;
    
    // ✅ Dessiner la colonne verticale
    for (int y = door_top; y <= door_bottom; y++)
    {
        if (y < 0 || y >= DISPLAY_HEIGHT)
            continue;
        
        // Position Y dans la texture
        int tex_y = ((y - door_top) * door->sprite.height) / (door_bottom - door_top + 1);
        if (tex_y >= door->sprite.height) tex_y = door->sprite.height - 1;
        
        // Récupérer le pixel
        char *src = door->sprite.addr + tex_y * door->sprite.line_length + 
                   tex_x * (door->sprite.bits_per_pixel / 8);
        unsigned int color = *(unsigned int *)src;
        
        int red = (color >> 16) & 0xFF;
        int green = (color >> 8) & 0xFF;
        int blue = color & 0xFF;
        
        // Transparence (ne dessiner que les montants)
        if (red >= 15 || green >= 15 || blue >= 15) // Pas noir
        {
            if (!(abs(red - 255) <= 3 && abs(green - 0) <= 3 && abs(blue - 0) <= 3)) // Pas rouge pur
            {
                if (!(red > 240 && green > 240 && blue > 240)) // Pas blanc
                {
                    char *dst = game->screen.addr + y * game->screen.line_length + 
                               column_x * (game->screen.bits_per_pixel / 8);
                    *(unsigned int *)dst = color;
                }
            }
        }
    }
}

void check_open_doors_on_ray(t_game *game, int column_x, double radiant_angle)
{
    double ray_x = game->player.x;
    double ray_y = game->player.y;
    double ray_dx = cos(radiant_angle);
    double ray_dy = sin(radiant_angle);
    double step = 5.0; // Pas de 5 pixels
    double max_distance = 1000.0;
    
    // Marcher le long du rayon
    for (double distance = 0; distance < max_distance; distance += step)
    {
        ray_x += ray_dx * step;
        ray_y += ray_dy * step;
        
        // Vérifier si on touche un mur (arrêter le rayon)
        int map_x = (int)(ray_x / TILE_SIZE);
        int map_y = (int)(ray_y / TILE_SIZE);
        
        if (map_x < 0 || map_x >= game->map.width || 
            map_y < 0 || map_y >= game->map.height ||
            game->map.matrix[map_y][map_x] == '1')
            break;
        
        // Vérifier si on est proche d'une porte ouverte
        for (int i = 0; i < game->num_open_doors; i++)
        {
            t_open_door *door = &game->open_doors[i];
            if (!door->active)
                continue;
            
            double door_distance = sqrt((ray_x - door->x) * (ray_x - door->x) + 
                                       (ray_y - door->y) * (ray_y - door->y));
            
            // Si on est très proche de la porte (dans un rayon de 20 pixels)
            if (door_distance < 20.0)
            {
                // Rendre la porte sur cette colonne
                render_door_on_column(game, door, column_x, distance);
                return;
            }
        }
    }
}

double ray_casting(t_game *game, double radiant_angle, int column_x)
{
    t_intersect v;
    t_intersect h;
    double epsilon = 0.00001;
    int max_iterations = 1000;
    int iter = 0;
    double dist_v, dist_h;
    char hit_type_v, hit_type_h;

    radiant_angle = normalize_angle(radiant_angle);
    v = v_intersection(game->player.x, game->player.y, radiant_angle);
    h = h_intersection(game->player.x, game->player.y, radiant_angle);

    // ✅ FONCTION POUR AJUSTER LA POSITION DES MURS TYPE 'D'
    

    // Raycasting vertical
    iter = 0;
    while (iter < max_iterations)
    {
        if (!is_not_wall(&game->map, v.x, v.y))
            break;
        v.x += v.step_x;
        v.y += v.step_y;
        iter++;
    }

    // Raycasting horizontal
    iter = 0;
    while (iter < max_iterations)
    {
        if (!is_not_wall(&game->map, h.x, h.y))
            break;
        h.x += h.step_x;
        h.y += h.step_y;
        iter++;
    }

    // ✅ AJUSTER LES POSITIONS POUR LES MURS TYPE 'D'
    hit_type_v = get_hit_type(&game->map, v.x, v.y);
    hit_type_h = get_hit_type(&game->map, h.x, h.y);
    // adjust_door_position(&v.x, &v.y, hit_type_v, 1); // 1 = vertical
    // adjust_door_position(&h.x, &h.y, hit_type_h, 0); // 0 = horizontal

    // Recalculer les distances après ajustement
    dist_v = sqrt(pow(v.x - game->player.x, 2) + pow(v.y - game->player.y, 2));
    dist_h = sqrt(pow(h.x - game->player.x, 2) + pow(h.y - game->player.y, 2));

    if (fabs(dist_v - dist_h) < epsilon)
    {
        store_ray_info(game, column_x, dist_h, h.x, h.y, 0, hit_type_h);
        return (dist_h);
    }
    if (dist_v < dist_h)
    {
        store_ray_info(game, column_x, dist_v, v.x, v.y, 1, hit_type_v);
        return (dist_v);
    }
    else
    {
        store_ray_info(game, column_x, dist_h, h.x, h.y, 0, hit_type_h);
        return (dist_h);
    }
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
