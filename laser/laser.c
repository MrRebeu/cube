#include "cube3d.h"

int count_lasers_in_map(t_game *game)
{
    int count = 0;
    int y = 0;
    int x;
    
    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            if (game->map.matrix[y][x] == 'L')
                count++;
            x++;
        }
        y++;
    }
    return count;
}


int init_lasers(t_game *game)
{
    int y = 0;
    int x;
    int laser_index = 0;
    
    game->num_lasers = count_lasers_in_map(game);
    
    if (game->num_lasers == 0)
    {
        game->lasers = NULL;
        return (1);
    }
    
    game->lasers = malloc(sizeof(t_laser) * game->num_lasers);
    if (!game->lasers)
        return (0);
    
    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            if (game->map.matrix[y][x] == 'L' && laser_index < game->num_lasers)
            {
                game->lasers[laser_index].x = (x * TILE_SIZE) + (TILE_SIZE / 2);
                game->lasers[laser_index].y = (y * TILE_SIZE) + (TILE_SIZE / 2);
                game->lasers[laser_index].active = 1;
                laser_index++;
            }
            x++;
        }
        y++;
    }
    
    return (1);
}


void render_laser_sprite(t_game *game, t_laser *laser)
{
    // ✅ Même logique que les ennemis
    double dx = laser->x - game->player.x;
    double dy = laser->y - game->player.y;
    double distance = sqrt(dx * dx + dy * dy);
    
    if (distance < 5.0 || distance > 800.0)
        return;
    
    // ✅ Transformation 3D
    double inv_det = 1.0f / (game->player.plane_x * game->player.dir_y - 
                              game->player.dir_x * game->player.plane_y);
    double transform_x = inv_det * (game->player.dir_y * dx - game->player.dir_x * dy);
    double transform_y = inv_det * (-game->player.plane_y * dx + game->player.plane_x * dy);
    
    if (transform_y <= 0.1f)
        return;
    
    // ✅ Position à l'écran
    int screen_x = (int)((DISPLAY_WIDTH / 2) * (1 + transform_x / transform_y));
    
    if (screen_x < -50 || screen_x > DISPLAY_WIDTH + 50)
        return;
    
    // ✅ Taille basée sur la distance
    double distance_to_projection = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    double laser_height = TILE_SIZE * 1.2;
    int sprite_height = (int)((laser_height / transform_y) * distance_to_projection);
    
    if (sprite_height < 10) sprite_height = 10;
    if (sprite_height > DISPLAY_HEIGHT) sprite_height = DISPLAY_HEIGHT;
    
    // ✅ Position verticale
    int laser_top = (DISPLAY_HEIGHT / 2) - (sprite_height / 2) + game->pitch;
    int laser_bottom = laser_top + sprite_height;
    
    // ✅ Largeur (très fine)
    int laser_width = 3;
    int start_col = screen_x - laser_width / 2;
    int end_col = screen_x + laser_width / 2;
    
    if (start_col < 0) start_col = 0;
    if (end_col >= DISPLAY_WIDTH) end_col = DISPLAY_WIDTH - 1;
    
    // ✅ Dessiner les 4 traits rouges
    draw_laser_lines(game, start_col, end_col, laser_top, laser_bottom, transform_y);
}

void draw_laser_lines(t_game *game, int start_col, int end_col, int top, int bottom, double depth)
{
    unsigned int laser_color = 0xFF0000;
    int height = bottom - top;
    int trait_thickness = 2;
    
    for (int col = start_col; col <= end_col; col++)
    {
        if (col < 0 || col >= DISPLAY_WIDTH)
            continue;
            
        // ✅ Vérifier depth buffer (comme les ennemis)
        if (depth >= game->depth_buffer[col])
            continue;
        
        // ✅ 4 traits verticaux
        int spacing = height / 5;
        
        // Trait 1
        for (int y = top; y < top + trait_thickness && y <= bottom; y++)
        {
            if (y >= 0 && y < DISPLAY_HEIGHT)
            {
                char *dst = game->screen.addr + y * game->screen.line_length + 
                           col * (game->screen.bits_per_pixel / 8);
                *(unsigned int *)dst = laser_color;
            }
        }
        
        // Trait 2
        for (int y = top + spacing; y < top + spacing + trait_thickness && y <= bottom; y++)
        {
            if (y >= 0 && y < DISPLAY_HEIGHT)
            {
                char *dst = game->screen.addr + y * game->screen.line_length + 
                           col * (game->screen.bits_per_pixel / 8);
                *(unsigned int *)dst = laser_color;
            }
        }
        
        // Trait 3
        for (int y = top + spacing * 2; y < top + spacing * 2 + trait_thickness && y <= bottom; y++)
        {
            if (y >= 0 && y < DISPLAY_HEIGHT)
            {
                char *dst = game->screen.addr + y * game->screen.line_length + 
                           col * (game->screen.bits_per_pixel / 8);
                *(unsigned int *)dst = laser_color;
            }
        }
        
        // Trait 4
        for (int y = bottom - trait_thickness; y <= bottom; y++)
        {
            if (y >= 0 && y < DISPLAY_HEIGHT)
            {
                char *dst = game->screen.addr + y * game->screen.line_length + 
                           col * (game->screen.bits_per_pixel / 8);
                *(unsigned int *)dst = laser_color;
            }
        }
    }
}

void render_all_lasers(t_game *game)
{
    int i = 0;
    
    while (i < game->num_lasers)
    {
        if (game->lasers[i].active)
            render_laser_sprite(game, &game->lasers[i]);
        i++;
    }
}