#include "../cube3d.h"

#include "../cube3d.h"

void	handle_shoot_cooldown(t_enemy *e, t_player *p)
{
	if (e->cooldown <= 0)
	{
		p->health -= 10;
		e->cooldown = 180;
	}
	else
		e->cooldown--;
}

void	handle_melee_cooldown(t_enemy *e, t_player *p)
{
	if (e->cooldown <= 0)
	{
		p->health -= 25;
		e->cooldown = 90;
	}
	else
		e->cooldown--;
}
