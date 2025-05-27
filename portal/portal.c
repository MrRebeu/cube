#include "../cube3d.h"

int get_portal_orientation(t_game *game, int portal_x, int portal_y)
{
    int north_blocked;
    int east_blocked;
    int south_blocked;
    int west_blocked;
    
    // Vérifier quels côtés sont bloqués par des murs
    north_blocked = (portal_y - 1 < 0) || (game->map.matrix[portal_y - 1][portal_x] == '1');
    east_blocked = (portal_x + 1 >= game->map.width) || (game->map.matrix[portal_y][portal_x + 1] == '1');
    south_blocked = (portal_y + 1 >= game->map.height) || (game->map.matrix[portal_y + 1][portal_x] == '1');
    west_blocked = (portal_x - 1 < 0) || (game->map.matrix[portal_y][portal_x - 1] == '1');
    
    // Le portail "regarde" vers le côté libre
    if (!north_blocked) return 0; // Face Nord
    if (!east_blocked)  return 1; // Face Est  
    if (!south_blocked) return 2; // Face Sud
    if (!west_blocked)  return 3; // Face Ouest
    
    return 0; // Par défaut
}

void remove_all_portals(t_game *game)
{
    int y;
    int x;
    int i;
    
    y = 0;
    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            if (game->map.matrix[y][x] == 'P')
                game->map.matrix[y][x] = '1';
            x++;
        }
        y++;
    }
    game->portal_count = 0;
    game->portal_1.is_active = 0;
    game->portal_2.is_active = 0;
    
    i = 0;
    while (i < DISPLAY_WIDTH)
    {
        double ray_offset;
        double radiant_angle;
        
        ray_offset = game->player.fov * ((double)i / DISPLAY_WIDTH - 0.5);
        radiant_angle = normalize_angle(game->player.angle + ray_offset);
        game->rays[i].radiant_angle = radiant_angle;
        game->rays[i].player_angle = game->player.angle;
        game->rays[i].distance = ray_casting(game, radiant_angle, i);
        i++;
    }
}

void check_portal_teleportation(t_game *game)
{
    double player_x;
    double player_y;
    int player_map_x;
    int player_map_y;
    int check_offsets[9][2];
    int i;
    int check_x;
    int check_y;
    double portal_center_x;
    double portal_center_y;
    double distance;
    
    player_x = game->player.x;
    player_y = game->player.y;
    player_map_x = (int)(player_x / TILE_SIZE);
    player_map_y = (int)(player_y / TILE_SIZE);
    
    check_offsets[0][0] = -1; check_offsets[0][1] = -1;
    check_offsets[1][0] = 0;  check_offsets[1][1] = -1;
    check_offsets[2][0] = 1;  check_offsets[2][1] = -1;
    check_offsets[3][0] = -1; check_offsets[3][1] = 0;
    check_offsets[4][0] = 0;  check_offsets[4][1] = 0;
    check_offsets[5][0] = 1;  check_offsets[5][1] = 0;
    check_offsets[6][0] = -1; check_offsets[6][1] = 1;
    check_offsets[7][0] = 0;  check_offsets[7][1] = 1;
    check_offsets[8][0] = 1;  check_offsets[8][1] = 1;
    
    i = 0;
    while (i < 9)
    {
        check_x = player_map_x + check_offsets[i][0];
        check_y = player_map_y + check_offsets[i][1];
        
        if (check_x >= 0 && check_x < game->map.width && 
            check_y >= 0 && check_y < game->map.height &&
            game->map.matrix[check_y][check_x] == 'P')
        {
            portal_center_x = (check_x * TILE_SIZE) + (TILE_SIZE / 2);
            portal_center_y = (check_y * TILE_SIZE) + (TILE_SIZE / 2);
            distance = sqrt((player_x - portal_center_x) * (player_x - portal_center_x) + 
                           (player_y - portal_center_y) * (player_y - portal_center_y));
            
            if (distance < 40.0) // Distance correcte pour déclencher la TP
            {
                teleport_through_portal(game, check_x, check_y);
                return;
            }
        }
        i++;
    }
}

void teleport_through_portal(t_game *game, int portal_x, int portal_y)
{
    t_portal *dest_portal;
    int portal_1_map_x;
    int portal_1_map_y;
    int portal_2_map_x;
    int portal_2_map_y;
    int exit_direction;
    int adj_offsets[4][2];
    int adj_x;
    int adj_y;
    int dest_x;
    int dest_y;
    int i;
    
    if (game->portal_count != 2)
        return;
    
    // CALCULER LES POSITIONS DES PORTAILS EN CELLULES
    portal_1_map_x = (int)(game->portal_1.x / TILE_SIZE);
    portal_1_map_y = (int)(game->portal_1.y / TILE_SIZE);
    portal_2_map_x = (int)(game->portal_2.x / TILE_SIZE);
    portal_2_map_y = (int)(game->portal_2.y / TILE_SIZE);
    
    printf("🔍 DEBUG: Portail utilisé [%d,%d]\n", portal_x, portal_y);
    printf("🔍 DEBUG: Portal_1 à [%d,%d], orientation: %d\n", portal_1_map_x, portal_1_map_y, game->portal_1.orientation);
    printf("🔍 DEBUG: Portal_2 à [%d,%d], orientation: %d\n", portal_2_map_x, portal_2_map_y, game->portal_2.orientation);
    
    // IDENTIFIER LE PORTAIL DE DESTINATION
    if (portal_x == portal_1_map_x && portal_y == portal_1_map_y)
    {
        dest_portal = &game->portal_2;
        printf("✅ Utilise Portal_1 → Destination Portal_2\n");
    }
    else if (portal_x == portal_2_map_x && portal_y == portal_2_map_y)
    {
        dest_portal = &game->portal_1;
        printf("✅ Utilise Portal_2 → Destination Portal_1\n");
    }
    else
    {
        printf("❌ ERREUR: Portail non identifié!\n");
        return;
    }
    
    // CALCULER LA DIRECTION DE SORTIE (opposé du portail destination) 
    exit_direction = (dest_portal->orientation + 2) % 4;
    
    dest_x = (int)(dest_portal->x / TILE_SIZE);
    dest_y = (int)(dest_portal->y / TILE_SIZE);
    
    adj_offsets[0][0] = 0;  adj_offsets[0][1] = -1; // Nord
    adj_offsets[1][0] = 1;  adj_offsets[1][1] = 0;  // Est
    adj_offsets[2][0] = 0;  adj_offsets[2][1] = 1;  // Sud
    adj_offsets[3][0] = -1; adj_offsets[3][1] = 0;  // Ouest
    
    printf("Portail dest orienté: %s → Sortie par: %s\n",
           (dest_portal->orientation == 0) ? "Nord" :
           (dest_portal->orientation == 1) ? "Est" :
           (dest_portal->orientation == 2) ? "Sud" : "Ouest",
           (exit_direction == 0) ? "Nord" :
           (exit_direction == 1) ? "Est" :
           (exit_direction == 2) ? "Sud" : "Ouest");
    
    // PLACER LE JOUEUR
    adj_x = dest_x + adj_offsets[exit_direction][0];
    adj_y = dest_y + adj_offsets[exit_direction][1];
    
    if (adj_x >= 0 && adj_x < game->map.width &&
        adj_y >= 0 && adj_y < game->map.height &&
        game->map.matrix[adj_y][adj_x] == '0')
    {
        game->player.x = (adj_x * TILE_SIZE) + (TILE_SIZE / 2);
        game->player.y = (adj_y * TILE_SIZE) + (TILE_SIZE / 2);
        
        printf("✅ Téléportation réussie !\n");
        return;
    }
    
    // ALTERNATIVES SI BLOQUÉ
    i = 0;
    while (i < 4)
    {
        if (i != exit_direction)
        {
            adj_x = dest_x + adj_offsets[i][0];
            adj_y = dest_y + adj_offsets[i][1];
            
            if (adj_x >= 0 && adj_x < game->map.width &&
                adj_y >= 0 && adj_y < game->map.height &&
                game->map.matrix[adj_y][adj_x] == '0')
            {
                game->player.x = (adj_x * TILE_SIZE) + (TILE_SIZE / 2);
                game->player.y = (adj_y * TILE_SIZE) + (TILE_SIZE / 2);
                
                printf("🔄 Sortie alternative\n");
                return;
            }
        }
        i++;
    }
    
    // DERNIER RECOURS
    game->player.x = dest_portal->x;
    game->player.y = dest_portal->y + 30;
    printf("🆘 Téléportation de secours\n");
}