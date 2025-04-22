#include <cglm/struct.h>
#include <cglm/mat4.h>
#include <cglm/quat.h>

#include "SAC/cglm/misc.h"

void cglm_well_priming_test() {

  // Simple Test for CGLM
  printf("SETUP: Simple Test for CGLM\n");
  mat4 matrix;
  glm_mat4_identity(matrix);
  vec4 vec;
  versor quat;
  glm_quat_identity(quat);
  mat4 rot;
  glm_quat_mat4(quat, rot);
  mat4 test;
  // t x r x m
  glm_mul(matrix, rot, test);

  // for (int i = 0; i < 4; i++) {
  //   for (int j = 0; j < 4; j++) {
  //     printf("%f\n", test[i][j]);
  //   }
  // }
}
