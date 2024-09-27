#ifndef BALL_H
#define BALL_H

struct ball_s {
	int x;
	int y;
	short radius;
	short color;
};

void draw_ball_3D(struct ball_s *p_ball);
void draw_all_ball_3D(struct ball_s *p_ball, int nb_ball);
#endif
