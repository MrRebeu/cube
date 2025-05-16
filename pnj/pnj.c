#include "../cube3d.h"

// void render_pnj(t_game *game)
// {
// //not finish
// }

// void	update_pnj(t_game *game)
// {
// 	double dx = game->pnj.x - game->player.x;
//     double dy = game->pnj.y - game->player.y;
//     game->pnj.distance = sqrt(dx*dx + dy*dy);
// }

// void	init_pnj(t_game *game)
// {
// 	game->pnj.x = 0;
// 	game->pnj.y = 0;
// 	game->pnj.distance = 0;
// 	game->pnj.active = 1;
// 	game->pnj->texture.ptr = mlx_xpm_file_to_image(game->mlx, "./texture/pnj.xpm", 
//                                                  &width, &height);
// 	if (!game->pnj.texture.ptr)
//     {
//         printf("Erreur: a mettre en anglais mais pas de sprite ennemi / pnj\n");
//         game->pnj.active = 0;
//         return;
//     }
//     game->pnj.texture.width = width;
//     game->pnj.texture.height = height;
//     game->pnj.texture.addr = mlx_get_data_addr(game->pnj.texture.ptr,
//                                              &game->pnj.texture.bits_per_pixel,
//                                              &game->pnj.texture.line_length,
//                                              &game->pnj.texture.endian);
// }
