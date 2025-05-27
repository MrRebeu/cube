#include "../cube3d.h"

void render_weapon_pickup(t_game *game, t_weapon_pickup *weapon)
{
    t_render renderer;
    int is_visible;
    
    if (!weapon->active)
        return;
        
    calculate_weapon_transform(game, weapon, &renderer);
    
    if (renderer.floor_y <= 0.2f)
        return;
        
    calculate_weapon_screen_pos(game, &renderer);
    
    if (renderer.x < 0 || renderer.x >= DISPLAY_WIDTH)
        return;
        
    // ← Utilise les fonctions des ennemis (elles marchent pareil)
    is_visible = check_enemy_occlusion(game, &renderer);
    if (!is_visible)
        return;
        
    setup_enemy_render_params(game, &renderer);
    renderer.draw_end += 400;
    draw_weapon_pickup_sprite(game, &weapon->sprite, 
                             (t_point){renderer.draw_start, renderer.draw_end}, 
                             renderer.sprite_size);
}

void calculate_weapon_transform(t_game *game, t_weapon_pickup *weapon, t_render *render)
{
    double dx, dy, inv_det;
    
    dx = weapon->x - game->player.x;
    dy = weapon->y - game->player.y;
    
    inv_det = 1.0f / (game->player.plane_x * game->player.dir_y - 
                      game->player.dir_x * game->player.plane_y);
    render->floor_x = inv_det * (game->player.dir_y * dx - game->player.dir_x * dy);
    render->floor_y = inv_det * (-game->player.plane_y * dx + game->player.plane_x * dy);
}

void calculate_weapon_screen_pos(t_game *game, t_render *render)
{
    double distance_to_projection_plane;
    double corrected_dist;
    double weapon_height = TILE_SIZE * 0.1;
    
    render->x = (int)((DISPLAY_WIDTH / 2) * (1 + render->floor_x / render->floor_y));
    
    distance_to_projection_plane = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    corrected_dist = fmax(render->floor_y, 0.1);
    
    render->sprite_size = (int)((weapon_height / corrected_dist) * distance_to_projection_plane);
    
    if (render->sprite_size > DISPLAY_HEIGHT * 2)
        render->sprite_size = DISPLAY_HEIGHT * 2;
    if (render->sprite_size < 1)
        render->sprite_size = 1;
}

void draw_weapon_pickup_sprite(t_game *game, t_img *sprite, t_point pos, int size)
{
    int i = 0;
    int j;
    int src_x, src_y, x, y;
    char *src, *dst;
    unsigned int color;
    int red, green, blue;
    int skip_pixel = 0;
    
    if (!sprite || !sprite->addr || size <= 0)
        return;
        
    while (i < size)
    {
        j = 0;
        while (j < size)
        {
            src_x = i * sprite->width / size;
            src_y = j * sprite->height / size;
            
            if (src_x < 0) src_x = 0;
            else if (src_x >= sprite->width) src_x = sprite->width - 1;
            if (src_y < 0) src_y = 0;
            else if (src_y >= sprite->height) src_y = sprite->height - 1;
            
            src = sprite->addr + src_y * sprite->line_length + src_x * (sprite->bits_per_pixel / 8);
            color = *(unsigned int *)src;
            
            red = (color >> 16) & 0xFF;
            green = (color >> 8) & 0xFF;
            blue = color & 0xFF;

            // ✅ LOGIQUE DE TRANSPARENCE COMME LES ENNEMIS
            skip_pixel = 0;
            int tolerance = 2;
            
            // Skip pixels noirs (transparence classique)
            if (red < 10 && green < 10 && blue < 10)
                skip_pixel = 1;
            
            // Skip pixels rouge pur (comme les ennemis)
            if (abs(red - 255) <= tolerance && abs(green - 0) <= tolerance && abs(blue - 0) <= tolerance)
                skip_pixel = 1;
            
            // Skip pixels ffa*** spécifiquement
            if (red >= 250 && green >= 160 && green <= 175) // ffa*** avec tolérance
                skip_pixel = 1;

            if (!skip_pixel)
            {
                x = pos.x + i;
                y = pos.y + j;
                if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT)
                {
                    dst = game->screen.addr + y * game->screen.line_length + x * (game->screen.bits_per_pixel / 8);
                    *(unsigned int *)dst = color;
                }
            }
            j++;
        }
        i++;
    }
}