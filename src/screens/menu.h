#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "assets.h"
#include "board.h"

#define MENU_ITEMS 4
#define MENU_ITEM_CONTINUE 0
#define MENU_ITEM_LEVEL_SELECT 1
#define MENU_ITEM_SETTINGS 2
#define MENU_ITEM_QUIT 3

#define MENU_ITEM_CREDITS 4

typedef struct MenuLayout {
    Rectangle title;
    Rectangle panel;
    Rectangle button[MENU_ITEMS];
    Rectangle tag;
    Rectangle credits;
} MenuLayout;

MenuLayout MenuGetLayout(Assets *asset);

int MenuHitTest(const MenuLayout *layout, Vector2 mouse);
void MenuDraw(const Board *board, Assets *asset, const MenuLayout *layout,
              int hovered);

#endif
