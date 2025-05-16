NAME = cube3d
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I$(MLX_DIR) -I./
MLX_DIR = ./minilibx-linux
LDFLAGS = -L$(MLX_DIR) -lmlx_Linux -lXext -lX11 -lm

CORE_SRC = core/main.c core/game_loop.c core/init_game.c
RAYCASTER_SRC = raycaster/raycasting.c raycaster/ray_utils.c
MAP_SRC = map/map_parser.c
PLAYER_SRC = player/player_move.c
PNJ_SRC = pnj/pnj.c
RENDER_SRC = render/render.c render/render_env.c render/render_ui.c
GNL_SRC = gnl/get_next_line.c gnl/get_next_line_utils.c

SRCS = $(CORE_SRC) $(RAYCASTER_SRC) $(MAP_SRC) $(PLAYER_SRC) $(RENDER_SRC) $(GNL_SRC) $(PNJ_SRC)

OBJS = $(SRCS:.c=.o)
RM = rm -f

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(NAME)

re: fclean all
