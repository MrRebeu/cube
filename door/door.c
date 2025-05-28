#include "cube3d.h"

void open_door(t_game *game)
{
    double player_x = game->player.x;
    double player_y = game->player.y;
    double player_angle = game->player.angle;
    double check_distance = TILE_SIZE * 1.5; // 1.5 cellules devant
    double check_x = player_x + cos(player_angle) * check_distance;
    double check_y = player_y + sin(player_angle) * check_distance;
    
    int map_x = (int)(check_x / TILE_SIZE);
    int map_y = (int)(check_y / TILE_SIZE);
    
    // ✅ Vérifier les limites
    if (map_x < 0 || map_x >= game->map.width || 
        map_y < 0 || map_y >= game->map.height)
        return;
    
    char cell = game->map.matrix[map_y][map_x];
    
    if (cell == 'D') // ✅ Porte fermée
    {
        printf("🚪 Ouverture de la porte à [%d,%d]\n", map_x, map_y);
        game->map.matrix[map_y][map_x] = 'O';
        // create_open_door_at_position(game, map_x, map_y);
    }
    else if (cell == 'O') // ✅ Porte ouverte -> la fermer
    {
        printf("🚪 Fermeture de la porte à [%d,%d]\n", map_x, map_y);
        game->map.matrix[map_y][map_x] = 'D';
        // remove_open_door_at_position(game, map_x, map_y);
    }
}