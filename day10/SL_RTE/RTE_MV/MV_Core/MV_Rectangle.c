#include "MV_Rectangle.h"
#include "RTE_MEM.h"
#include <string.h>
/////////////////////
// Rectangle Stuff //
/////////////////////

void rectangle_init(rectangle_t *ptr, int x, int y, int w, int h)
{
    ptr->x = x;
    ptr->y = y;
    ptr->w = w;
    ptr->h = h;
}

void rectangle_copy(rectangle_t *dst, rectangle_t *src)
{
    memcpy(dst, src, sizeof(rectangle_t));
}

bool rectangle_equal_fast(rectangle_t *ptr0, rectangle_t *ptr1)
{
    return !memcmp(ptr0, ptr1, sizeof(rectangle_t));
}

bool rectangle_overlap(rectangle_t *ptr0, rectangle_t *ptr1)
{
    int x0 = ptr0->x;
    int y0 = ptr0->y;
    int w0 = ptr0->w;
    int h0 = ptr0->h;
    int x1 = ptr1->x;
    int y1 = ptr1->y;
    int w1 = ptr1->w;
    int h1 = ptr1->h;
    return (x0 < (x1 + w1)) && (y0 < (y1 + h1)) && (x1 < (x0 + w0)) && (y1 < (y0 + h0));
}

void rectangle_intersected(rectangle_t *dst, rectangle_t *src)
{
    int leftX = MATH_MAX(dst->x, src->x);
    int topY = MATH_MAX(dst->y, src->y);
    int rightX = MATH_MIN(dst->x + dst->w, src->x + src->w);
    int bottomY = MATH_MIN(dst->y + dst->h, src->y + src->h);
    dst->x = leftX;
    dst->y = topY;
    dst->w = rightX - leftX;
    dst->h = bottomY - topY;
}

void rectangle_united(rectangle_t *dst, rectangle_t *src)
{
    int leftX = MATH_MIN(dst->x, src->x);
    int topY = MATH_MIN(dst->y, src->y);
    int rightX = MATH_MAX(dst->x + dst->w, src->x + src->w);
    int bottomY = MATH_MAX(dst->y + dst->h, src->y + src->h);
    dst->x = leftX;
    dst->y = topY;
    dst->w = rightX - leftX;
    dst->h = bottomY - topY;
}


rectangle_t *rectangle_alloc(int16_t x, int16_t y, int16_t w, int16_t h)
{
    rectangle_t *r = RTE_MEM_Alloc0(MEM_DTCM, sizeof(rectangle_t));
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    return r;
}

bool rectangle_equal(rectangle_t *r1, rectangle_t *r2)
{
    return ((r1->x==r2->x)&&(r1->y==r2->y)&&(r1->w==r2->w)&&(r1->h==r2->h));
}

bool rectangle_intersects(rectangle_t *r1, rectangle_t *r2)
{
    return  ((r1->x < (r2->x+r2->w)) &&
             (r1->y < (r2->y+r2->h)) &&
             ((r1->x+r1->w) > r2->x) &&
             ((r1->y+r1->h) > r2->y));
}

// Determine subimg even if it is going off the edge of the main image.
bool rectangle_subimg(image_t *img, rectangle_t *r, rectangle_t *r_out)
{
    rectangle_t r_img;
    r_img.x = 0;
    r_img.y = 0;
    r_img.w = img->w;
    r_img.h = img->h;
    bool result = rectangle_intersects(&r_img, r);
    if (result) {
        int r_img_x2 = r_img.x + r_img.w;
        int r_img_y2 = r_img.y + r_img.h;
        int r_x2 = r->x + r->w;
        int r_y2 = r->y + r->h;
        r_out->x = MATH_MAX(r_img.x, r->x);
        r_out->y = MATH_MAX(r_img.y, r->y);
        r_out->w = MATH_MIN(r_img_x2, r_x2) - r_out->x;
        r_out->h = MATH_MIN(r_img_y2, r_y2) - r_out->y;
    }
    return result;
}

// This isn't for actually combining the rects standardly, but, to instead
// find the average rectangle between a bunch of overlapping rectangles.
static void rectangle_add(rectangle_t *r1, rectangle_t *r2)
{
    r1->x += r2->x;
    r1->y += r2->y;
    r1->w += r2->w;
    r1->h += r2->h;
}

// This isn't for actually combining the rects standardly, but, to instead
// find the average rectangle between a bunch of overlapping rectangles.
static void rectangle_div(rectangle_t *r, int c)
{
    r->x /= c;
    r->y /= c;
    r->w /= c;
    r->h /= c;
}

array_t *rectangle_merge(array_t *rectangles)
{
    array_t *objects; array_alloc(&objects, MEM_DTCM);
    array_t *overlap; array_alloc(&overlap, MEM_DTCM);
    /* merge overlaping detections */
    while (array_length(rectangles)) {
        /* check for overlaping detections */
        rectangle_t *rect = (rectangle_t *) array_take(rectangles, 0);
        for (int j=0; j<array_length(rectangles); j++) { // do not cache bound
            if (rectangle_intersects(rect, (rectangle_t *) array_at(rectangles, j))) {
                array_push_back(overlap, array_take(rectangles, j--));
            }
        }
        /* add the overlaping detections */
        int count = array_length(overlap);
        for (int i=0; i<count; i++) {
            rectangle_t *overlap_rect = (rectangle_t *) array_pop_back(overlap);
            rectangle_add(rect, overlap_rect);
            RTE_MEM_Free(MEM_DTCM, overlap_rect);
        }
        /* average the overlaping detections */
        rectangle_div(rect, count + 1);
        array_push_back(objects, rect);
    }
    array_free(rectangles);
    array_free(overlap);
    return objects;
}

// Expands a bounding box with a point.
// After adding all points sub x from w and y from h.
void rectangle_expand(rectangle_t *r, int x, int y)
{
    if (x < r->x) {
        r->x = x;
    }
    if (y < r->y) {
        r->y = y;
    }
    if (x > r->w) {
        r->w = x;
    }
    if (y > r->h) {
        r->h = y;
    }
}
