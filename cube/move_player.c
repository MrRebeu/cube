#include "../cube3d.h"

int	key_release(int keycode, t_player *player)
{
	if (keycode == W)
		player->key_up = false;
	if (keycode == S)
		player->key_down = false;
	if (keycode == A)
		player->key_left = false;
	if (keycode == D)
		player->key_right = false;
	if (keycode == Q)
		player->left_rotate = false;
	if (keycode == E)
		player->right_rotate = false;
	if (keycode == R)
		player->turn_back = false;
	return (0);
}

int	key_press(int keycode, t_player *player)
{
	if (keycode == W)
		player->key_up = true;
	if (keycode == S)
		player->key_down = true;
	if (keycode == A)
		player->key_left = true;
	if (keycode == D)
		player->key_right = true;
	if (keycode == Q)
		player->left_rotate = true;
	if (keycode == E)
		player->right_rotate = true;
	if (keycode == R)
		player->turn_back = true;
	return (0);
}

void move_player(t_player *player)
{
	float angle_speed;
	int speed;
	float cos_angle;
	float sin_angle;
	float new_x;
	float new_y;

	cos_angle = cos(player->angle);
	sin_angle = sin(player->angle);
	angle_speed = 0.05;
	speed= 3;
	player->fov = player->angle;
	if (player->left_rotate)
		player->angle -= angle_speed;
	if (player->right_rotate)
		player->angle += angle_speed;
	if (player->angle > M_PI * 2)
		player->angle = 0;
	if (player->angle < 0)
		player->angle = 2 * M_PI;
	if (player->key_up)
	{
		new_x = player->x + cos_angle * speed;
		new_y = player->y + sin_angle * speed;
		// if (!is_wall(game, new_x, new_y))
		// {
			player->x = new_x;
			player->y = new_y;
		//}
	}
	if (player->key_down)
	{
		float new_x = player->x - cos_angle * speed;
		float new_y = player->y - sin_angle * speed;
		// if (!is_wall(game, new_x, new_y))
		// {
			player->x = new_x;
			player->y = new_y;
		// }
	}
}
