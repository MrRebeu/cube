/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:55:05 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/15 17:10:59 by abkhefif         ###   ########.fr       */
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

void render_weapon(t_game *game)
{
    t_render renderer;
    t_img *weapon;
    char *dst;
    
    weapon = &game->weapons[game->current_weapon];
    renderer.x = (DISPLAY_WIDTH - weapon->width) - 50;  // Weapon X position
    renderer.y = (DISPLAY_HEIGHT - weapon->height) + 250;  // Weapon Y position
    
    /* Draw weapon image pixel by pixel */
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
                    /* Get weapon texture pixel */
                    renderer.tex_addr = weapon->addr + (renderer.tex_y * weapon->line_length + 
                                renderer.tex_x * (weapon->bits_per_pixel / 8));
                    renderer.color = *(unsigned int*)renderer.tex_addr;
                    
                    /* Extract RGB components */
                    renderer.red = (renderer.color >> 16) & 0xFF;
                    renderer.green = (renderer.color >> 8) & 0xFF;
                    renderer.blue = renderer.color & 0xFF;
                    
                    /* Skip nearly black pixels (transparency) */
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
    renderer->color = 0x87CEEB; // Sky blue color
    
    /* Draw sky from top of screen to wall top */
    while (renderer->y < renderer->draw_start)
    {
        renderer->screen_pixel = game->screen.addr + (renderer->y * game->screen.line_length
                + column_x * (game->screen.bits_per_pixel / 8));
        *(unsigned int *)(renderer->screen_pixel) = renderer->color;
        renderer->y++;
    }
}
void render_wall(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    double step;
    double texture_pos;
    int texture_y;

    /* Calculate texture coordinate based on exact hit point */
    if (ray->hit_vertical)
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;

    renderer->y = renderer->draw_start;
    step = (double)TILE_SIZE / renderer->wall_height;
    texture_pos = (renderer->draw_start - (DISPLAY_HEIGHT / 2 - renderer->wall_height / 2)) * step;
    
    /* Draw wall pixel by pixel */
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            texture_y = (int)texture_pos;
            if (renderer->tex_x >= 0 && renderer->tex_x < TILE_SIZE
                && texture_y >= 0 && texture_y < TILE_SIZE
                && game->map.wall_texture.addr != NULL)
            {
                /* Get texture pixel color */
                renderer->tex_addr = game->map.wall_texture.addr + (texture_y
                    * game->map.wall_texture.line_length + renderer->tex_x
                    * (game->map.wall_texture.bits_per_pixel / 8));
                renderer->color = *(unsigned int *)(renderer->tex_addr);
            }
            else
                renderer->color = 0x654321; // Default brown color
            
            /* Draw pixel to screen buffer */
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

    /* Calculate texture coordinate based on exact hit point */
    if (ray->hit_vertical)
        renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;

    renderer->y = renderer->draw_start;
    step = (double)TILE_SIZE / renderer->wall_height;
    texture_pos = (renderer->draw_start - (DISPLAY_HEIGHT / 2 - renderer->wall_height / 2)) * step;
    
    /* Draw door pixel by pixel */
    while (renderer->y <= renderer->draw_end)
    {
        if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
        {
            texture_y = (int)texture_pos;
            if (renderer->tex_x >= 0 && renderer->tex_x < TILE_SIZE
                && texture_y >= 0 && texture_y < TILE_SIZE
                && game->map.door_texture.addr != NULL)
            {
                /* Get texture pixel color */
                renderer->tex_addr = game->map.door_texture.addr + (texture_y
                    * game->map.door_texture.line_length + renderer->tex_x
                    * (game->map.door_texture.bits_per_pixel / 8));
                renderer->color = *(unsigned int *)(renderer->tex_addr);
            }
            else
                renderer->color = 0x654321; // Default brown color
            
            /* Draw pixel to screen buffer */
            renderer->screen_pixel = game->screen.addr + (renderer->y * game->screen.line_length
                    + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)(renderer->screen_pixel) = renderer->color;
        }
        texture_pos = texture_pos + step;
        renderer->y++;
    }
}

/*

Explanation: Why Adding the Ray Parameter Fixes the Door Rendering
The key improvement in our rendering code comes from passing the ray parameter to the rendering functions. This change is crucial for accurate texture mapping and solving the duplicate door problem.
The Previous Issue
Before the fix, we were using column_x % TILE_SIZE to determine the horizontal texture coordinate (tex_x). This approach had a critical flaw:

It created a repeating pattern based solely on the screen column
Multiple screen columns pointing at the same door would show different parts of the door texture
This created a visual "duplication" effect where one door in the map appeared as multiple doors on screen

The Solution: Using Ray Impact Data
By passing the ray parameter to the rendering functions, we gain access to the exact coordinates where each ray hits the wall or door. This allows us:

Accurate Texture Mapping: Instead of using the arbitrary screen column for texture mapping, we use the actual hit position (wall_hit_x or wall_hit_y).
Consistent Door Rendering: All rays hitting the same door cell now sample the texture based on their relative position within that cell, creating a single, coherent door image.
Correct Perspective: The texture coordinates now properly reflect the 3D space, maintaining correct perspective and scale regardless of viewing angle.
Distinction Between Wall Types: The rendering system can now properly differentiate between walls and doors based on the ray's hit_type information.

This approach aligns with how professional raycasting engines handle texturing, creating a more realistic and accurate representation of the game world.

*/


void render_floor(t_game *game, int column_x, t_render *renderer, t_ray *ray)
{
    unsigned int base_color;
    double floor_step_x, floor_step_y;
    double weight;

    renderer->y = renderer->draw_end + 1;
    
    /* Draw floor from wall bottom to screen bottom */
    while (renderer->y < DISPLAY_HEIGHT)
    {
        // Calculate weight for distance
        weight = (DISPLAY_HEIGHT / (2.0 * renderer->y - DISPLAY_HEIGHT));
        
        // Calculate exact floor pixel coordinates in 3D space
        // Uses more precise projection
        floor_step_x = weight * (ray->wall_hit_x - game->player.x) / ray->distance + game->player.x;
        floor_step_y = weight * (ray->wall_hit_y - game->player.y) / ray->distance + game->player.y;
        
        // Calculate texture coordinates (with appropriate scaling)
        renderer->tex_x = (int)(floor_step_x * 0.5) % TILE_SIZE;
        renderer->tex_y = (int)(floor_step_y * 0.5) % TILE_SIZE;
        
        // Darkening factor based on distance
        renderer->dim_factor = 1.0 - fmin(1.0, (renderer->y - renderer->draw_end) / 
                             (double)(DISPLAY_HEIGHT - renderer->draw_end)) * 0.6;
        
        // Ensure texture coordinates are valid
        if (renderer->tex_x >= 0 && renderer->tex_x < TILE_SIZE && 
            renderer->tex_y >= 0 && renderer->tex_y < TILE_SIZE && 
            game->map.floor_texture.addr != NULL)
        {
            // Get texture pixel color
            renderer->tex_addr = game->map.floor_texture.addr + (renderer->tex_y * 
                game->map.floor_texture.line_length + renderer->tex_x * 
                (game->map.floor_texture.bits_per_pixel / 8));
            
            // Apply darkening factor to RGB components
            base_color = *(unsigned int *)(renderer->tex_addr);
            renderer->red = ((base_color >> 16) & 0xFF) * renderer->dim_factor;
            renderer->green = ((base_color >> 8) & 0xFF) * renderer->dim_factor;
            renderer->blue = (base_color & 0xFF) * renderer->dim_factor;
            renderer->color = (renderer->red << 16) | (renderer->green << 8) | renderer->blue;
        }
        else
            renderer->color = 0x228B22; // Default green color
        
        // Draw pixel to screen buffer
        renderer->screen_pixel = game->screen.addr + (renderer->y * game->screen.line_length
            + column_x * (game->screen.bits_per_pixel / 8));
        *(unsigned int *)(renderer->screen_pixel) = renderer->color;
        renderer->y++;
    }
}
