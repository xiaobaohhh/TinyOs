/*_BITMAP_H_
 * @Author: xiaobao xiaobaogenji@163.com
 * @Date: 2025-06-03 12:48:56
 * @LastEditors: xiaobao xiaobaogenji@163.com
 * @LastEditTime: 2025-06-03 12:49:04
 * @FilePath: \start\source\kernel\include\tools\bitmap.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef BITMAP_H
#define BITMAP_H

#include "comm/types.h"
typedef struct _bitmap_t
{
    int bit_count;
    uint8_t *bits;
}bitmap_t;


int bitmap_byte_count(int bit_count);
void bitmap_init(bitmap_t *bitmap, uint8_t *bits, int count ,int init_bit);
int bitmap_get_bit(bitmap_t *bitmap, int index);
void bitmap_set_bit(bitmap_t *bitmap, int index, int count,int bit);
int bitmap_is_set(bitmap_t *bitmap, int index);
int bitmap_alloc_nbits(bitmap_t *bitmap, int bit, int count);


#endif
