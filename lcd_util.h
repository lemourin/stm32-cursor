#ifndef LCD_UTIL_H
#define LCD_UTIL_H

#define CURSOR_SIZE 10

struct lcd_screen {
  int pos_x;
  int pos_y;
} lcd;

void initialize_lcd();
void lcd_move(int dx, int dy);
void lcd_update(int* current_x, int* current_y);

#endif  // LCD_UTIL_H
