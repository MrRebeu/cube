#include "cube3d.h"

void render_healgun_animation(t_game *game)
{
    // ✅ AFFICHER LE BRAS GAUCHE avec la piqûre
    t_img *arm_sprite = &game->map.arm_1; // Ton sprite de bras gauche
    
    // Position du bras à gauche de l'écran
    int arm_x = 50;
    int arm_y = DISPLAY_HEIGHT - arm_sprite->height - 50;
    
    // Dessiner le bras
    for (int y = 0; y < arm_sprite->height; y++)
    {
        for (int x = 0; x < arm_sprite->width; x++)
        {
            if (arm_x + x >= 0 && arm_x + x < DISPLAY_WIDTH && 
                arm_y + y >= 0 && arm_y + y < DISPLAY_HEIGHT)
            {
                char *src = arm_sprite->addr + (y * arm_sprite->line_length + 
                           x * (arm_sprite->bits_per_pixel / 8));
                unsigned int color = *(unsigned int *)src;
                
                // Skip transparence
                if (!(((color >> 16) & 0xFF) < 10 && ((color >> 8) & 0xFF) < 10 && (color & 0xFF) < 10))
                {
                    char *dst = game->screen.addr + ((arm_y + y) * game->screen.line_length + 
                               (arm_x + x) * (game->screen.bits_per_pixel / 8));
                    *(unsigned int *)dst = color;
                }
            }
        }
    }
}

void use_healgun(t_game *game)
{
    start_healgun_animation(game); // ✅ DÉMARRER L'ANIMATION

    // ✅ CONSOMMER LA MUNITION
    game->player.healgun_ammo--;
    game->player.healgun_is_loaded = 0;
    
    // ✅ SOIGNER LE JOUEUR
    game->player.health = 100; 
    if (game->player.health > 100)
        game->player.health = 100;
    
    game->player.healgun_animation = 1;
    game->player.healgun_anim_timer = 60; // 1 seconde à 60 FPS
    
    printf("💉 Heal Gun utilisé ! Santé: %d, Munitions: %d\n", 
           game->player.health, game->player.healgun_ammo);
    
    // ✅ RECHARGER AUTOMATIQUEMENT SI ON A D'AUTRES MUNITIONS
    if (game->player.healgun_ammo > 0)
    {
        game->player.healgun_is_loaded = 1;
        printf("🔋 Heal Gun rechargé automatiquement !\n");
    }
}
int load_healgun(t_game *game)
{
    const char *healgun_sprites[5] = { // ← Bien 5 sprites
        "./texture/healgun.xpm",
        "./texture/healgun_1.xpm", 
        "./texture/healgun_2.xpm",
        "./texture/healgun_3.xpm",
        "./texture/healgun_4.xpm"
    };
    
    for (int i = 0; i < 5; i++) // ← Bien 5 iterations
    {
        if (!load_single_weapon_texture(game->mlx, &game->healgun_frames[i], 
                                       (char *)healgun_sprites[i]))
        {
            printf("Erreur: %s non trouvé\n", healgun_sprites[i]);
            return (0);
        }
        printf("✅ Chargé: %s\n", healgun_sprites[i]); // Debug
    }
    return (1);
}

void update_healgun_animation(t_game *game)
{
    if (!game->player.healgun_animating)
        return;
        
    game->player.healgun_anim_timer--;
    
    if (game->player.healgun_anim_timer <= 0)
    {
        game->player.healgun_anim_frame++; // ← CORRIGER : utiliser anim_frame au lieu de anim_state
        
        // Durées différentes pour chaque frame
        int frame_durations[5] = {10, 15, 20, 15, 10}; // En frames
        
        if (game->player.healgun_anim_frame >= 5)
        {
            // Animation terminée
            game->player.healgun_animating = 0;
            game->player.healgun_anim_frame = 0; // ← CORRIGER : remettre anim_frame à 0
            printf("✅ Animation Heal Gun terminée !\n");
        }
        else
        {
            game->player.healgun_anim_timer = frame_durations[game->player.healgun_anim_frame];
            printf("💉 Frame healgun: %d\n", game->player.healgun_anim_frame); // Debug
        }
    }
}
void start_healgun_animation(t_game *game)
{
    if (game->player.healgun_animating)
        return; // Déjà en cours
        
    // ✅ VÉRIFICATIONS
    if (!game->player.healgun_is_loaded || game->player.healgun_ammo <= 0)
    {
        printf("❌ Heal Gun vide !\n");
        return;
    }
    
    // ✅ DÉMARRER L'ANIMATION
    game->player.healgun_animating = 1;
    game->player.healgun_anim_frame = 0;
    game->player.healgun_anim_timer = 10; // Durée première frame
    game->player.healgun_heal_applied = 0;
    
    printf("💉 Animation Heal Gun démarrée !\n");
}
