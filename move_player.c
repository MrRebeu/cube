/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   move_player.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcaccava <tcaccava@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:44:16 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/14 23:16:53 by tcaccava         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cube3d.h"

int	key_press(int keycode, t_game *game)
{
	if (keycode == W)
		game->player.key_up = true;
	if (keycode == S)
		game->player.key_down = true;
	if (keycode == A)
		game->player.key_left = true;
	if (keycode == D)
		game->player.key_right = true;
	if (keycode == Q)
		game->player.left_rotate = true;
	if (keycode == E)
		game->player.right_rotate = true;
	if (keycode == R)
		game->player.turn_back = true;
	return (0);
}

int	key_release(int keycode, t_game *game)
{
	if (keycode == W)
		game->player.key_up = false;
	if (keycode == S)
		game->player.key_down = false;
	if (keycode == A)
		game->player.key_left = false;
	if (keycode == D)
		game->player.key_right = false;
	if (keycode == Q)
		game->player.left_rotate = false;
	if (keycode == E)
		game->player.right_rotate = false;
	if (keycode == R)
		game->player.turn_back = false;
	return (0);
}
int	is_wall(t_map *map, double x, double y)
{
	int	map_x;
	int	map_y;

	map_x = (int)(x / TILE_SIZE);
	map_y = (int)(y / TILE_SIZE);
	if (map_x < 0 || map_x >= map->width || map_y < 0 || map_y >= map->height)
		return (1);
	if (map->matrix[map_y][map_x] == '1')
		return (1);
	return (0);
}
int try_move(t_game *game, double new_x, double new_y)
{
    // Check collision for both new coordinates at once
    if (!is_wall(&game->map, new_x, new_y))
    {
        // Only update both coordinates if the movement is valid
        game->player.x = new_x;
        game->player.y = new_y;
        return (1);
    }
    
    // If we can't move diagonally, try moving only horizontally
    if (!is_wall(&game->map, new_x, game->player.y))
    {
        game->player.x = new_x;
        return (1);
    }
    
    // If we can't move horizontally, try moving only vertically
    if (!is_wall(&game->map, game->player.x, new_y))
    {
        game->player.y = new_y;
        return (1);
    }
    
    return (0);
}


void move_player(t_game *game)
{
    const double angle_speed = 0.05;
    const double move_speed = 13.0;
    double cos_a = cos(game->player.angle);
    double sin_a = sin(game->player.angle);
    double new_x = game->player.x;
    double new_y = game->player.y;

    // Handle rotations
    if (game->player.left_rotate)
        game->player.angle = normalize_angle(game->player.angle - angle_speed);
    if (game->player.right_rotate)
        game->player.angle = normalize_angle(game->player.angle + angle_speed);
    if (game->player.turn_back)
    {
        game->player.angle = normalize_angle(game->player.angle + M_PI);
        game->player.turn_back = false;
    }

    // Calculate movement vector
    double move_x = 0;
    double move_y = 0;

    // Forward/backward movement
    if (game->player.key_up)
    {
        move_x += cos_a * move_speed;
        move_y += sin_a * move_speed;
    }
    if (game->player.key_down)
    {
        move_x -= cos_a * move_speed;
        move_y -= sin_a * move_speed;
    }

    // Strafe movement
    if (game->player.key_left)
    {
        move_x -= sin_a * move_speed;
        move_y += cos_a * move_speed;
    }
    if (game->player.key_right)
    {
        move_x += sin_a * move_speed;
        move_y -= cos_a * move_speed;
    }

    // Calculate new position
    new_x = game->player.x + move_x;
    new_y = game->player.y + move_y;

    // Try to move to new position with collision detection
    // Check for wall collisions with a bit of margin to prevent getting too close to walls
    double margin = TILE_SIZE * 0.2;
    
    // Check collision at player's position plus some margin in movement direction
    if (!is_wall(&game->map, new_x, new_y))
    {
        // Also check the corners of the player (represented as a small square)
        int can_move = 1;
        
        if (is_wall(&game->map, new_x + margin, new_y + margin) ||
            is_wall(&game->map, new_x + margin, new_y - margin) ||
            is_wall(&game->map, new_x - margin, new_y + margin) ||
            is_wall(&game->map, new_x - margin, new_y - margin))
        {
            can_move = 0;
        }
        
        if (can_move)
        {
            game->player.x = new_x;
            game->player.y = new_y;
        }
        else
        {
            // Try moving only on x-axis
            if (!is_wall(&game->map, new_x, game->player.y) &&
                !is_wall(&game->map, new_x + margin, game->player.y + margin) &&
                !is_wall(&game->map, new_x + margin, game->player.y - margin) &&
                !is_wall(&game->map, new_x - margin, game->player.y + margin) &&
                !is_wall(&game->map, new_x - margin, game->player.y - margin))
            {
                game->player.x = new_x;
            }
            
            // Try moving only on y-axis
            if (!is_wall(&game->map, game->player.x, new_y) &&
                !is_wall(&game->map, game->player.x + margin, new_y + margin) &&
                !is_wall(&game->map, game->player.x + margin, new_y - margin) &&
                !is_wall(&game->map, game->player.x - margin, new_y + margin) &&
                !is_wall(&game->map, game->player.x - margin, new_y - margin))
            {
                game->player.y = new_y;
            }
        }
    }
}

void	update_rays(t_game *game)
{
	int x;
	double ray_angle;
	double angle_step;

	angle_step = FOV / DISPLAY_WIDTH;
	ray_angle = game->player.angle - (FOV / 2);
	x = 0;
	while (x < DISPLAY_WIDTH)
	{
		game->rays[x].radiant_angle = normalize_angle(ray_angle);
		game->rays[x].player_angle = game->player.angle;
		game->rays[x].distance = ray_casting(game, game->rays[x].radiant_angle,
				x);
		ray_angle = ray_angle + angle_step;
		x++;
	}
}