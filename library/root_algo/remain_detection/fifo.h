#ifndef FIFO_H
#define FIFO_H

#if defined(__cplusplus)
extern "C" {
#endif

struct fifo {
    unsigned char *buffer;     /* the buffer holding the data */
    unsigned int size;         /* the size of the allocated buffer */
    unsigned int in;           /* data is added at offset (in % size) */
    unsigned int out;          /* data is extracted from off. (out % size) */
};

void fifo_init(struct fifo *k, unsigned char *buff, unsigned int len);
unsigned int fifo_put(struct fifo *k, unsigned char *buff, unsigned int len);
unsigned int fifo_get(struct fifo *k, unsigned char *buff, unsigned int len);
unsigned int fifo_peek(struct fifo *k, unsigned int len_want, unsigned char **buff1, unsigned char **buff2, unsigned int *len1, unsigned int *len2);
unsigned int fifo_peek_to_buf(struct fifo *fifo, unsigned char *buff, unsigned int len);
unsigned int fifo_len(struct fifo *fifo);

#if defined(__cplusplus)
}
#endif

#endif /* KFIFO_H */
