
#include "../cube3d.h"


void render_laser(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    (void)ray;
    
    // ✅ Couleur rouge vif
    unsigned int laser_color = 0xFF0000;
    
    // ✅ Utiliser la hauteur normale du mur (comme les autres éléments)
    int laser_start = renderer->draw_start;
    int laser_end = renderer->draw_end;
    int laser_height = laser_end - laser_start;
    
    if (laser_height <= 0)
        return;
    
    // ✅ Dessiner SEULEMENT 4 traits fins (pas de fond)
    int trait_thickness = 2;
    int spacing = laser_height / 5;
    
    // ✅ Trait 1 (haut)
    for (int y = laser_start; y < laser_start + trait_thickness && y <= laser_end; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
    
    // ✅ Trait 2
    int trait2_start = laser_start + spacing;
    for (int y = trait2_start; y < trait2_start + trait_thickness && y <= laser_end; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
    
    // ✅ Trait 3
    int trait3_start = laser_start + spacing * 2;
    for (int y = trait3_start; y < trait3_start + trait_thickness && y <= laser_end; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
    
    // ✅ Trait 4 (bas)
    int trait4_start = laser_end - trait_thickness;
    for (int y = trait4_start; y <= laser_end && y >= laser_start; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
    
    // ✅ IMPORTANT : Ne pas appeler render_floor_and_ceiling() pour cette colonne
    // Le fond reste celui qui était déjà dessiné (sol/plafond)
}

void render_laser_overlays(t_game *game)
{
    int col = 0;
    
    while (col < DISPLAY_WIDTH)
    {
        // ✅ Vérifier si ce rayon traverse une cellule laser
        if (ray_crosses_laser(game, col))
        {
            render_laser_overlay_on_column(game, col);
        }
        col++;
    }
}

int ray_crosses_laser(t_game *game, int column_x)
{
    double ray_x = game->player.x;
    double ray_y = game->player.y;
    double radiant_angle = game->rays[column_x].radiant_angle;
    double ray_dx = cos(radiant_angle);
    double ray_dy = sin(radiant_angle);
    double step = 5.0;
    
    // ✅ Marcher le long du rayon jusqu'à la distance max
    double max_distance = game->rays[column_x].distance;
    
    for (double distance = 0; distance < max_distance; distance += step)
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
    }
    
    return (0);
}

void render_laser_overlay_on_column(t_game *game, int column_x)
{
    // ✅ Calculer la hauteur du laser (plus petit qu'un mur)
    double laser_distance = game->rays[column_x].distance * 0.7; // Plus proche = plus visible
    double distance_to_projection = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    int laser_height = (int)((TILE_SIZE * 0.8) / laser_distance * distance_to_projection);
    
    if (laser_height < 10) laser_height = 10;
    if (laser_height > DISPLAY_HEIGHT) laser_height = DISPLAY_HEIGHT;
    
    // ✅ Position verticale
    int laser_start = (DISPLAY_HEIGHT / 2) - (laser_height / 2) + game->pitch;
    int laser_end = laser_start + laser_height;
    
    // ✅ Clamp
    if (laser_start < 0) laser_start = 0;
    if (laser_end >= DISPLAY_HEIGHT) laser_end = DISPLAY_HEIGHT - 1;
    
    // ✅ Couleur laser avec alpha (semi-transparent)
    unsigned int laser_color = 0xFF0000; // Rouge pur
    
    // ✅ Dessiner 4 traits fins
    int trait_thickness = 1;
    int spacing = laser_height / 5;
    
    // ✅ Trait 1
    for (int y = laser_start; y < laser_start + trait_thickness && y <= laser_end; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
    
    // ✅ Trait 2
    int trait2_y = laser_start + spacing;
    for (int y = trait2_y; y < trait2_y + trait_thickness && y <= laser_end; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
    
    // ✅ Trait 3
    int trait3_y = laser_start + spacing * 2;
    for (int y = trait3_y; y < trait3_y + trait_thickness && y <= laser_end; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
    
    // ✅ Trait 4
    int trait4_y = laser_end - trait_thickness;
    for (int y = trait4_y; y < laser_end && y >= laser_start; y++)
    {
        if (y >= 0 && y < DISPLAY_HEIGHT)
        {
            char *pixel = game->screen.addr + (y * game->screen.line_length + 
                         column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)pixel = laser_color;
        }
    }
}

void render_column(t_game *game, int column_x, t_ray *ray)
{
    t_render renderer;

    renderer.corrected_dist = no_fish_eye(ray->distance, ray->radiant_angle, ray->player_angle);
    renderer.wall_height = calc_wall_height(renderer.corrected_dist);
    renderer.door_height = (int)(renderer.wall_height * 1.3);
    
    renderer.draw_start = (DISPLAY_HEIGHT / 2) - (renderer.wall_height / 2) + game->pitch;
    renderer.draw_end = (DISPLAY_HEIGHT / 2) + (renderer.wall_height / 2) + game->pitch;
    
    renderer.texture_offset_y = 0;
    if (renderer.wall_height > DISPLAY_HEIGHT)
        renderer.texture_offset_y = (renderer.wall_height - DISPLAY_HEIGHT) / 2;
    
    if (renderer.draw_start < 0)
        renderer.draw_start = 0;
    if (renderer.draw_end >= DISPLAY_HEIGHT)
        renderer.draw_end = DISPLAY_HEIGHT - 1;
    
    if (ray->hit_type == 'P')
        render_wall_portal(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'D')
        render_door(game, column_x, &renderer, ray);
    // else if (ray->hit_type == 'V') // a faire
    //     render_door_openable(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'i')
        render_wall_shooted(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'd')
        render_door_shooted(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'o')
        render_shooted_open_door(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'O')
        render_open_door(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'L') // ✅ LASER par-dessus le sol/plafond
        render_laser(game, column_x, &renderer, ray);
    else
        render_wall(game, column_x, &renderer, ray);
    render_floor_and_ceiling(game, column_x, &renderer);
}

void render_scene(t_game *game)
{
    int col = 0;

    while (col < DISPLAY_WIDTH)
    {
        render_column(game, col, &game->rays[col]);
        col++;
    }
    render_laser_overlays(game);
}

void render_frame(t_game *game)
{
    int col = 0;

    while (col < DISPLAY_WIDTH)
    {
        render_column(game, col, &game->rays[col]);
        col++;
    }
    mlx_put_image_to_window(game->mlx, game->win, game->screen.ptr, 0, 0);
    render_weapon(game);
}

void render_ui(t_game *game)
{
    render_weapon(game);
    draw_health_bar(game);
    minimap(game);
}

void render_next_frame_enemies(t_game *game)
{
    int j = 0;
    
    while (j < game->num_enemies)
    {
        if (game->enemies[j].active)
            render_enemy(game, &game->enemies[j]);
        j++;
    }
}