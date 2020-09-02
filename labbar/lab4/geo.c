#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

struct point {
  int x;
  int y;
};

typedef struct point point_t;

struct rectangle {
  point_t upper_left;
  point_t lower_right;
};

typedef struct rectangle rectangle_t;

void translate(point_t *p1, point_t *p2)
{
  p1->x += p2->x;
  p1->y += p2->y;
}

void print_point(point_t *point) {
  printf("point(%d,%d)", point->x, point->y);
}

void print_rect(rectangle_t *rect) {
  printf("rectangle(upper_left=");
  print_point(&rect->upper_left);
  printf(", lower_right=");
  print_point(&rect->lower_right);
  printf(")\n");
}

point_t make_point(int x, int y) {
  point_t point = { .x = x, .y = y };
  return point;
}

rectangle_t make_rect(int lx, int ly, int rx, int ry) {
  point_t upper_left = { .x = lx, .y = ly };
  point_t lower_right = { .x = rx, .y = ry };
  rectangle_t rect = { .upper_left = upper_left, .lower_right = lower_right };

  return rect;
}

int area_rect(rectangle_t *rect) {
  int width = rect->upper_left.x - rect->lower_right.x;
  int height = rect->upper_left.y - rect->lower_right.y;

  return abs(width * height);
}

bool intersects_rect(rectangle_t *rect1, rectangle_t *rect2) {
  return (
    (rect1->upper_left.x < rect2->lower_right.x || rect1->upper_left.y > rect2->lower_right.y) &&
    (rect2->upper_left.x < rect1->lower_right.x || rect2->upper_left.y > rect1->lower_right.y)
  );
}

rectangle_t intersection_rect(rectangle_t *rect1, rectangle_t *rect2) {
  rectangle_t intersection = {};

  if (!intersects_rect(rect1, rect2)) {
    return intersection; // defaults to an rectangle with ((0,0),(0,0))
  }

  if (rect1->upper_left.x <= rect2->upper_left.x) {
    intersection.upper_left.x = rect2->upper_left.x;
    intersection.lower_right.x = rect1->lower_right.x;
  } else {
    intersection.upper_left.x = rect1->upper_left.x;
    intersection.lower_right.x = rect2->lower_right.x;
  }

  if (rect1->upper_left.y <= rect2->upper_left.y) {
    intersection.upper_left.y = rect2->upper_left.y;
    intersection.lower_right.y = rect1->lower_right.y;
  } else {
    intersection.upper_left.y = rect1->upper_left.y;
    intersection.lower_right.y = rect2->lower_right.y;
  }

  return intersection;
}

int main(void) {
  rectangle_t rect1 = make_rect(0, 0, 100, 100);
  rectangle_t rect2 = make_rect(100, 100, 200, 200);
  rectangle_t rect3 = make_rect(50, 50, 101, 101);

  printf("intersects(rect1,rect2)=%d\n", intersects_rect(&rect1, &rect2));
  printf("intersects(rect1,rect3)=%d\n", intersects_rect(&rect1, &rect3));
  printf("intersects(rect2,rect3)=%d\n", intersects_rect(&rect2, &rect3));
  printf("intersects(rect1,rect1)=%d\n", intersects_rect(&rect1, &rect1));

  puts("");

  rectangle_t intersection;

  intersection = intersection_rect(&rect1, &rect3);
  printf("intersection(rect1,rect3)=\n");
  print_rect(&intersection);
  printf("\n");

  intersection = intersection_rect(&rect2, &rect3);
  printf("intersection(rect2,rect3)=\n");
  print_rect(&intersection);
  printf("\n");

  intersection = intersection_rect(&rect1, &rect1);
  printf("intersection(rect1,rect1)=\n");
  print_rect(&intersection);
  printf("\n");
}
