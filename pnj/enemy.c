/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 17:39:45 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/22 18:26:15 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

int load_enemy_animations(t_game *game, t_enemy *enemy)
{
    int width, height;
    
    // Charger sprite 0
    enemy->walk_morty[0].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_walk.xpm", &width, &height);
    if (!enemy->walk_morty[0].ptr)
        return (0);
        
    enemy->walk_morty[0].width = width;
    enemy->walk_morty[0].height = height;
    enemy->walk_morty[0].addr = mlx_get_data_addr(enemy->walk_morty[0].ptr,
        &enemy->walk_morty[0].bits_per_pixel,
        &enemy->walk_morty[0].line_length,
        &enemy->walk_morty[0].endian);
        
    // Charger sprite 1
    enemy->walk_morty[1].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_walk01.xpm", &width, &height);
    if (!enemy->walk_morty[1].ptr)
        return (0);
        
    enemy->walk_morty[1].width = width;
    enemy->walk_morty[1].height = height;
    enemy->walk_morty[1].addr = mlx_get_data_addr(enemy->walk_morty[1].ptr,
        &enemy->walk_morty[1].bits_per_pixel,
        &enemy->walk_morty[1].line_length,
        &enemy->walk_morty[1].endian);
        

	    enemy->shoot_morty[0].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_shoot.xpm", &width, &height);
    if (!enemy->shoot_morty[0].ptr)
        return (0);
        
    enemy->shoot_morty[0].width = width;
    enemy->shoot_morty[0].height = height;
    enemy->shoot_morty[0].addr = mlx_get_data_addr(enemy->shoot_morty[0].ptr,
        &enemy->shoot_morty[0].bits_per_pixel,
        &enemy->shoot_morty[0].line_length,
        &enemy->shoot_morty[0].endian);
        
    // Charger sprite 1
    enemy->shoot_morty[1].ptr = mlx_xpm_file_to_image(game->mlx,
        "./texture/morty_shoot01.xpm", &width, &height);
    if (!enemy->shoot_morty[1].ptr)
        return (0);
        
    enemy->shoot_morty[1].width = width;
    enemy->shoot_morty[1].height = height;
    enemy->shoot_morty[1].addr = mlx_get_data_addr(enemy->shoot_morty[1].ptr,
        &enemy->shoot_morty[1].bits_per_pixel,
        &enemy->shoot_morty[1].line_length,
        &enemy->shoot_morty[1].endian);	
    return (1);
}

void update_enemy_animation(t_enemy *enemy)
{
    if (!enemy->active || enemy->state == DEAD)
        return;

    if (enemy->state == SEARCH || enemy->state == IDLE)
    {
        enemy->animation.frame_counter++;
        if (enemy->animation.frame_counter >= ANIMATION_SPEED)
        {
            enemy->animation.current_frame = (enemy->animation.current_frame + 1) % 2;
            enemy->animation.frame_counter = 0;
        }
    }
    else if (enemy->state == SHOOT)
    {
        enemy->animation.frame_counter++;
        if (enemy->animation.frame_counter >= ANIMATION_SPEED)
        {
            // Si tu as plusieurs frames pour le shoot, boucle dessus, sinon force 0
            enemy->animation.current_frame = (enemy->animation.current_frame + 1) % 2;
            enemy->animation.frame_counter = 0;
        }
    }
    else
    {
        enemy->animation.current_frame = 0;
        enemy->animation.frame_counter = 0;
    }
}

void calculate_enemy_transform(t_game *game, t_enemy *enemy, t_render *render)
{
    double dx, dy, inv_det;
    
    // AVANT : multiplication par TILE_SIZE car enemy était en cellules
    // dx = enemy->x * TILE_SIZE - game->player.x;
    
    // APRÈS : les deux sont en pixels, simple soustraction !
    dx = enemy->x - game->player.x;
    dy = enemy->y - game->player.y;
    
    // Le reste ne change pas
    inv_det = 1.0f / (game->player.plane_x * game->player.dir_y - 
                      game->player.dir_x * game->player.plane_y);
    render->floor_x = inv_det * (game->player.dir_y * dx - game->player.dir_x * dy);
    render->floor_y = inv_det * (-game->player.plane_y * dx + game->player.plane_x * dy);
}

void calculate_enemy_screen_pos(t_game *game, t_render *render)
{
    // Position X sur l'écran
    render->x = (int)((DISPLAY_WIDTH / 2) * (1 + render->floor_x / render->floor_y));
    
    // EXACTEMENT la même formule que calc_wall_height()
    double distance_to_projection_plane = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    double corrected_dist = fmax(render->floor_y, 0.1);
    
    // L'ennemi fait 80% de TILE_SIZE (64 * 0.8 = 51.2)
    double enemy_height = TILE_SIZE * 1.4;
    
    // Formule IDENTIQUE aux murs
    render->sprite_size = (int)((enemy_height / corrected_dist) * distance_to_projection_plane);
    
    // Limites de sécurité uniquement
    if (render->sprite_size > DISPLAY_HEIGHT * 2)
        render->sprite_size = DISPLAY_HEIGHT * 2;
    if (render->sprite_size < 1)
        render->sprite_size = 1;
}

int check_enemy_occlusion(t_game *game, t_render *render)
{
    int sprite_left_edge;
    int sprite_start_pixel;
    int sprite_end_pixel;
    int sample_count;
    int col;

    // ← TU MANQUES CES CALCULS !
    // 1. Calculer le bord gauche du sprite
    sprite_left_edge = render->x - render->sprite_size / 2;
    
    // 2. Vérifier les limites à gauche
    if (sprite_left_edge < 0)
        sprite_start_pixel = 0;
    else
        sprite_start_pixel = sprite_left_edge;
        
    // 3. Calculer le bord droit
    sprite_end_pixel = sprite_left_edge + render->sprite_size - 1;
    
    // 4. Vérifier les limites à droite
    if (sprite_end_pixel >= DISPLAY_WIDTH)
        sprite_end_pixel = DISPLAY_WIDTH - 1;

    // Maintenant le reste de ton code marche
    sample_count = 0;
    for (int i = 0; i < 5; i++)
    {
        col = sprite_start_pixel + i * (sprite_end_pixel - sprite_start_pixel) / 4;
        if (render->floor_y < game->depth_buffer[col])
            sample_count++;
    }
    //return (1);
    return (sample_count > 0);
}

void setup_enemy_render_params(t_game *game, t_render *render)
{
    int y_offset;
    
    render->draw_start = render->x - render->sprite_size / 2;
    
    y_offset = 55;
    
    render->draw_end = (DISPLAY_HEIGHT - render->sprite_size) / 2 + game->pitch + y_offset;
}

void render_enemy(t_game *game, t_enemy *enemy)
{
    t_render renderer;
    t_img *current_sprite;
    int is_visible;
    
    if (enemy->state == DEAD)
        return;
    
    update_enemy_animation(enemy);
	if (enemy->state == SHOOT)
		current_sprite = &enemy->shoot_morty[enemy->animation.current_frame];
	else
    	current_sprite = &enemy->walk_morty[enemy->animation.current_frame];
    
    calculate_enemy_transform(game, enemy, &renderer);
    
    if (renderer.floor_y <= 0.2f)
        return;
    
    calculate_enemy_screen_pos(game, &renderer);
    
    // Vérifier si hors écran
    if (renderer.x <
	 0 || renderer.x >= DISPLAY_WIDTH)
        return;
    
    // AJOUTER CES LIGNES :
    is_visible = check_enemy_occlusion(game, &renderer);
    if (!is_visible)
        return;
    
    setup_enemy_render_params(game, &renderer);
    draw_enemy_sprite(game, current_sprite, 
                     (t_point){renderer.draw_start, renderer.draw_end}, 
                     renderer.sprite_size);
}

int enemy_sees_you(t_enemy *enemy, t_player *player, t_map *map)
{
    double dx, dy, angle_to_player, delta_angle, fov;
    
    // AVANT : conversion du joueur en cellules
    // dx = (player->x / TILE_SIZE) - enemy->x;
    
    // APRÈS : tout en pixels
    dx = player->x - enemy->x;
    dy = player->y - enemy->y;
    
    angle_to_player = atan2(dy, dx);
    delta_angle = normalize_angle(angle_to_player - enemy->angle);
    fov = M_PI; // 180° de vision
    
    if (fabs(delta_angle) < fov / 2)
    {
        // line_of_sight attend des pixels maintenant
        if (line_of_sight(enemy->x, enemy->y, player->x, player->y, map))
            return (1);
    }
    return (0);
}


int line_of_sight(double ex, double ey, double px, double py, t_map *map)
{
    double dx = px - ex;
    double dy = py - ey;
    double distance = sqrt(dx * dx + dy * dy);
    double step_x = (dx / distance) * 5.0;  // Step de 5 pixels
    double step_y = (dy / distance) * 5.0;
    double x = ex;
    double y = ey;
    double traveled = 0.0;
    int map_x, map_y;
    
    while (traveled < distance)
    {
        // Convertir pixels → cellules pour vérifier la map
        map_x = (int)(x / TILE_SIZE);
        map_y = (int)(y / TILE_SIZE);
        
        if (map_x < 0 || map_x >= map->width || 
            map_y < 0 || map_y >= map->height)
            return (0);
            
        if (map->matrix[map_y][map_x] == '1')
            return (0);
            
        x += step_x;
        y += step_y;
        traveled += 5.0;  // On avance de 5 pixels
    }
    return (1);
}

void update_enemy(t_enemy *enemy, t_player *player, t_map *map)
{
    // Tout est maintenant en pixels !
    double dx = player->x - enemy->x;
    double dy = player->y - enemy->y;
    double distance = sqrt(dx * dx + dy * dy);
    
    if (enemy->health <= 0)
    {
        enemy->state = DEAD;
        return;
    }
    
    // Passer les valeurs en pixels aux fonctions
    if (enemy->state == IDLE)
        idle(enemy, player, map, dx, dy, distance);
    else if (enemy->state == SEARCH)
        search(enemy, player, map, dx, dy, distance);
    else if (enemy->state == SHOOT)
        shoot(enemy, player, map, dx, dy, distance);
    else if (enemy->state == MELEE)
        melee(enemy, player, map, dx, dy, distance);
}


void idle(t_enemy *e, t_player *p, t_map *m, double dx, double dy, double d)
{
    double move_x, move_y;
    double speed = 1.5;  // AVANT: 0.02 cellule, APRÈS: 1.5 pixels par frame
    int next_x, next_y;
    
    (void)dx; (void)dy; (void)d;
    
    move_x = cos(e->angle) * speed;
    move_y = sin(e->angle) * speed;
    
    // Vérifier la collision en pixels
    next_x = (int)((e->x + move_x) / TILE_SIZE);
    next_y = (int)((e->y + move_y) / TILE_SIZE);
    
    if (next_x >= 0 && next_x < m->width && 
        next_y >= 0 && next_y < m->height &&
        m->matrix[next_y][next_x] != '1')
    {
        e->x += move_x;  // Ajouter des PIXELS
        e->y += move_y;
    }
    else
    {
        // Changer de direction aléatoirement
        e->angle += ((rand() % 60) - 30) * M_PI / 180;
        e->angle = normalize_angle(e->angle);
    }
    
    if (enemy_sees_you(e, p, m))
    {
        e->state = SEARCH;
        e->sees_player = 1;
    }
}

void shoot(t_enemy *e, t_player *p, t_map *m, double dx, double dy, double d)
{
    double angle_to_player;
    double distance_in_tiles = d / TILE_SIZE;
    
    // Si le joueur n'est plus visible, retourner en recherche
    if (!enemy_sees_you(e, p, m))
    {
        e->state = SEARCH;
        e->sees_player = 0;
        return;
    }
    
    // Changement d'état selon la distance
    if (distance_in_tiles >= 6.0)
    {
        e->state = SEARCH;
        return;
    }
    
    if (distance_in_tiles < 1.5)
    {
        e->state = MELEE;
        e->cooldown = 0;
        return;
    }
    
    // Orienter vers le joueur
    angle_to_player = atan2(dy, dx);
    e->angle = angle_to_player;
    
    // NOUVELLE GESTION DU TIR
    if (e->cooldown <= 0)
    {
        p->health -= 10;
        
        // AVANT : 30 frames = 0.5 secondes
        // APRÈS : 180 frames = 3 secondes à 60 FPS
        e->cooldown = 180;  // ← Un tir toutes les 3 secondes
        
        // Alternative : utiliser un temps plus long
        // e->cooldown = 180;  // 3 secondes
        // e->cooldown = 240;  // 4 secondes
    }
    else
        e->cooldown--;
}

void search(t_enemy *e, t_player *p, t_map *m, double dx, double dy, double d)
{
    double move_x, move_y, angle_to_player;
    double speed = 3.0;  // Plus rapide quand il te cherche
    int next_x, next_y;
    
    // dx et dy sont maintenant en pixels (depuis update_enemy)
    
    if (!enemy_sees_you(e, p, m))
    {
        e->state = IDLE;
        e->sees_player = 0;
        return;
    }
    
    // Distances en TILES pour les états
    double distance_in_tiles = d / TILE_SIZE;
    
    if (distance_in_tiles < 1.5)
    {
        e->state = MELEE;
        e->cooldown = 0;
        return;
    }
    
    if (distance_in_tiles < 6)
    {
        e->state = SHOOT;
        e->cooldown = 0;
        return;
    }
    
    angle_to_player = atan2(dy, dx);
    e->angle = angle_to_player;
    
    move_x = cos(e->angle) * speed;
    move_y = sin(e->angle) * speed;
    
    next_x = (int)((e->x + move_x) / TILE_SIZE);
    next_y = (int)((e->y + move_y) / TILE_SIZE);
    
    if (next_x >= 0 && next_x < m->width && 
        next_y >= 0 && next_y < m->height &&
        m->matrix[next_y][next_x] != '1')
    {
        e->x += move_x;
        e->y += move_y;
    }
}

void melee(t_enemy *e, t_player *p, t_map *m, double dx, double dy, double d)
{
    double angle_to_player;
    double distance_in_tiles = d / TILE_SIZE;
    
    if (!enemy_sees_you(e, p, m) || distance_in_tiles >= 1.5)
    {
        e->state = SEARCH;
        return;
    }
    
    angle_to_player = atan2(dy, dx);
    e->angle = angle_to_player;
    
    if (e->cooldown <= 0)
    {
        p->health -= 25;  // Plus de dégâts car plus proche
        
        // AVANT : 30 frames = 0.5 secondes
        // APRÈS : 90 frames = 1.5 secondes à 60 FPS
        e->cooldown = 90;  // ← Attaque de mêlée toutes les 1.5 secondes
    }
    else
        e->cooldown--;
}

void draw_enemy_sprite(t_game *game, t_img *sprite, t_point pos, int size)
{
	int i;
	int j;
	int src_x;
	int src_y;
	char *src;
	unsigned int color;
	int x;
	int y;
	char *dst;

	// Protezione base
	if (!sprite || !sprite->addr || size <= 0)
		return;
	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			// 1) Mappatura texture
			src_x = i * sprite->width / size;
			src_y = j * sprite->height / size;
			// clamp
			if (src_x < 0)
				src_x = 0;
			else if (src_x >= sprite->width)
				src_x = sprite->width - 1;
			if (src_y < 0)
				src_y = 0;
			else if (src_y >= sprite->height)
				src_y = sprite->height - 1;
			// 2) Leggi pixel
			src = sprite->addr + src_y * sprite->line_length + src_x * (sprite->bits_per_pixel / 8);
			color = *(unsigned int *)src;
			// 3) Trasparenza
			int red   = (color >> 16) & 0xFF;
			int green = (color >> 8) & 0xFF;
			int blue  = color & 0xFF;

			// Couleur cible à ignorer : rouge pur
			int target_red = 255;
			int target_green = 0;
			int target_blue = 0;

			// Tolérance (par exemple : ignorer tout ce qui est très rouge)
			int tolerance = 2;

			if (abs(red - target_red) <= tolerance &&
				abs(green - target_green) <= tolerance &&
				abs(blue - target_blue) <= tolerance)
			{
				j++;
				continue;
			}
			// 4) Posizione sullo schermo
			x = pos.x + i;
			y = pos.y + j;
			// 5) Clipping
			if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT)
			{
				j++;
				continue;
			}
			// 6) Disegna
			dst = game->screen.addr + y * game->screen.line_length + x * (game->screen.bits_per_pixel / 8);
			*(unsigned int *)dst = color;
			j++;
		}
		i++;
	}
}
void update_camera_vectors(t_player *player)
{
	double fov_half;

	player->dir_x = cos(player->angle);
	player->dir_y = sin(player->angle);
	fov_half = player->fov / 2.0;
	player->plane_x = -sin(player->angle) * tan(fov_half);
	player->plane_y = cos(player->angle) * tan(fov_half);
}

void update_enemy_position_on_map(t_game *game, t_enemy *enemy, double old_x, double old_y)
{
	int old_map_x = (int)(old_x);
	int old_map_y = (int)(old_y);
	int new_map_x = (int)(enemy->x);
	int new_map_y = (int)(enemy->y);

	// Rimuovi il nemico dalla vecchia posizione se è cambiata
	if (old_map_x != new_map_x || old_map_y != new_map_y)
	{
		// CORREZIONE: Rimuovi 'M' dalla vecchia posizione solo se c'è davvero un nemico lì
		if (old_map_x >= 0 && old_map_x < game->map.width &&
			old_map_y >= 0 && old_map_y < game->map.height)
		{
			if (game->map.matrix[old_map_y][old_map_x] == 'M')
			{
				// Controlla se c'è ancora un nemico in questa posizione
				int nemico_presente = 0;
				for (int i = 0; i < game->num_enemies; i++)
				{
					if (game->enemies[i].active &&
						(int)(game->enemies[i].x) == old_map_x &&
						(int)(game->enemies[i].y) == old_map_y)
					{
						nemico_presente = 1;
						break;
					}
				}
				// Solo se non c'è più nessun nemico, rimuovi la 'M'
				if (!nemico_presente)
					game->map.matrix[old_map_y][old_map_x] = '0';
			}
		}

		// Aggiungi il nemico alla nuova posizione
		if (new_map_x >= 0 && new_map_x < game->map.width &&
			new_map_y >= 0 && new_map_y < game->map.height && enemy->active)
		{
			game->map.matrix[new_map_y][new_map_x] = 'M';
		}
	}
}