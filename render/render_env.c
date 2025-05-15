/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:55:05 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/15 16:55:21 by abkhefif         ###   ########.fr       */
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
    t_render r;
    t_img *weapon;
    char *dst;
    
    weapon = &game->weapons[game->current_weapon];
    r.x = (DISPLAY_WIDTH - weapon->width) - 50;  // position x de l'arme
    r.y = (DISPLAY_HEIGHT - weapon->height) + 250;  // position y de l'arme
    r.tex_y = 0;
    while (r.tex_y < weapon->height)
    {
        if (r.y + r.tex_y >= 0 && r.y + r.tex_y < DISPLAY_HEIGHT)
        {
            r.tex_x = 0;
            while (r.tex_x < weapon->width)
            {
                if (r.x + r.tex_x >= 0 && r.x + r.tex_x < DISPLAY_WIDTH)
                {
                    r.tex_addr = weapon->addr + (r.tex_y * weapon->line_length + 
                                r.tex_x * (weapon->bits_per_pixel / 8));
                    r.color = *(unsigned int*)r.tex_addr;
                    r.r = (r.color >> 16) & 0xFF;
                    r.g = (r.color >> 8) & 0xFF;
                    r.b = r.color & 0xFF;
                    if (!(r.r < 10 && r.g < 10 && r.b < 10))
                    {
                        dst = game->screen.addr + ((r.y + r.tex_y) * game->screen.line_length + 
                              (r.x + r.tex_x) * (game->screen.bits_per_pixel / 8));
                        *(unsigned int*)dst = r.color;
                    }
                }
                r.tex_x++;
            }
        }
        r.tex_y++;
    }
}


void	render_sky(t_game *game, int column_x, t_render *r)
{
	r->y = 0;
	r->color = 0x87CEEB;
	while (r->y < r->draw_start)
	{
		r->screen_pixel = game->screen.addr + (r->y * game->screen.line_length
				+ column_x * (game->screen.bits_per_pixel / 8));
		*(unsigned int *)(r->screen_pixel) = r->color;
		r->y++;
	}
}
void render_wall(t_game *game, int column_x, t_render *r, t_ray *ray)
{
    double step;
    double texture_pos;
    int texture_y;

    // Calcul de la coordonnée de texture en fonction du point d'impact exact
    if (ray->hit_vertical)
        r->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        r->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;

    r->y = r->draw_start;
    step = (double)TILE_SIZE / r->wall_height;
    texture_pos = (r->draw_start - (DISPLAY_HEIGHT / 2 - r->wall_height / 2)) * step;
    
    while (r->y <= r->draw_end)
    {
        if (r->y >= 0 && r->y < DISPLAY_HEIGHT)
        {
            texture_y = (int)texture_pos;
            if (r->tex_x >= 0 && r->tex_x < TILE_SIZE
                && texture_y >= 0 && texture_y < TILE_SIZE
                && game->map.wall_texture.addr != NULL)
            {
                r->tex_addr = game->map.wall_texture.addr + (texture_y
                    * game->map.wall_texture.line_length + r->tex_x
                    * (game->map.wall_texture.bits_per_pixel / 8));
                r->color = *(unsigned int *)(r->tex_addr);
            }
            else
                r->color = 0x654321;
                
            r->screen_pixel = game->screen.addr + (r->y * game->screen.line_length
                    + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)(r->screen_pixel) = r->color;
        }
        texture_pos = texture_pos + step;
        r->y++;
    }
}


void render_door(t_game *game, int column_x, t_render *r, t_ray *ray)
{
    double step;
    double texture_pos;
    int texture_y;

    // Calcul de la coordonnée de texture en fonction du point d'impact exact
    if (ray->hit_vertical)
        r->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
    else
        r->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;

    r->y = r->draw_start;
    step = (double)TILE_SIZE / r->wall_height;
    texture_pos = (r->draw_start - (DISPLAY_HEIGHT / 2 - r->wall_height / 2)) * step;
    
    while (r->y <= r->draw_end)
    {
        if (r->y >= 0 && r->y < DISPLAY_HEIGHT)
        {
            texture_y = (int)texture_pos;
            if (r->tex_x >= 0 && r->tex_x < TILE_SIZE
                && texture_y >= 0 && texture_y < TILE_SIZE
                && game->map.door_texture.addr != NULL)
            {
                r->tex_addr = game->map.door_texture.addr + (texture_y
                    * game->map.door_texture.line_length + r->tex_x
                    * (game->map.door_texture.bits_per_pixel / 8));
                r->color = *(unsigned int *)(r->tex_addr);
            }
            else
                r->color = 0x654321;
                
            r->screen_pixel = game->screen.addr + (r->y * game->screen.line_length
                    + column_x * (game->screen.bits_per_pixel / 8));
            *(unsigned int *)(r->screen_pixel) = r->color;
        }
        texture_pos = texture_pos + step;
        r->y++;
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


void render_floor(t_game *game, int column_x, t_render *r, t_ray *ray)
{
    unsigned int base_color;
    double floor_step_x, floor_step_y;
    double weight;

    r->y = r->draw_end + 1;
    while (r->y < DISPLAY_HEIGHT)
    {
        // Calcul du poids pour la distance
        weight = (DISPLAY_HEIGHT / (2.0 * r->y - DISPLAY_HEIGHT));
        
        // Coordonnées exactes du pixel du sol dans l'espace 3D
        // Utilise une projection plus précise
        floor_step_x = weight * (ray->wall_hit_x - game->player.x) / ray->distance + game->player.x;
        floor_step_y = weight * (ray->wall_hit_y - game->player.y) / ray->distance + game->player.y;
        
        // Calcul des coordonnées de texture (avec une échelle appropriée)
        r->tex_x = (int)(floor_step_x * 0.5) % TILE_SIZE;
        r->tex_y = (int)(floor_step_y * 0.5) % TILE_SIZE;
        
        // Facteur de gradation basé sur la distance
        r->dim_factor = 1.0 - fmin(1.0, (r->y - r->draw_end) / (double)(DISPLAY_HEIGHT - r->draw_end)) * 0.6;
        
        // S'assurer que les coordonnées de texture sont valides
        if (r->tex_x >= 0 && r->tex_x < TILE_SIZE && 
            r->tex_y >= 0 && r->tex_y < TILE_SIZE && 
            game->map.floor_texture.addr != NULL)
        {
            // Obtenir la couleur du pixel de la texture
            r->tex_addr = game->map.floor_texture.addr + (r->tex_y * 
                game->map.floor_texture.line_length + r->tex_x * 
                (game->map.floor_texture.bits_per_pixel / 8));
            
            // Appliquer le facteur de gradation aux composantes RGB
            base_color = *(unsigned int *)(r->tex_addr);
            r->r = ((base_color >> 16) & 0xFF) * r->dim_factor;
            r->g = ((base_color >> 8) & 0xFF) * r->dim_factor;
            r->b = (base_color & 0xFF) * r->dim_factor;
            r->color = (r->r << 16) | (r->g << 8) | r->b;
        }
        else
            r->color = 0x228B22; // Couleur verte par défaut
        
        // Dessiner le pixel
        r->screen_pixel = game->screen.addr + (r->y * game->screen.line_length
            + column_x * (game->screen.bits_per_pixel / 8));
        *(unsigned int *)(r->screen_pixel) = r->color;
        r->y++;
    }
}
