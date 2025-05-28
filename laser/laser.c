#include "cube3d.h"

int ray_crosses_laser(t_game *game, double radiant_angle)
{
    double ray_x = game->player.x;
    double ray_y = game->player.y;
    double ray_dx = cos(radiant_angle);
    double ray_dy = sin(radiant_angle);
    double step = 5.0;
    
    // ✅ Marcher le long du rayon
    for (double distance = 0; distance < 800.0; distance += step)
    {
        ray_x += ray_dx * step;
        ray_y += ray_dy * step;
        
        int map_x = (int)(ray_x / TILE_SIZE);
        int map_y = (int)(ray_y / TILE_SIZE);
        
        if (map_x >= 0 && map_x < game->map.width && 
            map_y >= 0 && map_y < game->map.height)
        {
            if (game->map.matrix[map_y][map_x] == 'L')
                return (1); // ✅ Ce rayon traverse un laser !
        }
        
        // ✅ Arrêter si on touche un mur solide
        if (map_x >= 0 && map_x < game->map.width && 
            map_y >= 0 && map_y < game->map.height &&
            game->map.matrix[map_y][map_x] == '1')
            break;
    }
    
    return (0);
}

void render_laser_overlays(t_game *game)
{
    int col = 0;
    
    while (col < DISPLAY_WIDTH)
    {
        // ✅ Vérifier si ce rayon traverse un laser
        if (ray_crosses_laser(game, game->rays[col].radiant_angle))
        {
            draw_laser_line_on_column(game, col);
        }
        col++;
    }
}

void draw_laser_line_on_column(t_game *game, int column)
{
    unsigned int laser_color = 0xFF0000; // Rouge
    int center_y = DISPLAY_HEIGHT / 2 + game->pitch;
    
    // ✅ Dessiner 3 lignes horizontales courtes
    int line_positions[3] = {center_y - 20, center_y, center_y + 20};
    int line_width = 30; // Largeur de chaque ligne
    
    for (int line = 0; line < 3; line++)
    {
        int y = line_positions[line];
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            // ✅ Dessiner une ligne horizontale centrée sur cette colonne
            for (int x_offset = -line_width/2; x_offset <= line_width/2; x_offset++)
            {
                int x = column + x_offset;
                if (x >= 0 && x < DISPLAY_WIDTH)
                {
                    char *pixel = game->screen.addr + (y * game->screen.line_length + 
                                 x * (game->screen.bits_per_pixel / 8));
                    *(unsigned int *)pixel = laser_color;
                }
            }
        }
    }
}