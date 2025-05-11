#include "../cube3d.h"

void	render_column(t_game *game, int column_x, t_ray *ray)
{
	t_render	r;

	// Calcul des dimensions du mur
	r.corrected_dist = no_fish_eye(ray->distance, ray->radiant_angle,
			ray->player_angle);
	r.wall_height = calc_wall_height(r.corrected_dist);
	r.draw_start = (DISPLAY_HEIGHT / 2) - (r.wall_height / 2);
	r.draw_end = (DISPLAY_HEIGHT / 2) + (r.wall_height / 2);
	// Ajustement des limites
	if (r.draw_start < 0)
		r.draw_start = 0;
	if (r.draw_end >= DISPLAY_HEIGHT)
		r.draw_end = DISPLAY_HEIGHT - 1;
	// Rendu des différentes parties de la colonne
	render_sky(game, column_x, &r);
	render_wall(game, column_x, &r);
	render_floor(game, column_x, &r, ray);
}
void render_frame(t_game *game)
{
    int col;
	col = 0;
    while (col < DISPLAY_WIDTH)
    {
        render_column(game, col, &game->rays[col]);
        col++;
    }
    mlx_put_image_to_window(game->mlx, game->win, game->screen.ptr, 0, 0);
    render_weapon(game);
}
