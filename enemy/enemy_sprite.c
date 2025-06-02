/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_sprite.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 23:09:16 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/26 23:09:21 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

void	draw_enemy_sprite(t_game *game, t_img *sprite,
		t_point pos, int size, t_enemy *enemy)
{
	int	i;
	int	j;

	(void)enemy;
	if (!sprite || !sprite->addr || size <= 0)
		return ;
	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			draw_sprite_pixel_at_pos(game, sprite, pos, size, i, j);
			j++;
		}
		i++;
	}
}

void	render_death_animation(t_game *game, t_enemy *enemy)
{
	t_render	renderer;
	t_img		*current_sprite;

	current_sprite = get_death_sprite(enemy);
	if (!setup_death_render(game, enemy, &renderer))
		return ;
	apply_death_modifiers(enemy, &renderer);
	if (!validate_death_render(&renderer))
		return ;
	if (!check_enemy_occlusion(game, &renderer))
		return ;
	finalize_death_render(game, &renderer);
	draw_enemy_sprite(game, current_sprite,
		(t_point){renderer.draw_start, renderer.draw_end},
		renderer.sprite_size, enemy);
}
