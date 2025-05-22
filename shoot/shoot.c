#include "../cube3d.h"


void calculate_shoot(t_game *game)
{
    double impact_x;
    double impact_y;
    int map_x;
    int map_y;
    
    int center_ray_index = DISPLAY_WIDTH / 2;
    t_ray *center_ray = &game->rays[center_ray_index];
    
    if (game->current_weapon == PORTALGUN)
    {
        if (center_ray->hit_type == '1')
        {
            impact_x = center_ray->wall_hit_x;
            impact_y = center_ray->wall_hit_y;
            map_x = (int)(impact_x / TILE_SIZE);
            map_y = (int)(impact_y / TILE_SIZE);
            
            if (game->portal_count < 2)
            {
                game->map.matrix[map_y][map_x] = 'P';
                
                // Activer le portail directement ici
                if (game->portal_count == 0)
                {
                    game->portal_1.x = map_x + 0.5;  // Centre de la cellule
                    game->portal_1.y = map_y + 0.5;
                    game->portal_1.is_active = 1;    // IMPORTANT: Activer le portail
                    printf("Portal 1 ACTIVATED at position (%f, %f)\n", game->portal_1.x, game->portal_1.y);
                }
                else // game->portal_count == 1
                {
                    game->portal_2.x = map_x + 0.5;
                    game->portal_2.y = map_y + 0.5;
                    game->portal_2.is_active = 1;    // IMPORTANT: Activer le portail
                    printf("Portal 2 ACTIVATED at position (%f, %f)\n", game->portal_2.x, game->portal_2.y);
                }
                
                game->portal_count++;
                printf("DEBUG: Portal created at map position (%d, %d), portal_count=%d\n", 
                       map_x, map_y, game->portal_count);
                printf("Portal states: portal1.is_active=%d, portal2.is_active=%d\n",
                       game->portal_1.is_active, game->portal_2.is_active);
            }
            else
            {
                // Votre code existant pour la gestion des portails existants
                remove_all_portals(game);
                game->map.matrix[map_y][map_x] = 'P';
                
                // Réinitialiser les portails et activer le premier
                game->portal_1.x = map_x + 0.5;
                game->portal_1.y = map_y + 0.5;
                game->portal_1.is_active = 1;    // IMPORTANT: Activer le nouveau portail
                game->portal_2.is_active = 0;    // Désactiver le second portail
                game->portal_count = 1;
                
                printf("All portals removed and new Portal 1 ACTIVATED at position (%f, %f)\n",
                       game->portal_1.x, game->portal_1.y);
            }
        }
    }
    else if (game->current_weapon == RAYGUN)
    {
        if (center_ray->hit_type == '1')
        {
            impact_x = center_ray->wall_hit_x;
            impact_y = center_ray->wall_hit_y;
            map_x = (int)(impact_x / TILE_SIZE);
            map_y = (int)(impact_y / TILE_SIZE);
            game->map.matrix[map_y][map_x] = 'i';
        }
        else if (center_ray->hit_type == 'D')
        {
            impact_x = center_ray->wall_hit_x;
            impact_y = center_ray->wall_hit_y;
            map_x = (int)(impact_x / TILE_SIZE);
            map_y = (int)(impact_y / TILE_SIZE);
            game->map.matrix[map_y][map_x] = 'd';
        }
    }
}

int mouse_button(int button, int x, int y, t_game *game)
{
    (void)x;
    (void)y;
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
