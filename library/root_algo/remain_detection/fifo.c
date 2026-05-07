#include <stdio.h>
#include <string.h>
#include "fifo.h"
#include "typedef.h"

volatile uint8_t fifo_operate_flag = 0;

void fifo_init(struct fifo *fifo, unsigned char *buffer, unsigned int len)
{
    fifo->size = len;
    fifo->buffer = buffer;
    fifo->in = fifo->out = 0;
}
unsigned int fifo_put(struct fifo *fifo, unsigned char *buffer, unsigned int len)
{
    unsigned int l;

    if(fifo_operate_flag == 1)
    {
        return 0;
    }
    fifo_operate_flag = 1;
    len = MIN(len, fifo->size - fifo->in + fifo->out);

    l = MIN(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
    memcpy(fifo->buffer, buffer + l, len - l);
    fifo->in += len;
    fifo_operate_flag = 0;

    return len;
}
unsigned int fifo_get(struct fifo *fifo, unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    if(fifo_operate_flag == 1)
    {
        return 0;
    }

    fifo_operate_flag = 1;
    len = MIN(len, fifo->in - fifo->out);

    l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    memcpy(buffer + l, fifo->buffer, len - l);
    fifo->out += len;
    fifo_operate_flag = 0;
    return len;
}
unsigned int fifo_peek(struct fifo *fifo, unsigned int len_want, unsigned char **buff1, unsigned char **buff2, unsigned int *len1, unsigned int *len2)
{
    unsigned int l,len;

    *buff1 = *buff2 = NULL;
    *len1 = *len2 = 0;
    len = MIN(len_want, fifo->in - fifo->out);
    if (len < len_want) {
        return 0;
    }

    l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));
    *buff1 = fifo->buffer + (fifo->out & (fifo->size - 1));
    *len1 = l;
    if (l < len) {
        *buff2 = fifo->buffer;
        *len2 = len-l;
    }

    return len_want;
}

unsigned int fifo_peek_to_buf(struct fifo *fifo, unsigned char *buff, unsigned int len)
{
    unsigned char *buf1 = NULL, *buf2 = NULL;
    unsigned int len1 = 0, len2 = 0;

    fifo_peek(fifo, len, &buf1, &buf2, &len1, &len2);

    if (len == (len1 + len2)) {
        memcpy(buff, buf1, len1);
        memcpy(buff + len1, buf2, len2);
    } else {
        return 0;
    }

    return len;
}

unsigned int fifo_len(struct fifo *fifo)
{
    return (fifo->in-fifo->out);
}

#if 0
struct fifo test_fifo;
unsigned char fifo_buffer[32];
void fifo_test(void)
{
    unsigned char b[10];
    fifo_init(&test_fifo, fifo_buffer, 32);
    TRACE(1,"init 32 : fifo size %d", test_fifo.size);

    fifo_put(&test_fifo, "1234567890", 10);
    TRACE(1,"put 10 : fifo  len %d", fifo_len(&test_fifo));

    fifo_put(&test_fifo, "abcdefghij", 10);
    TRACE(1,"put 10 : fifo  len %d", fifo_len(&test_fifo));

    fifo_put(&test_fifo, "!@#$%^&*()", 10);
    TRACE(1,"put 10 : fifo  len %d", fifo_len(&test_fifo));

    fifo_put(&test_fifo, "VVVV", 4);
    TRACE(1,"put 4 : fifo  len %d", fifo_len(&test_fifo));

    fifo_get(&test_fifo, b, 2);
    TRACE(1,"get 2 : fifo  len %d", fifo_len(&test_fifo));
    TRACE(0,"data:");
    for(int i = 0; i < 2; ++i) {
        TRACE(1,"%c-", b[i]);
    }
    TRACE(0," ");
    fifo_get(&test_fifo, b, 10);
    TRACE(1,"get 10 : fifo  len %d", fifo_len(&test_fifo));
    TRACE(0,"data:");
    for(int i = 0; i < 10; ++i) {
        TRACE(1,"%c-", b[i]);
    }
    TRACE(0," ");
    fifo_get(&test_fifo, b, 10);
    TRACE(1,"get 10 : fifo  len %d", fifo_len(&test_fifo));
    TRACE(0,"data:");
    for(int i = 0; i < 10; ++i) {
        TRACE(1,"%c-", b[i]);
    }
    TRACE(0," ");
    fifo_get(&test_fifo, b, 10);
    TRACE(1,"get 10 : fifo  len %d", fifo_len(&test_fifo));
    TRACE(0,"data:");
    for(int i = 0; i < 10; ++i) {
        TRACE(1,"%c-", b[i]);
    }
    TRACE(0," ");
}
#endif
