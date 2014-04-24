#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>
#include <GL/glew.h>

typedef struct
{
    double x;
    double y;
    double z;
} point;

void normalize(point *a);
void cross(point *a, point *b, point *result);

void reset_to_identity(GLfloat *matrix);
void add_matrix_to_stack(GLfloat *matrix_stack, GLfloat *matrix, GLfloat *tmp_matrix);

void generate_projection_matrix(GLfloat *matrix, double fovx, double aspect, double nearDist, double farDist);
void generate_view_matrix(GLfloat *matrix, point *cam_dir, point *up);
void generate_translation_matrix(GLfloat *matrix, point *cam_pos);

#endif
