/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_sprite_pixel.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 23:09:16 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/26 23:09:21 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

static int	should_skip_pixel(unsigned int color, int enemy_state)
{
	int	red;
	int	green;
	int	blue;

	red = (color >> 16) & 0xFF;
	green = (color >> 8) & 0xFF;
	blue = color & 0xFF;
	if (enemy_state == DEAD)
		return (red > 180 && blue > 180 && green < 100);
	return (abs(red - 255) <= 2 && abs(green - 0) <= 2
		&& abs(blue - 0) <= 2);
}

static void	calculate_src_coords(t_pixel_data *pixel, t_img *sprite, int size)
{
	pixel->src_x = pixel->x * sprite->width / size;
	pixel->src_y = pixel->y * sprite->height / size;
	if (pixel->src_x < 0)
		pixel->src_x = 0;
	else if (pixel->src_x >= sprite->width)
		pixel->src_x = sprite->width - 1;
	if (pixel->src_y < 0)
		pixel->src_y = 0;
	else if (pixel->src_y >= sprite->height)
		pixel->src_y = sprite->height - 1;
}

static void	get_sprite_color(t_pixel_data *pixel, t_img *sprite)
{
	char	*src;

	src = sprite->addr + pixel->src_y * sprite->line_length
		+ pixel->src_x * (sprite->bits_per_pixel / 8);
	pixel->color = *(unsigned int *)src;
}

static void	draw_pixel_to_screen(t_game *game, t_pixel_data *pixel,
		t_point pos)
{
	char	*dst;
	int		screen_x;
	int		screen_y;

	screen_x = pos.x + pixel->x;
	screen_y = pos.y + pixel->y;
	if (screen_x >= 0 && screen_x < DISPLAY_WIDTH
		&& screen_y >= 0 && screen_y < DISPLAY_HEIGHT)
	{
		dst = game->screen.addr + screen_y * game->screen.line_length
			+ screen_x * (game->screen.bits_per_pixel / 8);
		*(unsigned int *)dst = pixel->color;
	}
}

void	draw_sprite_pixel_at_pos(t_game *game, t_img *sprite,
		t_point pos, int size, int i, int j)
{
	t_pixel_data	pixel;
	int				enemy_state;

	pixel.x = i;
	pixel.y = j;
	enemy_state = IDLE;
	calculate_src_coords(&pixel, sprite, size);
	get_sprite_color(&pixel, sprite);
	if (!should_skip_pixel(pixel.color, enemy_state))
		draw_pixel_to_screen(game, &pixel, pos);
}
