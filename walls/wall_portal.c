#include "../cube3d.h"

static void	calculate_portal_texture_coordinates(t_render *renderer, t_ray *ray)
{
	if (ray->hit_vertical)
		renderer->tex_x = (int)(ray->wall_hit_y) % TILE_SIZE;
	else
		renderer->tex_x = (int)(ray->wall_hit_x) % TILE_SIZE;
}

static void	render_portal_pixel(t_game *game, t_render *renderer,
				int column_x, int texture_y)
{
	renderer->tex_addr = game->map.wall_portal_texture.addr
		+ (texture_y * game->map.wall_portal_texture.line_length + renderer
			->tex_x * (game->map.wall_portal_texture.bits_per_pixel / 8));
	renderer->color = *(unsigned int *)renderer->tex_addr;
	renderer->screen_pixel = game->screen.addr
		+ (renderer->y * game->screen.line_length
			+ column_x * (game->screen.bits_per_pixel / 8));
	*(unsigned int *)renderer->screen_pixel = renderer->color;
}

static void	render_portal_column(t_game *game, t_render *renderer, int column_x)
{
	int		cy;
	double	h;
	int		texture_y;
	float	rel;

	cy = (DISPLAY_HEIGHT / 2) + game->pitch;
	h = renderer->wall_height;
	renderer->y = renderer->draw_start;
	while (renderer->y <= renderer->draw_end)
	{
		if (renderer->y >= 0 && renderer->y < DISPLAY_HEIGHT)
		{
			rel = ((renderer->y - cy) / h) + 0.5f;
			texture_y = (int)(rel * TILE_SIZE);
			if (texture_y < 0)
				texture_y = 0;
			else if (texture_y >= TILE_SIZE)
				texture_y = TILE_SIZE - 1;
			render_portal_pixel(game, renderer, column_x, texture_y);
		}
		renderer->y++;
	}
}

void	render_wall_portal(t_game *game, int column_x,
			t_render *renderer, t_ray *ray)
{
	calculate_portal_texture_coordinates(renderer, ray);
	render_portal_column(game, renderer, column_x);
}
