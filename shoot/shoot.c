#include "../cube3d.h"

// void calculate_shoot(t_game *game)
// {
//     // Obtenir le rayon central
//     int center_index = DISPLAY_WIDTH / 2;
//     t_ray *center_ray = &game->rays[center_index];
    
//     // Récupérer les coordonnées du point d'impact
//     double impact_x = center_ray->wall_hit_x;
//     double impact_y = center_ray->wall_hit_y;
    
//     // Convertir en indices dans la grille de la carte
//     int map_x = (int)(impact_x / TILE_SIZE);
//     int map_y = (int)(impact_y / TILE_SIZE);
    
//     printf("Impact à: %.2f, %.2f → indices carte: (%d, %d)\n", 
//            impact_x, impact_y, map_x, map_y);
    
//     // Vérifier que les indices sont dans les limites de la carte
//     if (map_x >= 0 && map_x < game->map.width && 
//         map_y >= 0 && map_y < game->map.height)
//     {
//         // Valeur actuelle dans la carte
//         char current_value = game->map.matrix[map_y][map_x];
//         printf("Valeur actuelle: '%c'\n", current_value);
        
//         // Modifier la carte directement
//         game->map.matrix[map_y][map_x] = 'P';
        
//         // Vérifier immédiatement si la modification a pris effet
//         char new_value = game->map.matrix[map_y][map_x];
//         printf("Nouvelle valeur: '%c'\n", new_value);
        
//         // Si la modification n'a pas pris effet, afficher un message d'erreur
//         if (new_value != 'P')
//         {
//             printf("ERREUR: La modification n'a pas été appliquée!\n");
//         }
//     }
//     else
//     {
//         printf("ERREUR: Indices hors limites de la carte (%d x %d)\n", 
//                game->map.width, game->map.height);
//     }
// }

// void calculate_shoot(t_game *game)
// {
//     // Obtenir le rayon central
//     int center_index = DISPLAY_WIDTH / 2;
//     t_ray *center_ray = &game->rays[center_index];
    
//     printf("Tir! Type touché: %c, Distance: %f\n", 
//            center_ray->hit_type, center_ray->distance);
    
//     // Si le rayon touche un mur ou une porte
//     if (center_ray->hit_type == '1' || center_ray->hit_type == 'D')
//     {
//         // Convertir en indices de la grille
//         double impact_x = center_ray->wall_hit_x;
//         double impact_y = center_ray->wall_hit_y;
//         int map_x = (int)(impact_x / TILE_SIZE);
//         int map_y = (int)(impact_y / TILE_SIZE);
        
//         printf("Impact à: %.2f, %.2f → indices carte: (%d, %d)\n", 
//                impact_x, impact_y, map_x, map_y);
        
//         // Vérifier que les indices sont valides
//         if (map_x >= 0 && map_x < game->map.width && 
//             map_y >= 0 && map_y < game->map.height)
//         {
//             // Valeur actuelle dans la carte
//             char current_value = game->map.matrix[map_y][map_x];
//             printf("Valeur actuelle: '%c'\n", current_value);
            
//             // Modifier la carte directement
//             game->map.matrix[map_y][map_x] = 'P';
            
//             // Vérifier immédiatement si la modification a pris effet
//             char new_value = game->map.matrix[map_y][map_x];
//             printf("Nouvelle valeur: '%c'\n", new_value);
            
//             // Gérer les portails
//             if (game->portal_count == 0)
//             {
//                 game->portal_1.x = impact_x;
//                 game->portal_1.y = impact_y;
//                 game->portal_1.is_vertical = center_ray->hit_vertical;
//                 game->portal_1.surface_type = current_value;
//                 game->portal_1.is_active = 1;
//                 game->portal_count = 1;
//                 printf("Premier portail créé\n");
//             }
//             else if (game->portal_count == 1)
//             {
//                 game->portal_2.x = impact_x;
//                 game->portal_2.y = impact_y;
//                 game->portal_2.is_vertical = center_ray->hit_vertical;
//                 game->portal_2.surface_type = current_value;
//                 game->portal_2.is_active = 1;
//                 game->portal_count = 2;
//                 printf("Deuxième portail créé\n");
//             }
//             else // game->portal_count == 2
//             {
//                 // Restaurer le premier portail
//                 int old_map_x = (int)(game->portal_1.x / TILE_SIZE);
//                 int old_map_y = (int)(game->portal_1.y / TILE_SIZE);
//                 if (old_map_x >= 0 && old_map_x < game->map.width &&
//                     old_map_y >= 0 && old_map_y < game->map.height)
//                 {
//                     game->map.matrix[old_map_y][old_map_x] = game->portal_1.surface_type;
//                     printf("Premier portail restauré\n");
//                 }
                
//                 // Faire glisser le deuxième portail à la première position
//                 game->portal_1 = game->portal_2;
                
//                 // Créer le nouveau portail
//                 game->portal_2.x = impact_x;
//                 game->portal_2.y = impact_y;
//                 game->portal_2.is_vertical = center_ray->hit_vertical;
//                 game->portal_2.surface_type = current_value;
//                 game->portal_2.is_active = 1;
//                 printf("Portails mis à jour (remplacement du plus ancien)\n");
//             }
            
//             // IMPORTANT: Recalculer tous les rayons après avoir modifié la carte
//             int i = 0;
//             while (i < DISPLAY_WIDTH)
//             {
//                 double ray_offset = game->player.fov * ((double)i / DISPLAY_WIDTH - 0.5);
//                 double radiant_angle = game->player.angle + ray_offset;
                
//                 // Normaliser l'angle
//                 radiant_angle = normalize_angle(radiant_angle);
                
//                 game->rays[i].radiant_angle = radiant_angle;
//                 game->rays[i].player_angle = game->player.angle;
                
//                 // Recalculer la distance avec la carte modifiée
//                 game->rays[i].distance = ray_casting(game, radiant_angle, i);
                
//                 i++;
//             }
            
//             printf("Tous les rayons ont été recalculés après placement du portail\n");
//         }
//         else
//         {
//             printf("ERREUR: Indices hors limites de la carte (%d x %d)\n", 
//                    game->map.width, game->map.height);
//         }
//     }
//     else
//     {
//         printf("Tir sur une surface non valide (ni mur ni porte)\n");
//     }
// }

void calculate_shoot(t_game *game)
{
	double impact_x;
	double impact_y;
	int	map_x;
	int map_y;

    int center_ray_index = DISPLAY_WIDTH / 2;  // Créer cette variable avec ce nom
    t_ray *center_ray = &game->rays[center_ray_index];  // Utiliser cette variable
    
	if (game->current_weapon == PORTALGUN)
	{
		printf("Tir Portal Gun! Type touché: %c, Distance: %f\n", 
               center_ray->hit_type, center_ray->distance);
        if (center_ray->hit_type == '1')
        {
			impact_y = center_ray->wall_hit_y;
			impact_x = center_ray->wall_hit_x;
			map_x = (int)(impact_x / TILE_SIZE);
			map_y = (int)(impact_y / TILE_SIZE);
			game->map.matrix[map_y][map_x] = 'P';
			// printf("calculate_shot: game->map.matrix[%d][%d] = '%c', adresse = %p\n", 
     		//   map_y, map_x, game->map.matrix[map_y][map_x], (void*)&game->map.matrix[map_y][map_x]);

			// // Dans ray_casting, quand tu accèdes à un point qui pourrait être un portail:
			// if (game->map.matrix[map_y][map_x] == 'P' || game->map.matrix[map_y][map_x] == '1') {
			// 	printf("ray_casting: map->matrix[%d][%d] = '%c', adresse = %p\n", 
			// 		map_y, map_x,game->map.matrix[map_y][map_x], (void*)&game->map.matrix[map_y][map_x]);
			// }
			    //         printf("creeate a portal at %.2f, %.2f\n", 
                //    center_ray->wall_hit_x, center_ray->wall_hit_y);
        }
	}
	else if (game->current_weapon == RAYGUN)
	{
			printf("Tir Ray Gun! Type touché: %c, Distance: %f\n", 
               center_ray->hit_type, center_ray->distance);
        
        if (center_ray->hit_type == 'D')
        {
			
            printf("La porte a été endommagée!\n");
        }
        else if (center_ray->hit_type == 'M')
        {
            printf("Ennemi touché!\n");
        }
	}
    printf("Tir! Type touché: %c, Distance: %f\n", 
           center_ray->hit_type, center_ray->distance);
}

int	mouse_button(int button, int x, int y, t_game *game)
{
	(void)x;
	(void)y;
	if (button == 1)
	{
		if (game->player.fire_cooldown <= 0)
		{
			game->player.is_firing = 1;
			game->player.fire_cooldown = 10;
			calculate_shoot(game);
		}
	}
	return (0);
}
