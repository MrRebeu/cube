/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:55:05 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/12 17:37:26 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

void render_weapon(t_game *game)
{
    t_render r;
    t_img *weapon;
    
    weapon = &game->weapons[game->current_weapon];
    r.x = (DISPLAY_WIDTH - weapon->width) - 50;  // position x de l'arme
    r.y = (DISPLAY_HEIGHT - weapon->height) + 250;  // position y de l'arme
    r.tex_y = 0;
    while (r.tex_y < weapon->height)
    {
        r.tex_x = 0;
        while (r.tex_x < weapon->width)
        {
            r.tex_addr = weapon->addr + (r.tex_y * weapon->line_length + 
                         r.tex_x * (weapon->bits_per_pixel / 8));
            r.color = *(unsigned int*)r.tex_addr;
            r.r = (r.color >> 16) & 0xFF;
            r.g = (r.color >> 8) & 0xFF;
            r.b = r.color & 0xFF;
            if (!(r.r < 10 && r.g < 10 && r.b < 10))
            {
                mlx_pixel_put(game->mlx, game->win, r.x + r.tex_x, r.y + r.tex_y, 
                              mlx_get_color_value(game->mlx, r.color));
            }
            r.tex_x++;
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

void	render_wall(t_game *game, int column_x, t_render *r)
{
	double	step;
	double	texture_pos;
	int		texture_y;

	r->tex_x = column_x % TILE_SIZE;
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


void	render_door(t_game *game, int column_x, t_render *r)
{
		r->tex_x = column_x % TILE_SIZE;
	r->y = r->draw_start;
	while (r->y <= r->draw_end)
	{
		r->tex_y = (r->y - r->draw_start) * TILE_SIZE / r->door_height;
		if (r->tex_x >= 0 && r->tex_x < TILE_SIZE && r->tex_y >= 0
			&& r->tex_y < TILE_SIZE && game->map.door_texture.addr != NULL)
		{
			r->tex_addr = game->map.door_texture.addr + (r->tex_y
					* game->map.door_texture.line_length + r->tex_x
					* (game->map.door_texture.bits_per_pixel / 8));
			r->color = *(unsigned int *)(r->tex_addr);
		}
		else
			r->color = 0x654321;
		r->screen_pixel = game->screen.addr + (r->y * game->screen.line_length
				+ column_x * (game->screen.bits_per_pixel / 8));
		*(unsigned int *)(r->screen_pixel) = r->color;
		r->y++;
	}
}

void	render_floor(t_game *game, int column_x, t_render *r, t_ray *ray)
{
	unsigned int	base_color;

	r->y = r->draw_end + 1;
	while (r->y < DISPLAY_HEIGHT)
	{
		r->row_distance = (DISPLAY_HEIGHT / 2.0) / (r->y - DISPLAY_HEIGHT
				/ 2.0);
		r->floor_x = game->player.x + r->row_distance * cos(ray->radiant_angle)
			/ 4.0;
		r->floor_y = game->player.y + r->row_distance * sin(ray->radiant_angle)
			/ 4.0;
		r->tex_x = ((int)(r->floor_x * TILE_SIZE)) % TILE_SIZE;
		r->tex_y = ((int)(r->floor_y * TILE_SIZE)) % TILE_SIZE;
		r->dim_factor = 1.0 - fmin(1.0, r->row_distance / 15.0);
		if (r->tex_x >= 0 && r->tex_x < TILE_SIZE && r->tex_y >= 0
			&& r->tex_y < TILE_SIZE && game->map.floor_texture.addr != NULL)
		{
			r->tex_addr = game->map.floor_texture.addr + (r->tex_y
					* game->map.floor_texture.line_length + r->tex_x
					* (game->map.floor_texture.bits_per_pixel / 8));
			// Manipulation directe des composantes RGB
			base_color = *(unsigned int *)(r->tex_addr);
			r->r = ((base_color >> 16) & 0xFF) * r->dim_factor;
			r->g = ((base_color >> 8) & 0xFF) * r->dim_factor;
			r->b = (base_color & 0xFF) * r->dim_factor;
			r->color = (r->r << 16) | (r->g << 8) | r->b;
		}
		else
			r->color = 0x228B22;
		r->screen_pixel = game->screen.addr + (r->y * game->screen.line_length
				+ column_x * (game->screen.bits_per_pixel / 8));
		*(unsigned int *)(r->screen_pixel) = r->color;
		r->y++;
	}
}
