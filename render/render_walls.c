/* ************************************************************************** */
/*                                                                            */
/*   render_walls.c - Rendu des murs et portes                              */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"


// void render_open_door(t_game *game, t_open_door *door)
// {
//     t_render renderer;
//     int is_visible;
    
//     if (!door->active)
//         return;
        
//     // ✅ Utiliser les mêmes fonctions que les ennemis
//     calculate_enemy_transform(game, (t_enemy*)door, &renderer);
    
//     if (renderer.floor_y <= 0.2f)
//         return;
        
//     calculate_enemy_screen_pos(game, &renderer);
    
//     if (renderer.x < 0 || renderer.x >= DISPLAY_WIDTH)
//         return;
        
//     is_visible = check_enemy_occlusion(game, &renderer);
//     if (!is_visible)
//         return;
        
//     setup_enemy_render_params(game, &renderer);
    
//     // ✅ Faire la porte plus haute que les ennemis
//     renderer.sprite_size = (int)(renderer.sprite_size * 1.5);
//     renderer.draw_end -= 50; // Ajuster la position verticale
    
//     draw_open_door_sprite(game, &door->sprite, 
//                          (t_point){renderer.draw_start, renderer.draw_end}, 
//                          renderer.sprite_size);
// }

void render_open_door_as_sprite(t_game *game, t_open_door *door)
{
    if (!door->active)
        return;
    
    // ✅ CALCUL DE LA POSITION FIXE DE LA PORTE
    double door_world_x = door->x;
    double door_world_y = door->y;
    
    // ✅ Vecteur du joueur vers la porte
    double dx = door_world_x - game->player.x;
    double dy = door_world_y - game->player.y;
    
    // ✅ Distance à la porte
    double distance = sqrt(dx * dx + dy * dy);
    if (distance < 10.0 || distance > 1000.0)
        return;
    
    // ✅ Angle de la porte par rapport au joueur
    double door_angle = atan2(dy, dx);
    door_angle = normalize_angle(door_angle);
    
    // ✅ Différence d'angle avec la direction du joueur
    double angle_diff = normalize_angle(door_angle - game->player.angle);
    
    // ✅ Vérifier si la porte est dans le champ de vision
    double half_fov = game->player.fov / 2.0;
    if (fabs(angle_diff) > half_fov)
        return;
    
    // ✅ Convertir l'angle en colonne d'écran FIXE
    double screen_ratio = angle_diff / half_fov;
    int door_screen_column = (DISPLAY_WIDTH / 2) + (int)(screen_ratio * (DISPLAY_WIDTH / 2));
    
    // ✅ Clamp à l'écran
    if (door_screen_column < 0 || door_screen_column >= DISPLAY_WIDTH)
        return;
    
    // ✅ Vérifier l'occlusion avec le depth buffer
    if (distance >= game->depth_buffer[door_screen_column])
        return;
    
    // ✅ Calculer la taille basée sur la distance
    double distance_to_projection = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    double door_height = TILE_SIZE * 1.5;
    int sprite_height = (int)((door_height / distance) * distance_to_projection);
    
    if (sprite_height < 1) sprite_height = 1;
    if (sprite_height > DISPLAY_HEIGHT * 2) sprite_height = DISPLAY_HEIGHT * 2;
    
    // ✅ Position verticale
    int door_top = (DISPLAY_HEIGHT / 2) - (sprite_height / 2) + game->pitch;
    int door_bottom = door_top + sprite_height;
    
    // ✅ Largeur de la porte (quelques pixels seulement pour être fine)
    int door_width = fmax(1, sprite_height / 8); // Ultra-fine !
    
    // ✅ Dessiner la porte FIXE sur quelques colonnes
    render_fixed_door_columns(game, door, door_screen_column, door_width, 
                             door_top, door_bottom, distance);
}

void render_fixed_door_columns(t_game *game, t_open_door *door, int center_column, 
                              int door_width, int door_top, int door_bottom, double distance)
{
    (void)distance;
    
    if (!door->sprite.ptr)
        return;
    
    // ✅ Calculer les colonnes de début et fin
    int start_col = center_column - door_width / 2;
    int end_col = center_column + door_width / 2;
    
    // ✅ Clamp aux limites d'écran
    if (start_col < 0) start_col = 0;
    if (end_col >= DISPLAY_WIDTH) end_col = DISPLAY_WIDTH - 1;
    
    int sprite_height = door_bottom - door_top;
    if (sprite_height <= 0) return;
    
    // ✅ Pour chaque colonne de la porte
    for (int col = start_col; col <= end_col; col++)
    {
        if (col < 0 || col >= DISPLAY_WIDTH)
            continue;
        
        // ✅ Position X dans la texture (centrée)
        int tex_x = (door->sprite.width / 2); // Centre de la texture
        if (door_width > 1)
        {
            // Si la porte fait plusieurs pixels, étaler sur la texture
            tex_x = ((col - start_col) * door->sprite.width) / door_width;
        }
        if (tex_x >= door->sprite.width) tex_x = door->sprite.width - 1;
        
        // ✅ Pour chaque pixel vertical
        for (int y = door_top; y <= door_bottom; y++)
        {
            if (y < 0 || y >= DISPLAY_HEIGHT)
                continue;
            
            // ✅ Position Y dans la texture
            int tex_y = ((y - door_top) * door->sprite.height) / sprite_height;
            if (tex_y >= door->sprite.height) tex_y = door->sprite.height - 1;
            
            // ✅ Récupérer le pixel de texture
            char *src = door->sprite.addr + tex_y * door->sprite.line_length + 
                       tex_x * (door->sprite.bits_per_pixel / 8);
            unsigned int color = *(unsigned int *)src;
            
            int red = (color >> 16) & 0xFF;
            int green = (color >> 8) & 0xFF;
            int blue = color & 0xFF;
            
            // ✅ Transparence pour ne garder que les montants
            int skip_pixel = 0;
            
            if (red < 15 && green < 15 && blue < 15) // Noir
                skip_pixel = 1;
            if (abs(red - 255) <= 3 && abs(green - 0) <= 3 && abs(blue - 0) <= 3) // Rouge pur
                skip_pixel = 1;
            if (red > 240 && green > 240 && blue > 240) // Blanc
                skip_pixel = 1;
            
            // ✅ Dessiner le pixel FIXE
            if (!skip_pixel)
            {
                char *dst = game->screen.addr + y * game->screen.line_length + 
                           col * (game->screen.bits_per_pixel / 8);
                *(unsigned int *)dst = color;
            }
        }
    }
}

void draw_ultra_thin_door_sprite(t_game *game, t_img *sprite, t_point pos, int size)
{
    int i = 0;
    int j;
    int src_x, src_y, x, y;
    char *src, *dst;
    unsigned int color;
    int red, green, blue;
    
    if (!sprite || !sprite->addr || size <= 0)
        return;
    
    while (i < size)
    {
        j = 0;
        while (j < size)
        {
            // ✅ Échantillonnage de la texture
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
            
            // ✅ TRANSPARENCE ULTRA-PRÉCISE pour porte fine
            int skip_pixel = 0;
            
            // Skip pixels noirs (fond transparent)
            if (red < 15 && green < 15 && blue < 15)
                skip_pixel = 1;
            
            // Skip pixels rouge pur (marqueur de transparence)
            if (abs(red - 255) <= 3 && abs(green - 0) <= 3 && abs(blue - 0) <= 3)
                skip_pixel = 1;
            
            // Skip pixels blancs très clairs (souvent transparents)
            if (red > 240 && green > 240 && blue > 240)
                skip_pixel = 1;
            
            // ✅ NE DESSINER QUE LES PIXELS OPAQUES (montants de porte)
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

void render_open_door(t_game *game, t_open_door *door)
{
    double dx, dy, distance;
    int door_screen_start, door_screen_end;
    int door_height, door_top, door_bottom;
    double distance_to_projection;
    
    if (!door->active)
        return;
    
    // ✅ Distance du joueur à la porte
    dx = door->x - game->player.x;
    dy = door->y - game->player.y;
    distance = sqrt(dx * dx + dy * dy);
    
    if (distance < 10.0)
        return;
    
    // ✅ Calculer les positions FIXES des extrémités de la porte
    double door_start_x, door_start_y, door_end_x, door_end_y;
    
    if (door->orientation == 0) // Horizontale
    {
        door_start_x = door->start_x;
        door_start_y = door->y;
        door_end_x = door->start_x + door->width;
        door_end_y = door->y;
    }
    else // Verticale
    {
        door_start_x = door->x;
        door_start_y = door->start_y;
        door_end_x = door->x;
        door_end_y = door->start_y + door->width;
    }
    
    // ✅ Convertir les positions 3D fixes en colonnes d'écran
    door_screen_start = world_to_screen_column(game, door_start_x, door_start_y);
    door_screen_end = world_to_screen_column(game, door_end_x, door_end_y);
    
    // ✅ S'assurer que start < end
    if (door_screen_start > door_screen_end)
    {
        int temp = door_screen_start;
        door_screen_start = door_screen_end;
        door_screen_end = temp;
    }
    
    // ✅ Vérifier si visible à l'écran
    if (door_screen_end < 0 || door_screen_start >= DISPLAY_WIDTH)
        return;
    
    // ✅ Clamp aux limites d'écran
    if (door_screen_start < 0) door_screen_start = 0;
    if (door_screen_end >= DISPLAY_WIDTH) door_screen_end = DISPLAY_WIDTH - 1;
    
    // ✅ Hauteur basée sur la distance
    distance_to_projection = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    door_height = (int)((TILE_SIZE * 1.2) / distance * distance_to_projection);
    
    door_top = (DISPLAY_HEIGHT / 2) - (door_height / 2) + game->pitch;
    door_bottom = (DISPLAY_HEIGHT / 2) + (door_height / 2) + game->pitch;
    
    // ✅ Vérifier occlusion
    int mid_column = (door_screen_start + door_screen_end) / 2;
    if (mid_column >= 0 && mid_column < DISPLAY_WIDTH && 
        distance >= game->depth_buffer[mid_column])
        return;
    
    // ✅ Rendre avec positions fixes
    render_door_columns(game, door, door_screen_start, door_screen_end, 
                       door_top, door_bottom, distance);
}

int world_to_screen_column(t_game *game, double world_x, double world_y)
{
    // ✅ Vecteur du joueur vers le point
    double dx = world_x - game->player.x;
    double dy = world_y - game->player.y;
    
    // ✅ Angle du point par rapport au joueur
    double point_angle = atan2(dy, dx);
    point_angle = normalize_angle(point_angle);
    
    // ✅ Différence avec l'angle de vue du joueur
    double angle_diff = normalize_angle(point_angle - game->player.angle);
    
    // ✅ Convertir en colonne d'écran
    double screen_ratio = angle_diff / (game->player.fov / 2.0);
    int screen_column = DISPLAY_WIDTH / 2 + (int)(screen_ratio * (DISPLAY_WIDTH / 2));
    
    return screen_column;
}

void render_door_columns(t_game *game, t_open_door *door, int col_start, int col_end, 
                        int door_top, int door_bottom, double distance)
{
    (void)distance;
    int col, y;
    int tex_x, tex_y;
    char *src, *dst;
    unsigned int color;
    int red, green, blue;
    int door_width_screen = col_end - col_start;
    
    if (door_width_screen <= 0 || !door->sprite.ptr)
        return;
    
    // ✅ Calculer le facteur d'étirement de la texture
    double texture_scale = (double)door->sprite.width / door_width_screen;
    
    col = col_start;
    while (col <= col_end)
    {
        if (col >= 0 && col < DISPLAY_WIDTH)
        {
            // ✅ Étirer la texture sur toute la largeur
            double tex_x_float = (col - col_start) * texture_scale;
            tex_x = (int)tex_x_float;
            if (tex_x >= door->sprite.width) tex_x = door->sprite.width - 1;
            
            y = door_top;
            while (y <= door_bottom)
            {
                if (y >= 0 && y < DISPLAY_HEIGHT)
                {
                    tex_y = ((y - door_top) * door->sprite.height) / (door_bottom - door_top + 1);
                    if (tex_y >= door->sprite.height) tex_y = door->sprite.height - 1;
                    
                    src = door->sprite.addr + tex_y * door->sprite.line_length + 
                          tex_x * (door->sprite.bits_per_pixel / 8);
                    color = *(unsigned int *)src;
                    
                    red = (color >> 16) & 0xFF;
                    green = (color >> 8) & 0xFF;
                    blue = color & 0xFF;
                    
                    // ✅ Skip transparence mais garder les montants de porte
                    if (!(red < 10 && green < 10 && blue < 10))
                    {
                        dst = game->screen.addr + y * game->screen.line_length + 
                              col * (game->screen.bits_per_pixel / 8);
                        *(unsigned int *)dst = color;
                    }
                }
                y++;
            }
        }
        col++;
    }
}

void draw_open_door_sprite(t_game *game, t_img *sprite, t_point pos, int size)
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

            // ✅ Transparence (pixels noirs ou couleur spécifique)
            skip_pixel = 0;
            
            // Skip pixels noirs
            if (red < 10 && green < 10 && blue < 10)
                skip_pixel = 1;
            
            // Skip pixels rouge pur (comme les ennemis)
            if (abs(red - 255) <= 2 && abs(green - 0) <= 2 && abs(blue - 0) <= 2)
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

void render_all_open_doors(t_game *game)
{
    int i = 0;
    
    while (i < game->num_open_doors)
    {
        if (game->open_doors[i].active)
            render_open_door_as_sprite(game, &game->open_doors[i]); // ← Nouvelle fonction
        i++;
    }
}

t_img* get_wall_texture(t_game *game, t_ray *ray)
{
    if (game->map.north.ptr != NULL)
    {
        if (ray->hit_vertical)
        {
            if (cos(ray->radiant_angle) > 0)
                return &game->map.east;
            else
                return &game->map.west;
        }
        else
        {
            if (sin(ray->radiant_angle) > 0)
                return &game->map.south;
            else
                return &game->map.north;
        }
    }
    else
    {
        return &game->map.wall_texture;
    }
}

void render_wall(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    int CY = (DISPLAY_HEIGHT / 2) + game->pitch;
    double H = renderer->wall_height;
    int texture_y;
    t_img *current_texture = get_wall_texture(game, ray);

    if (ray->hit_vertical)
    {
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
        if (cos(ray->radiant_angle) > 0)
            renderer->tex_x = TILE_SIZE - renderer->tex_x - 1;
    }
    else
    {
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;
        if (sin(ray->radiant_angle) < 0)
            renderer->tex_x = TILE_SIZE - renderer->tex_x - 1;
    }

    renderer->y = renderer->draw_start;
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            float rel = ((renderer->y - CY) / H) + 0.5f;
            texture_y = (int)(rel * TILE_SIZE);
            if (texture_y < 0) texture_y = 0;
            else if (texture_y >= TILE_SIZE) texture_y = TILE_SIZE - 1;

            renderer->tex_addr = current_texture->addr
                + (texture_y * current_texture->line_length
                + renderer->tex_x * (current_texture->bits_per_pixel / 8));
            renderer->color = *(unsigned int *)renderer->tex_addr;

            renderer->screen_pixel = game->screen.addr
                + (renderer->y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)renderer->screen_pixel = renderer->color;
        }
        renderer->y++;
    }
}

void render_door(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    // ✅ Vérifier si on doit rendre la porte à cette position
    double hit_pos;
    int pixel_in_cell;
    int door_thickness = 64; // Épaisseur de la porte (ajustable)
    int cell_center = TILE_SIZE / 8; // Centre de la cellule (32 si TILE_SIZE=64)
    
    // Déterminer quelle coordonnée utiliser selon l'orientation de l'intersection
    if (ray->hit_vertical) {
        // Intersection verticale → porte horizontale → vérifier position Y
        hit_pos = ray->wall_hit_y;
    } else {
        // Intersection horizontale → porte verticale → vérifier position X
        hit_pos = ray->wall_hit_x;
    }
    
    pixel_in_cell = (int)hit_pos % TILE_SIZE;
    
    // ✅ Ne rendre que si on est au centre de la cellule
    if (pixel_in_cell < (cell_center - door_thickness/2) || 
        pixel_in_cell > (cell_center + door_thickness/2)) {
        return; // Pas de rendu = transparent
    }

    // ✅ Rendu normal de la porte
    int CY = (DISPLAY_HEIGHT / 2) + game->pitch;
    double H = renderer->wall_height;
    int texture_y;

    if (ray->hit_vertical)
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;

    renderer->y = renderer->draw_start;
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            float rel = ((renderer->y - CY) / H) + 0.5f;
            texture_y = (int)(rel * TILE_SIZE);
            if (texture_y < 0) texture_y = 0;
            else if (texture_y >= TILE_SIZE) texture_y = TILE_SIZE - 1;

            renderer->tex_addr = game->map.door_texture.addr
                + (texture_y * game->map.door_texture.line_length
                + renderer->tex_x * (game->map.door_texture.bits_per_pixel / 8));
            renderer->color = *(unsigned int *)renderer->tex_addr;

            renderer->screen_pixel = game->screen.addr
                + (renderer->y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)renderer->screen_pixel = renderer->color;
        }
        renderer->y++;
    }
}

void render_wall_portal(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    int CY = (DISPLAY_HEIGHT / 2) + game->pitch;
    double H = renderer->wall_height;
    int texture_y;

    if (ray->hit_vertical)
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;

    renderer->y = renderer->draw_start;
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            float rel = ((renderer->y - CY) / H) + 0.5f;
            texture_y = (int)(rel * TILE_SIZE);
            if (texture_y < 0) texture_y = 0;
            else if (texture_y >= TILE_SIZE) texture_y = TILE_SIZE - 1;

            renderer->tex_addr = game->map.wall_portal_texture.addr
                + (texture_y * game->map.wall_portal_texture.line_length
                + renderer->tex_x * (game->map.wall_portal_texture.bits_per_pixel / 8));
            renderer->color = *(unsigned int *)renderer->tex_addr;

            renderer->screen_pixel = game->screen.addr
                + (renderer->y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)renderer->screen_pixel = renderer->color;
        }
        renderer->y++;
    }
}

void render_wall_shooted(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    int CY = (DISPLAY_HEIGHT / 2) + game->pitch;
    double H = renderer->wall_height;
    int texture_y;

    if (ray->hit_vertical)
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;

    renderer->y = renderer->draw_start;
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            float rel = ((renderer->y - CY) / H) + 0.5f;
            texture_y = (int)(rel * TILE_SIZE);
            if (texture_y < 0) texture_y = 0;
            else if (texture_y >= TILE_SIZE) texture_y = TILE_SIZE - 1;

            renderer->tex_addr = game->map.wall_shooted_texture.addr
                + (texture_y * game->map.wall_shooted_texture.line_length
                + renderer->tex_x * (game->map.wall_shooted_texture.bits_per_pixel / 8));
            renderer->color = *(unsigned int *)renderer->tex_addr;

            renderer->screen_pixel = game->screen.addr
                + (renderer->y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)renderer->screen_pixel = renderer->color;
        }
        renderer->y++;
    }
}