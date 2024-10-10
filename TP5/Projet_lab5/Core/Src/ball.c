#include "ball.h"
#include "math.h"
#include "ili9341_gfx.h"

extern ili9341_t *_screen;

void draw_ball_3D(struct ball_s *p_ball) {
   int x = p_ball->x;
   int y = p_ball->y;
   int radius = p_ball->radius;
   int lsb_rgb = (p_ball->color & 0xFFDF)/31;

   int dx = 0;
   int dy = radius;
   int d2z = 0;
   int dz = 0;
   int color = p_ball->color;
   
   while (dx <= dy) {
      int back_d2z = d2z;
      int back_dy = dy;
      while (dx <= dy) {
         dz = (sqrt(d2z)*31)/radius;
         
         if (dz > 5) {
             color = lsb_rgb * dz;
                
             int xhi = x + dx;
             int xlo = x - dx;
             int yhi = y + dy;
             int ylo = y - dy;
               
             ili9341_draw_pixel(_screen,color,xhi,yhi);
             ili9341_draw_pixel(_screen,color,xhi,ylo);
             ili9341_draw_pixel(_screen,color,xlo,yhi);
             ili9341_draw_pixel(_screen,color,xlo,ylo);
               
             xhi = x + dy;
             xlo = x - dy;
             yhi = y + dx;
             ylo = y - dx;
               
             ili9341_draw_pixel(_screen,color,xhi,yhi);
             ili9341_draw_pixel(_screen,color,xhi,ylo);
             ili9341_draw_pixel(_screen,color,xlo,yhi);
             ili9341_draw_pixel(_screen,color,xlo,ylo);
         }         
         d2z += (dy+dy-1);
         dy--;
      }
      d2z = back_d2z;
      dy = back_dy;
      dx++; d2z -= (dx+dx-1);
      if (d2z < 0) {
         dy--; d2z += (dy+dy+1);
      }
   }    
}

void draw_all_ball_3D(struct ball_s *p_ball, int nb_ball) {
	 for (int i=0; i<nb_ball; i++) {
		 if (p_ball[i].radius>0) draw_ball_3D(&(p_ball[i]));
	 }
}
