#include "cube3d.h"

void	apply_damage_effect(unsigned int color, unsigned int *damaged_color)
{
	int	red;
	int	green;
	int	blue;

	red = (color >> 16) & 0xFF;
	green = (color >> 8) & 0xFF;
	blue = color & 0xFF;
	red = (int)(red * 0.7);
	green = (int)(green * 0.7);
	blue = (int)(blue * 0.7);
	*damaged_color = (red << 16) | (green << 8) | blue;
}
