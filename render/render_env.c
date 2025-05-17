/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:55:05 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/17 19:36:55 by abkhefif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// Improved weapon rendering to eliminate flickering:

// This modification fixes the weapon flickering issue by changing how the weapon is rendered.

// PROBLEM:
// - The previous implementation used mlx_pixel_put() to draw the weapon directly to the window
// - This caused visible flickering because:
//   1. Each pixel was drawn individually and directly to the screen
//   2. Drawing was done after the main scene was already displayed
//   3. mlx_pixel_put() is slow (makes a separate X server call for each pixel)

// SOLUTION:
// - Now we draw the weapon directly into the main image buffer (game->screen)
// - We use direct memory manipulation like we do for walls, floor, etc.
// - The entire scene (3D world + weapon) is prepared in memory first
// - Then everything is displayed at once with a single mlx_put_image_to_window() call

// BENEFITS:
// - No flickering - the player sees only completed frames
// - Better performance - direct memory access is much faster
// - Better synchronization - everything appears on screen simultaneously
// - Follows standard game rendering practices (buffer rendering)
// */

#include "../cube3d.h"

// FOR PISTO PORTAL
void render_weapon(t_game *game)
{
    t_render renderer;
    t_img *weapon;
    char *dst;

    weapon = &game->weapons[game->current_weapon];
    renderer.x = (DISPLAY_WIDTH - weapon->width) + 180;
    renderer.y = (DISPLAY_HEIGHT - weapon->height) + 250;
    renderer.tex_y = 0;
    while (renderer.tex_y < weapon->height)
    {
        if (renderer.y + renderer.tex_y >= 0 && renderer.y + renderer.tex_y < DISPLAY_HEIGHT)
        {
            renderer.tex_x = 0;
            while (renderer.tex_x < weapon->width)
            {
                if (renderer.x + renderer.tex_x >= 0 && renderer.x + renderer.tex_x < DISPLAY_WIDTH)
                {
                    renderer.tex_addr = weapon->addr + (renderer.tex_y * weapon->line_length +
                                renderer.tex_x * (weapon->bits_per_pixel / 8));
                    renderer.color = *(unsigned int*)renderer.tex_addr;

                    renderer.red = (renderer.color >> 16) & 0xFF;
                    renderer.green = (renderer.color >> 8) & 0xFF;
                    renderer.blue = renderer.color & 0xFF;
                    if (!(renderer.red < 10 && renderer.green< 10 && renderer.blue< 10))
                    {
                        dst = game->screen.addr + ((renderer.y + renderer.tex_y) * game->screen.line_length +
                              (renderer.x + renderer.tex_x) * (game->screen.bits_per_pixel / 8));
                        *(unsigned int*)dst = renderer.color;
                    }
                }
                renderer.tex_x++;
            }
        }
        renderer.tex_y++;
    }
}


void render_sky(t_game *game, int column_x, t_render *renderer)
{
    renderer->y = 0;
    renderer->color = 0xb0a56b; //87CEEB; // Sky blue color
    while (renderer->y < renderer->draw_start)
    {
        renderer->screen_pixel = game->screen.addr + (renderer->y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
        *(unsigned int *)(renderer->screen_pixel) = renderer->color;
        renderer->y++;
    }
}

void render_wall_portal(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    double step;
    double texture_pos;
    int texture_y;

    if (ray->hit_vertical)
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;
    renderer->y = renderer->draw_start;
    step = (double)TILE_SIZE / renderer->wall_height;
    texture_pos = (renderer->draw_start - (DISPLAY_HEIGHT / 2 - renderer->wall_height / 2)) * step;
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            texture_y = (int)texture_pos;
            if (renderer->tex_x >= 0 && renderer->tex_x < TILE_SIZE
                && texture_y >= 0 && texture_y < TILE_SIZE
                && game->map.wall_portal_texture.addr != NULL)
            {
                renderer->tex_addr = game->map.wall_portal_texture.addr + (texture_y
                    * game->map.wall_portal_texture.line_length + renderer->tex_x
                    * (game->map.wall_portal_texture.bits_per_pixel / 8));
                renderer->color = *(unsigned int *)(renderer->tex_addr);
            }
            else
                renderer->color = 0x654321;
            renderer->screen_pixel = game->screen.addr + (renderer->y * game->screen.line_length
                    + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)(renderer->screen_pixel) = renderer->color;
        }
        texture_pos = texture_pos + step;
        renderer->y++;
    }
}

void render_wall(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    double step;
    double texture_pos;
    int texture_y;

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
    step = (double)TILE_SIZE / renderer->wall_height;
    texture_pos = (renderer->draw_start - (DISPLAY_HEIGHT / 2 - renderer->wall_height / 2)) * step;
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            texture_y = (int)texture_pos;
            if (renderer->tex_x >= 0 && renderer->tex_x < TILE_SIZE
                && texture_y >= 0 && texture_y < TILE_SIZE
                && game->map.wall_texture.addr != NULL)
            {
                renderer->tex_addr = game->map.wall_texture.addr + (texture_y
                    * game->map.wall_texture.line_length + renderer->tex_x
                    * (game->map.wall_texture.bits_per_pixel / 8));
                renderer->color = *(unsigned int *)(renderer->tex_addr);
            }
            else
                renderer->color = 0x654321; // Default brown color
            renderer->screen_pixel = game->screen.addr + (renderer->y * game->screen.line_length
                    + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)(renderer->screen_pixel) = renderer->color;
        }
        texture_pos = texture_pos + step;
        renderer->y++;
    }
}


void render_door(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    double step;
    double texture_pos;
    int texture_y;

    if (ray->hit_vertical)
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;
    renderer->y = renderer->draw_start;
    step = (double)TILE_SIZE / renderer->wall_height;
    texture_pos = (renderer->draw_start - (DISPLAY_HEIGHT / 2 - renderer->wall_height / 2)) * step;
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            texture_y = (int)texture_pos;
            if (renderer->tex_x >= 0 && renderer->tex_x < TILE_SIZE
                && texture_y >= 0 && texture_y < TILE_SIZE
                && game->map.door_texture.addr != NULL)
            {
                renderer->tex_addr = game->map.door_texture.addr + (texture_y
                    * game->map.door_texture.line_length + renderer->tex_x
                    * (game->map.door_texture.bits_per_pixel / 8));
                renderer->color = *(unsigned int *)(renderer->tex_addr);
            }
            else
                renderer->color = 0x654321;
            renderer->screen_pixel = game->screen.addr + (renderer->y * game->screen.line_length
                    + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)(renderer->screen_pixel) = renderer->color;
        }
        texture_pos = texture_pos + step;
        renderer->y++;
    }
}


//render floor = render floor + sky (no sprite just color like in wolf3D)
void render_floor(t_game *game, int column_x, t_render *renderer)
{
    int y;
    unsigned int floor_color = 0x444444;
    unsigned int ceiling_color = 0x777777;

    y = 0;
    while (y < renderer->draw_start)
    {
        renderer->screen_pixel = game->screen.addr + (y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
        *(unsigned int *)(renderer->screen_pixel) = ceiling_color;
        y++;
    }
    y = renderer->draw_end + 1;
    while (y < DISPLAY_HEIGHT)
    {
        renderer->screen_pixel = game->screen.addr + (y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
        *(unsigned int *)(renderer->screen_pixel) = floor_color;
        y++;
    }
}
