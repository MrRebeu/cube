#include "./cube3d.h"

int is_near_portal(t_game *game, t_portal *portal)
{
    double player_x;
    double player_y;
    double distance;
    double portal_threshold;
    printf("c");
    // Si le portail n'est pas actif, retourner 0
    if (!portal->is_active)
    {
        printf("Portal not active, skipping proximity check\n");
        return (0);
    }
    
    // Convertir la position du joueur en unités de grille
    player_x = game->player.x / TILE_SIZE;
    player_y = game->player.y / TILE_SIZE;
    
    // Distance minimale pour déclencher le portail
    portal_threshold = 0.5;
    
    // Calculer la distance entre le joueur et le portail
    distance = sqrt(pow(player_x - portal->x, 2) + 
                   pow(player_y - portal->y, 2));
    
    printf("Player at (%f, %f), portal at (%f, %f), distance: %f, threshold: %f\n",
           player_x, player_y, portal->x, portal->y, distance, portal_threshold);
    
    // Vérifier si la distance est inférieure au seuil
    return (distance < portal_threshold);
}

void teleport_player(t_game *game, t_portal *to_portal)
{
    printf("Teleporting to portal at (%f, %f)\n", to_portal->x, to_portal->y);
    
    // Sauvegarder l'ancienne position
    double old_x = game->player.x;
    double old_y = game->player.y;
    
    // 1. Calculer la direction opposée au portail (la direction "vers l'extérieur")
    // On suppose que le portail est sur un mur et donc la normale du mur pointe vers l'espace libre
    
    // D'abord, trouver si le portail est sur un mur horizontal ou vertical
    // en vérifiant les cellules adjacentes
    int portal_grid_x = (int)to_portal->x;
    int portal_grid_y = (int)to_portal->y;
    
    double outward_angle = 0.0;
    
    // Vérifier les quatre directions principales autour du portail
    // pour déterminer où est le mur et où est l'espace libre
    if (portal_grid_x > 0 && game->map.matrix[portal_grid_y][portal_grid_x - 1] == '1')
        outward_angle = 0.0;  // Mur à gauche, sortie vers la droite (0 radian)
    else if (portal_grid_x < game->map.width - 1 && game->map.matrix[portal_grid_y][portal_grid_x + 1] == '1')
        outward_angle = M_PI;  // Mur à droite, sortie vers la gauche (PI radians)
    else if (portal_grid_y > 0 && game->map.matrix[portal_grid_y - 1][portal_grid_x] == '1')
        outward_angle = M_PI / 2;  // Mur en haut, sortie vers le bas (PI/2 radians)
    else if (portal_grid_y < game->map.height - 1 && game->map.matrix[portal_grid_y + 1][portal_grid_x] == '1')
        outward_angle = 3 * M_PI / 2;  // Mur en bas, sortie vers le haut (3*PI/2 radians)
    else
        outward_angle = game->player.angle;  // Par défaut, utiliser l'angle du joueur
    
    printf("Determined outward angle: %f radians\n", outward_angle);
    
    // 2. Chercher la distance maximale possible dans cette direction sans rencontrer de mur
    double max_distance = 0.0;
    double step_size = 0.1;  // Pas de 0.1 unité de grille pour la recherche
    double max_search_distance = 3.0;  // Maximum 3 unités de grille (3 * TILE_SIZE pixels)
    
    for (double distance = step_size; distance <= max_search_distance; distance += step_size)
    {
        double test_x = to_portal->x + cos(outward_angle) * distance;
        double test_y = to_portal->y + sin(outward_angle) * distance;
        
        int test_grid_x = (int)test_x;
        int test_grid_y = (int)test_y;
        
        // Vérifier si on sort des limites de la carte
        if (test_grid_x < 0 || test_grid_x >= game->map.width || 
            test_grid_y < 0 || test_grid_y >= game->map.height ||
            game->map.matrix[test_grid_y][test_grid_x] == '1')
        {
            // On a trouvé un mur, utiliser la distance précédente
            max_distance = distance - step_size;
            break;
        }
        
        // Si on arrive ici, la position est valide, continuons à chercher
        max_distance = distance;
    }
    
    // Utiliser au moins 0.7 unités de grille de distance pour éviter d'être trop près du portail
    max_distance = fmax(max_distance, 0.7);
    
    // 3. Placer le joueur à cette distance du portail dans la direction calculée
    game->player.x = (to_portal->x + cos(outward_angle) * max_distance) * TILE_SIZE;
    game->player.y = (to_portal->y + sin(outward_angle) * max_distance) * TILE_SIZE;
    
    // 4. Orienter le joueur dans la même direction que l'angle de sortie
    game->player.angle = outward_angle;
    
    printf("Teleported from (%f, %f) to (%f, %f) with dynamic positioning\n", 
           old_x, old_y, game->player.x, game->player.y);
}

void update_portal_positions(t_game *game)
{
    printf("update_portal_positions called, resetting portals\n");
    
    // Réinitialiser les portails
    game->portal_1.is_active = 0;
    game->portal_2.is_active = 0;
    game->portal_count = 0;
    
    // Parcourir la carte pour trouver les portails
    for (int y = 0; y < game->map.height; y++)
    {
        for (int x = 0; x < game->map.width; x++)
        {
            if (game->map.matrix[y][x] == 'P')
            {
                printf("Found P at position (%d, %d)\n", x, y);
                
                if (game->portal_count == 0)
                {
                    game->portal_1.x = x + 0.5;
                    game->portal_1.y = y + 0.5;
                    game->portal_1.is_active = 1;
                    game->portal_count++;
                    printf("Portal 1 found and ACTIVATED at grid (%d, %d), set to (%f, %f), is_active: %d\n", 
                           x, y, game->portal_1.x, game->portal_1.y, game->portal_1.is_active);
                }
                else if (game->portal_count == 1)
                {
                    game->portal_2.x = x + 0.5;
                    game->portal_2.y = y + 0.5;
                    game->portal_2.is_active = 1;
                    game->portal_count++;
                    printf("Portal 2 found and ACTIVATED at grid (%d, %d), set to (%f, %f), is_active: %d\n", 
                           x, y, game->portal_2.x, game->portal_2.y, game->portal_2.is_active);
                }
            }
        }
    }
    printf("Updated portal positions: found %d portals, portal1 active: %d, portal2 active: %d\n", 
           game->portal_count, game->portal_1.is_active, game->portal_2.is_active);
}

void remove_all_portals(t_game *game)
{
	int y;
	int x;
	int i;
	
	i = 0;
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
	while (i < DISPLAY_WIDTH)
	{
		double ray_offset = game->player.fov * ((double)i / DISPLAY_WIDTH - 0.5);
		double radiant_angle = normalize_angle(game->player.angle + ray_offset);
		game->rays[i].radiant_angle = radiant_angle;
		game->rays[i].player_angle = game->player.angle;
		game->rays[i].distance = ray_casting(game, radiant_angle, i);
		i++;
	}
}

void check_portal_teleport(t_game *game)
{
    static int teleport_cooldown = 0;
    
    // Afficher les positions et calculs une fois toutes les 30 frames pour ne pas spammer
    static int debug_counter = 0;
    debug_counter = (debug_counter + 1) % 30;
    
    if (debug_counter == 0)
    {
        // Position du joueur en pixels et en unités de grille
        double player_x_pixels = game->player.x;
        double player_y_pixels = game->player.y;
        double player_x_grid = player_x_pixels / TILE_SIZE;
        double player_y_grid = player_y_pixels / TILE_SIZE;
        
        // Informations sur les portails
        printf("===== Portal Debug Info =====\n");
        printf("Player position: pixels(%f, %f), grid(%f, %f)\n", 
               player_x_pixels, player_y_pixels, player_x_grid, player_y_grid);
        printf("Portal 1: active=%d, pos=(%f, %f)\n", 
               game->portal_1.is_active, game->portal_1.x, game->portal_1.y);
        printf("Portal 2: active=%d, pos=(%f, %f)\n", 
               game->portal_2.is_active, game->portal_2.x, game->portal_2.y);
        
        // Calculer les distances
        double dist_to_portal1 = sqrt(pow(player_x_grid - game->portal_1.x, 2) + 
                                     pow(player_y_grid - game->portal_1.y, 2));
        double dist_to_portal2 = sqrt(pow(player_x_grid - game->portal_2.x, 2) + 
                                     pow(player_y_grid - game->portal_2.y, 2));
        
        printf("Distance to portal 1: %f grid units\n", dist_to_portal1);
        printf("Distance to portal 2: %f grid units\n", dist_to_portal2);
        printf("Current threshold: 0.5 grid units\n");
        printf("=============================\n");
    }
    
    if (teleport_cooldown > 0)
    {
        teleport_cooldown--;
        return;
    }
    
    // Vérifier si les deux portails sont actifs
    if (!game->portal_1.is_active || !game->portal_2.is_active)
        return;
    
    // Calculer les distances
    double player_x_grid = game->player.x / TILE_SIZE;
    double player_y_grid = game->player.y / TILE_SIZE;
    double dist_to_portal1 = sqrt(pow(player_x_grid - game->portal_1.x, 2) + 
                                 pow(player_y_grid - game->portal_1.y, 2));
    double dist_to_portal2 = sqrt(pow(player_x_grid - game->portal_2.x, 2) + 
                                 pow(player_y_grid - game->portal_2.y, 2));
    
    // Augmenter le seuil de détection
    double threshold = 1.0;  // Augmenté de 0.5 à 1.0
    
    // Vérifier la proximité avec chaque portail
    if (dist_to_portal1 < threshold)
    {
        printf("TELEPORTING from portal 1 to portal 2! Distance: %f\n", dist_to_portal1);
        teleport_player(game, &game->portal_2);
        teleport_cooldown = 160;
    }
    else if (dist_to_portal2 < threshold)
    {
        printf("TELEPORTING from portal 2 to portal 1! Distance: %f\n", dist_to_portal2);
        teleport_player(game, &game->portal_1);
        teleport_cooldown = 160;
    }
}
