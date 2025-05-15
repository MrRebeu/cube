/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cube3d.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abkhefif <abkhefif@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 21:50:29 by tcaccava          #+#    #+#             */
/*   Updated: 2025/05/15 16:52:01 by abkhefif         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUBE3D_H
# define CUBE3D_H

# include "../gnl/get_next_line.h"
# include "mlx.h"
# include <fcntl.h>
# include <math.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# ifndef M_PI
# define M_PI 3.14159265358979323846
# endif

# define W 119
# define A 97
# define S 115
# define D 100

# define R 114

# define Q 113
# define E 101

# define TILE_SIZE 64
# define DISPLAY_WIDTH 1920
# define DISPLAY_HEIGHT 1080
# define FOV (M_PI / 3)

typedef struct s_player
{
	double x;       // Position X du joueur
	double y;       // Position Y du joueur
	double angle;   // Angle de vision du joueur
	double fov;     // Champ de vision (Field of View)
	int move_speed; // Vitesse de déplacement
	int rot_speed;  // Vitesse de rotation

	bool			key_down;
	bool			key_up;
	bool			key_right;
	bool			key_left;
	bool			left_rotate;
	bool			right_rotate;
	bool			turn_back;
}					t_player;

typedef struct s_intersect
{
	double			x;
	double			y;
	double			step_x;
	double			step_y;
}					t_intersect;

typedef struct s_img
{
	void			*ptr;
	char			*addr;
	int				bits_per_pixel;
	int				line_length;
	int				endian;
	int				width;
	int				height;
}					t_img;

typedef struct s_env
{
	void			*mlx;
	void			*win;
	t_img			img;
}					t_env;

typedef struct s_map
{
	char **matrix; // Données de la carte
	int width;     // Largeur de la carte en cellules
	int height;    // Hauteur de la carte en cellules
	t_img			floor_texture;
	t_img			wall_texture;
	t_img			door_texture;
	t_img arm_1;  // Image de l'arme
	int x_player; // Position X du joueur
	int y_player; // Position Y du joueur
}					t_map;

typedef struct s_ray
{
	double			radiant_angle;
	double			player_angle;
	double			distance;
    double wall_hit_x;
	double wall_hit_y;
	int hit_vertical;
	char hit_type;
}					t_ray;

typedef struct s_game
{
	void *mlx;       // Pointeur MLX
	void *win;       // Pointeur vers la fenêtre
	t_img screen;    // Image de l'écran principal
	t_map map;       // Données de la carte
	t_player player; // Données du joueur
	t_img			weapons[2];
	// Tableau pour les armes (vous pouvez ajouter plus d'armes)
	int current_weapon;        // Indice de l'arme actuellement équipée
	t_ray rays[DISPLAY_WIDTH]; // Résultats du raycasting pour chaque colonne
}					t_game;

typedef struct s_render
{
	// Dimensions du mur
	double			corrected_dist;
	int				wall_height;
	int				door_height;
	int				draw_start;
	int				draw_end;

	// Variables d'itération
	int				x;
	int				y;

	// Texture
	int				tex_x;
	int				tex_y;
	char			*tex_addr;

	// Sol
	double			row_distance;
	double			floor_x;
	double			floor_y;
	double			dim_factor;

	// Couleur
	unsigned int	color;
	unsigned int	r;
	unsigned int	g;
	unsigned int	b;

	// Pixel
	char			*screen_pixel;
    int texture_offset_y;
}					t_render;


typedef struct s_sprite
{
    double x;           // Position X du sprite dans le monde
    double y;           // Position Y du sprite dans le monde
    int texture_id;     // ID de la texture à utiliser
    double distance;    // Distance au joueur (calculée dynamiquement)
    double angle;       // Angle par rapport au joueur (calculé dynamiquement)
    double size;        // Taille à l'écran (calculée dynamiquement)
} t_sprite;


t_intersect			v_intersection(int x_player, int y_player,
						double radiant_angle);

t_intersect			h_intersection(int x_player, int y_player,
						double radiant_angle);

int					is_not_wall(t_map *map, double x, double y);
int					calc_wall_height(double corrected_dist);
double				no_fish_eye(double min_distance, double radiant_angle,
						double player_angle);

void				render_column(t_game *game, int column_x, t_ray *ray);
void				render_frame(t_game *game);
double				ray_casting(t_game *game, double radiant_angle, int column_x);

int					set_player_pos(t_game *game);
int					read_map(char *file_path, t_game *game);
int					init_game(t_game *game, char *map_file);

int					check_file_cub(char *file_path);
int					validate_map(t_map *map);

int					init_game(t_game *game, char *map_file);

// render
void render_wall(t_game *game, int column_x, t_render *r, t_ray *ray);
void				render_floor(t_game *game, int column_x, t_render *r,
						t_ray *ray);
void				render_sky(t_game *game, int column_x, t_render *r);
void				render_weapon(t_game *game);
void				move_player(t_player *player);
void				init_player(t_player *player);
int					key_press(int keycode, t_player *player);
int					key_release(int keycode, t_player *player);

int					render_next_frame(t_game *game);
void				render_scene(t_game *game);
void				render_ui(t_game *game);
int					loop_game(t_game *game);
void render_door(t_game *game, int column_x, t_render *r, t_ray *ray);
double				normalize_angle(double angle);

#endif
