#include "ringbuffer.h"
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_rngbuf_contents_size(void)
{
    struct rngbuf *rb;
    assert(rngbuf_new(0) == NULL);
    rb = rngbuf_new(1);
    assert(rngbuf_available_capacity(rb) == 1);
    assert(rngbuf_contents_size(rb) == 0);
    char a = 'a', b = 'b';
    assert(rngbuf_push_copy(rb,&a,1) == 0);
    assert(rngbuf_contents_size(rb) == 1);
    assert(rngbuf_available_capacity(rb) == 0);
    assert(rngbuf_shift_in(rb,&b,1) == 0);
    assert(rngbuf_contents_size(rb) == 1);
    assert(rngbuf_available_capacity(rb) == 0);
    rngbuf_free(rb);
    printf("test_rngbuf_contents_size passed\n");
}

void test_rngbuf_get_slice(void)
{
    struct rngbuf *rb;
    struct rngbuf_slice rbs;
    rb = rngbuf_new(10);
    assert(rngbuf_available_capacity(rb) == 15);
    assert(rngbuf_contents_size(rb) == 0);
    char buf[] = "0123456789";
    assert(rngbuf_shift_in(rb,buf,strlen(buf)) == -1);
    assert(rngbuf_push_copy(rb,buf,strlen(buf)) == 0);
    assert(rngbuf_contents_size(rb) == 10);
    assert(rngbuf_available_capacity(rb) == 5);
    assert(rngbuf_get_slice(rb,&rbs,10,10) == -1);
    assert(rngbuf_get_slice(rb,&rbs,1,10) == -2);
    assert(rngbuf_get_slice(rb,&rbs,2,8) == 0);
    assert(rbs.second_region == NULL);
    assert(strncmp(rbs.first_region,"23456789",rbs.first_region_size) == 0);
    assert(rngbuf_advance_head(rb,7) == 0);
    assert(rngbuf_contents_size(rb) == 3);
    assert(rngbuf_push_copy(rb,buf,strlen(buf)) == 0);
    assert(rngbuf_get_slice(rb,&rbs,1,13) == -2);
    assert(rngbuf_get_slice(rb,&rbs,2,10) == 0);
    assert(strncmp(rbs.first_region,"9012345",rbs.first_region_size) == 0);
    assert(strncmp(rbs.second_region,"678",rbs.second_region_size) == 0);
    assert(rngbuf_get_slice(rb,&rbs,9,4) == 0);
    assert(strncmp(rbs.first_region,"6789",rbs.first_region_size) == 0);
    assert(rbs.second_region == NULL);
    assert(rngbuf_shift_in(rb,buf,strlen(buf)) == 0);
    assert(rngbuf_contents_size(rb) == 13);
    assert(rngbuf_get_slice(rb,&rbs,2,10) == 0);
    assert(strncmp(rbs.first_region,"90123456789",rbs.first_region_size) == 0);
    assert(rbs.second_region == NULL);
    rngbuf_free(rb);
    printf("test_rngbuf_get_slice passed\n");
}

int main (void)
{
    test_rngbuf_contents_size();
    test_rngbuf_get_slice();
    return 0;
}

