#include "./cube3d.h"

void	draw_minimap_background(t_game *game)
{
	t_minimap *mini_map = &game->minimap;
	char	*pixel;
	int		x;
	int y;

	y = mini_map->y;
	while (y < mini_map->y + mini_map->size)
	{
		x = mini_map->x;
		while (x < mini_map->x + mini_map->size)
		{
			//if border
			if (x < mini_map->x + mini_map->border || x >= mini_map->x + mini_map->size - mini_map->border ||
                y < mini_map->y + mini_map->border || y >= mini_map->y + mini_map->size - mini_map->border)
            {
				//write pixel border color
                pixel = game->screen.addr + (y * game->screen.line_length + 
                        x * (game->screen.bits_per_pixel / 8));
                *(unsigned int*)pixel = mini_map->border_color;
			}
			else
			{	// backgroud color 
				pixel = game->screen.addr + (y * game->screen.line_length + 
                        x * (game->screen.bits_per_pixel / 8));
                *(unsigned int*)pixel = mini_map->bg_color;
			}
			x++;
		}
		y++;
	}
}
void draw_minimap_cell(t_game *game, int map_x, int map_y, int screen_x, int screen_y)
{
	t_minimap *mini_map = &game->minimap;
	char	*pixel;
	char	cell_type;
	unsigned int cell_color;
	int row;
	int col;
	if (map_x >= 0 && map_x < game->map.width && 
        map_y >= 0 && map_y < game->map.height)
	{
		cell_type = game->map.matrix[map_y][map_x];
		if (cell_type == '1')
			cell_color = mini_map->wall_color;
		else if (cell_type == 'D')
            cell_color = mini_map->door_color;
        else if (cell_type == 'P')
            cell_color = mini_map->portal_color;
        else
            cell_color = mini_map->bg_color;
		row = 0;
		while (row < mini_map->cell_size)
		{
			col = 0;
			while (col < mini_map->cell_size)
			{
				int screen_pixel_x = screen_x + col;
				int screen_pixel_y = screen_y + row;
				if (screen_pixel_x >= mini_map->x + mini_map->border && 
                    screen_pixel_x < mini_map->x + mini_map->size - mini_map->border &&
                    screen_pixel_y >= mini_map->y + mini_map->border && 
                    screen_pixel_y < mini_map->y + mini_map->size - mini_map->border)
				{
					pixel = game->screen.addr + 
                            (screen_pixel_y * game->screen.line_length + 
                             screen_pixel_x * (game->screen.bits_per_pixel / 8));
                    *(unsigned int*)pixel = cell_color;
				}
				col++;
			}
			row++;
		}
	}
}

void draw_minimap_cells(t_game *game)
{
    t_minimap *mini_map = &game->minimap;
    int center_x = mini_map->x + mini_map->size / 2;
    int center_y = mini_map->y + mini_map->size / 2;
    
    // Position du joueur sur la carte
    int player_map_x = (int)(game->player.x / TILE_SIZE);
    int player_map_y = (int)(game->player.y / TILE_SIZE);
    
    // Parcourir de -visible_radius à +visible_radius
    int row = -mini_map->visible_radius;
    while (row <= mini_map->visible_radius)
    {
        int col = -mini_map->visible_radius;
        while (col <= mini_map->visible_radius)
        {
            // Calculer les coordonnées de la carte
            int map_x = player_map_x + col;
            int map_y = player_map_y + row;
            
            // Calculer les coordonnées à l'écran
            int cell_x = center_x + col * mini_map->cell_size;
            int cell_y = center_y + row * mini_map->cell_size;
            
            // Dessiner la cellule
            draw_minimap_cell(game, map_x, map_y, cell_x, cell_y);
            col++;
        }
        row++;
    }
}

void draw_minimap_player(t_game *game)
{
    t_minimap *mini_map = &game->minimap;
    char *pixel;
    
    // Centre de la mini-carte
    int center_x = mini_map->x + mini_map->size / 2;
    int center_y = mini_map->y + mini_map->size / 2;
    
    // Dessiner le joueur (cercle)
    int player_size = 4;
    for (int py = -player_size; py <= player_size; py++)
    {
        for (int px = -player_size; px <= player_size; px++)
        {
            if (px*px + py*py <= player_size*player_size)
            {
                int x = center_x + px;
                int y = center_y + py;
                
                if (x >= mini_map->x + mini_map->border && x < mini_map->x + mini_map->size - mini_map->border &&
                    y >= mini_map->y + mini_map->border && y < mini_map->y + mini_map->size - mini_map->border)
                {
                    pixel = game->screen.addr + (y * game->screen.line_length + 
                            x * (game->screen.bits_per_pixel / 8));
                    *(unsigned int*)pixel = mini_map->player_color;
                }
            }
        }
    }
    
    int dir_length = 15;
for (int i = 0; i < dir_length; i++)
{
    int x = center_x + cos(game->player.angle) * i;
    int y = center_y + sin(game->player.angle) * i;
    
    if (x >= mini_map->x + mini_map->border && 
        x < mini_map->x + mini_map->size - mini_map->border &&
        y >= mini_map->y + mini_map->border && 
        y < mini_map->y + mini_map->size - mini_map->border)
    {
        pixel = game->screen.addr + (y * game->screen.line_length + 
                x * (game->screen.bits_per_pixel / 8));
        *(unsigned int*)pixel = mini_map->player_color;
    }
}
}

void	minimap(t_game *game)
{
	if (!game->minimap.show)
		return ;
	draw_minimap_background(game);
	draw_minimap_cells(game);
	draw_minimap_player(game);
}
