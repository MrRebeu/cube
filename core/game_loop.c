/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:43:09 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/16 18:02:06 by abkhefif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

int close_window(void *param)
{
    (void)param;
    exit(0);
    return (0);
}

int render_next_frame(t_game *game)
{
    move_player(&game->player);
    int i = 0;
    while (i < DISPLAY_WIDTH)
    {
        double ray_offset = game->player.fov * ((double)i / DISPLAY_WIDTH - 0.5);
        double radiant_angle = game->player.angle + ray_offset;
        // Normaliser l'angle
        radiant_angle = normalize_angle(radiant_angle);

        game->rays[i].radiant_angle = radiant_angle;
        game->rays[i].player_angle = game->player.angle;

        // Recalculer la distance avec la position actuelle du joueur
        game->rays[i].distance = ray_casting(game, radiant_angle, i);
        
        i++;
    }

    render_scene(game);
    //render_pnj(game);

    draw_crosshair(game);
    mlx_put_image_to_window(game->mlx, game->win, game->screen.ptr, 0, 0);
    
    render_ui(game);
    
    return (0);
}

void render_scene(t_game *game)
{
    int col;
    
    col = 0;
    while (col < DISPLAY_WIDTH)
    {
        render_column(game, col, &game->rays[col]);
        col++;
    }
}

void render_ui(t_game *game)
{
    render_weapon(game);
}

int loop_game(t_game *game)
{
    mlx_hook(game->win, 2, 1L << 0, key_press, &game->player);     // Appui sur touche
    mlx_hook(game->win, 3, 1L << 1, key_release, &game->player);   // Relâchement de touche
    mlx_hook(game->win, 17, 1L << 17, close_window, game);         // Fermeture de fenêtre
    


// Nouvel hook pour la souris (MotionNotify est l'événement 6 sous X11)
    mlx_hook(game->win, 6, 1L << 6, mouse_move, game);
    mlx_loop_hook(game->mlx, render_next_frame, game);
    
    mlx_loop(game->mlx);
    
    return (0);
}
