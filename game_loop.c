/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:43:09 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/12 17:43:10 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

int close_window(void)
{
    exit(0);
    return (0);
}

int render_next_frame(t_game *game)
{
    move_player(&game->player);

    render_scene(game);
    
    mlx_put_image_to_window(game->mlx, game->win, game->screen.ptr, 0, 0);
    
    // Rendu des éléments d'interface (armes, etc.)
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
    // Configuration des hooks
    mlx_hook(game->win, 2, 1L << 0, key_press, &game->player);     // Appui sur touche
    mlx_hook(game->win, 3, 1L << 1, key_release, &game->player);   // Relâchement de touche
    mlx_hook(game->win, 17, 1L << 17, close_window, game);         // Fermeture de fenêtre
    
    // Fonction appelée en boucle
    mlx_loop_hook(game->mlx, render_next_frame, game);
    
    // Lancement de la boucle principale
    mlx_loop(game->mlx);
    
    return (0);
}
