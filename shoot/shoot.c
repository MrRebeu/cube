/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shoot.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 22:23:45 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/22 19:20:54 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

void calculate_shoot(t_game *game)
{
    double impact_x;
    double impact_y;
    int map_x;
    int map_y;
    int portal_orientation;
    int center_ray_index;
    t_ray *center_ray;

    center_ray_index = DISPLAY_WIDTH / 2;
    center_ray = &game->rays[center_ray_index];

    if (game->current_weapon == PORTALGUN)
    {
        if (center_ray->hit_type == '1')
        {
            impact_x = center_ray->wall_hit_x;
            impact_y = center_ray->wall_hit_y;
            map_x = (int)(impact_x / TILE_SIZE);
            map_y = (int)(impact_y / TILE_SIZE);
            
            // CALCULER L'ORIENTATION DU PORTAIL (vers quelle direction il "regarde")
            if (center_ray->hit_vertical)
            {
                if (cos(center_ray->radiant_angle) > 0)
                    portal_orientation = 3; // Mur à l'ouest → portail regarde vers l'ouest
                else
                    portal_orientation = 1; // Mur à l'est → portail regarde vers l'est
            }
            else
            {
                if (sin(center_ray->radiant_angle) > 0)
                    portal_orientation = 0; // Mur au nord → portail regarde vers le nord
                else
                    portal_orientation = 2; // Mur au sud → portail regarde vers le sud
            }
            
            // PLACER LE PORTAIL ET STOCKER SON ORIENTATION
            if (game->portal_count < 2)
            {
                game->map.matrix[map_y][map_x] = 'P';
                
                // Stocker dans le bon portail
                if (game->portal_count == 0)
                {
                    game->portal_1.x = (map_x * TILE_SIZE) + (TILE_SIZE / 2);
                    game->portal_1.y = (map_y * TILE_SIZE) + (TILE_SIZE / 2);
                    game->portal_1.is_active = 1;
                    game->portal_1.orientation = portal_orientation;
                }
                else
                {
                    game->portal_2.x = (map_x * TILE_SIZE) + (TILE_SIZE / 2);
                    game->portal_2.y = (map_y * TILE_SIZE) + (TILE_SIZE / 2);
                    game->portal_2.is_active = 1;
                    game->portal_2.orientation = portal_orientation;
                }
                
                game->portal_count++;
                printf("Portal %d créé à [%d, %d], orientation: %s\n", 
                       game->portal_count, map_x, map_y,
                       (portal_orientation == 0) ? "Nord" :
                       (portal_orientation == 1) ? "Est" :
                       (portal_orientation == 2) ? "Sud" : "Ouest");
            }
            else
            {
                remove_all_portals(game);
                game->map.matrix[map_y][map_x] = 'P';
                
                game->portal_1.x = (map_x * TILE_SIZE) + (TILE_SIZE / 2);
                game->portal_1.y = (map_y * TILE_SIZE) + (TILE_SIZE / 2);
                game->portal_1.is_active = 1;
                game->portal_1.orientation = portal_orientation;
                
                game->portal_count = 1;
                printf("Nouveaux portails, premier à [%d, %d], orientation: %s\n", 
                       map_x, map_y,
                       (portal_orientation == 0) ? "Nord" :
                       (portal_orientation == 1) ? "Est" :
                       (portal_orientation == 2) ? "Sud" : "Ouest");
            }
        }
    }
    else if (game->current_weapon == RAYGUN)
    {
        double player_x = game->player.x;
        double player_y = game->player.y;
        double ray_dir_x = cos(game->player.angle);
        double ray_dir_y = sin(game->player.angle);
        
        int enemy_hit = 0;
        double closest_enemy_distance = center_ray->distance;
        
        for (int i = 0; i < game->num_enemies; i++)
        {
            t_enemy *enemy = &game->enemies[i];
            if (!enemy->active || enemy->state == DEAD)
                continue;
                
            // ← CORRECTION ICI : Ennemis déjà en pixels !
            double enemy_px = enemy->x;  // ← ENLEVER * TILE_SIZE
            double enemy_py = enemy->y;  // ← ENLEVER * TILE_SIZE
            
            double dx_to_enemy = enemy_px - player_x;
            double dy_to_enemy = enemy_py - player_y;
            double distance_to_enemy = sqrt(dx_to_enemy * dx_to_enemy + dy_to_enemy * dy_to_enemy);
            
            double dot_product = (dx_to_enemy * ray_dir_x + dy_to_enemy * ray_dir_y) / distance_to_enemy;
            
            if (dot_product > 0.9 && distance_to_enemy < closest_enemy_distance)
            {
                double cross_product = fabs(dx_to_enemy * ray_dir_y - dy_to_enemy * ray_dir_x);
                double distance_from_line = cross_product / sqrt(ray_dir_x * ray_dir_x + ray_dir_y * ray_dir_y);
                
                if (distance_from_line < TILE_SIZE * 0.4)
                {
                    // ← CORRECTION ICI AUSSI : Convertir pixels → cellules
                    map_x = (int)(enemy->x / TILE_SIZE);  // ← Diviser par TILE_SIZE
                    map_y = (int)(enemy->y / TILE_SIZE);  // ← Diviser par TILE_SIZE
                    
                    printf("Enemy hit at [%d, %d]\n", map_x, map_y);
                    
                    if (damage_enemy_at_position(game, map_x, map_y, 100))
                    {
                        printf("Enemy flatlined at [%d, %d]\n", map_x, map_y);
                    }
                    enemy_hit = 1;
                    closest_enemy_distance = distance_to_enemy;
                    break;
                }
            }
        }
        
        // Se non abbiamo colpito nessun nemico, gestisci muri/porte normalmente
        if (!enemy_hit)
        {
            impact_x = center_ray->wall_hit_x;
            impact_y = center_ray->wall_hit_y;
            map_x = (int)(impact_x / TILE_SIZE);
            map_y = (int)(impact_y / TILE_SIZE);
            
            if (center_ray->hit_type == '1')
            {
                game->map.matrix[map_y][map_x] = 'i';
            }
            else if (center_ray->hit_type == 'D')
            {
                game->map.matrix[map_y][map_x] = 'd';
            }
        }
    }
}

int mouse_button(int button, int x, int y, t_game *game)
{
    (void)x;
    (void)y;
    if (game->map.north.ptr != NULL)
        return 0;
    if (button == 1)
    {
        if (game->current_weapon == RAYGUN)
        {
            if (!game->player.weapon.is_firing)
            {
                game->player.weapon.is_firing = 1;
                game->player.weapon.current_state = WEAPON_PREFIRE;
                game->player.weapon.frame = 1;
                game->player.weapon.frame_delay = 10;
                calculate_shoot(game);
            }
        }
        else if (game->current_weapon == PORTALGUN)
        {
            if (game->portal_count < 2)
            {
                calculate_shoot(game);
            }
        }
    }
    else if (button == 4 || button == 5)
    {
        if (game->current_weapon == RAYGUN)
            game->current_weapon = PORTALGUN;
        else if (game->current_weapon == PORTALGUN)
            game->current_weapon = RAYGUN;
    }
    else if (button == 3 && game->current_weapon == PORTALGUN)
    {
        remove_all_portals(game);
    }

    return (0);
}

// int damage_enemy_at_position(t_game *game, int tile_x, int tile_y, int damage)
// {
//     int i = 0;
    
//     while (i < game->num_enemies)
//     {
//         t_enemy *enemy = &game->enemies[i];
        
//         // AVANT : comparaison directe car enemy était en cellules
//         // int enemy_tile_x = (int)(enemy->x);
//         // int enemy_tile_y = (int)(enemy->y);
        
//         // APRÈS : convertir pixels → cellules
//         int enemy_tile_x = (int)(enemy->x / TILE_SIZE);
//         int enemy_tile_y = (int)(enemy->y / TILE_SIZE);

//         if (enemy_tile_x == tile_x && enemy_tile_y == tile_y && enemy->active)
//         {
//             enemy->health -= damage;
//             if (enemy->health <= 0)
//             {
//                 //enemy->active = 0;
//                 enemy->death_timer = 50;
//                 enemy->animation.current_frame = 0;
//                 enemy->animation.frame_counter = 0;
//                 enemy->state = DEAD;
//                 return (1); // Ennemi mort
//             }
//             else
//                 return (0);
//         }
//         i++;
//     }
//     return (0);
// }