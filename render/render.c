/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 16:06:33 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/22 17:24:53 by abkhefif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

void render_column(t_game *game, int column_x, t_ray *ray)
{
    t_render renderer;

    /* 1. Calculate wall dimensions */
    renderer.corrected_dist = no_fish_eye(ray->distance, ray->radiant_angle, ray->player_angle);
    
    // Calculate wall height
    renderer.wall_height = calc_wall_height(renderer.corrected_dist);
    renderer.door_height = (int)(renderer.wall_height * 1.3);
    
    /* 2. Determine vertical rendering boundaries */
    renderer.draw_start = (DISPLAY_HEIGHT / 2) - (renderer.wall_height / 2) + game->pitch;
    renderer.draw_end = (DISPLAY_HEIGHT / 2) + (renderer.wall_height / 2) + game->pitch;
    
    // Handle texture offset for very tall walls
    renderer.texture_offset_y = 0;
    if (renderer.wall_height > DISPLAY_HEIGHT)
        renderer.texture_offset_y = (renderer.wall_height - DISPLAY_HEIGHT) / 2;
    
    // Clamp values to screen dimensions
    if (renderer.draw_start < 0)
        renderer.draw_start = 0;
    if (renderer.draw_end >= DISPLAY_HEIGHT)
        renderer.draw_end = DISPLAY_HEIGHT - 1;
    
    /* 3. Render column sequentially */
    if (ray->hit_type == 'P')
        render_wall_portal(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'D')
        render_door(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'i')
        render_wall_shooted(game, column_x, &renderer, ray);
    else if (ray->hit_type == 'd')
        render_door_shooted(game, column_x, &renderer, ray);
    else
        render_wall(game, column_x, &renderer, ray);
        
    render_floor(game, column_x, &renderer);
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


