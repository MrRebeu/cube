/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 16:06:33 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/21 20:16:42 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

void render_door_column_fixed(t_game *game, t_open_door *door, int column_x, double distance)
{
    if (!door->sprite.ptr || distance < 5.0)
        return;
    
    // ✅ Calculer la hauteur basée sur la distance réelle
    double distance_to_projection = (DISPLAY_WIDTH / 2.0) / tan(game->player.fov / 2.0);
    double door_height_world = TILE_SIZE * 1.4;
    int door_height_screen = (int)((door_height_world / distance) * distance_to_projection);
    
    if (door_height_screen < 1) door_height_screen = 1;
    if (door_height_screen > DISPLAY_HEIGHT) door_height_screen = DISPLAY_HEIGHT;
    
    // ✅ Position verticale
    int door_top = (DISPLAY_HEIGHT / 2) - (door_height_screen / 2) + game->pitch;
    int door_bottom = door_top + door_height_screen;
    
    if (door_top < 0) door_top = 0;
    if (door_bottom >= DISPLAY_HEIGHT) door_bottom = DISPLAY_HEIGHT - 1;
    
    // ✅ Utiliser le centre de la texture (pour une porte fine)
    int tex_x = door->sprite.width / 2;
    
    // ✅ Dessiner uniquement sur cette colonne
    for (int y = door_top; y <= door_bottom; y++)
    {
        // Position Y dans la texture
        int tex_y = ((y - door_top) * door->sprite.height) / (door_bottom - door_top + 1);
        if (tex_y >= door->sprite.height) tex_y = door->sprite.height - 1;
        
        // Récupérer le pixel
        char *src = door->sprite.addr + tex_y * door->sprite.line_length + 
                   tex_x * (door->sprite.bits_per_pixel / 8);
        unsigned int color = *(unsigned int *)src;
        
        int red = (color >> 16) & 0xFF;
        int green = (color >> 8) & 0xFF;
        int blue = color & 0xFF;
        
        // ✅ Transparence stricte - ne garder que les montants
        int is_transparent = 0;
        
        if (red < 20 && green < 20 && blue < 20) // Noir
            is_transparent = 1;
        if (red > 240 && green > 240 && blue > 240) // Blanc
            is_transparent = 1;
        if (abs(red - 255) <= 5 && green <= 5 && blue <= 5) // Rouge pur
            is_transparent = 1;
        
        // ✅ Dessiner seulement les pixels opaques (montants)
        if (!is_transparent)
        {
            char *dst = game->screen.addr + y * game->screen.line_length + 
                       column_x * (game->screen.bits_per_pixel / 8);
            *(unsigned int *)dst = color;
        }
    }
}

void	render_frame(t_game *game)
{
	int	col;

	col = 0;
	while (col < DISPLAY_WIDTH)
	{
		render_column(game, col, &game->rays[col]);
		col++;
	}
	mlx_put_image_to_window(game->mlx, game->win, game->screen.ptr, 0, 0);
	render_weapon(game);
}


