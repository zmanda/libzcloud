#include <stdlib.h>
#include <strings.h>
#include <sys/times.h>
#include "s3.h"

int main() {
    static const size_t SIZE = 16777216;
    static const char *BUKKIT = "e7106b21ad371", *KEY = "e6615699f734d";
    char *access_key, *secret_key;
    CurlBuffer buf;
    struct tms start_data, end_data;
    clock_t start_time, end_time;
    long ticks_per_sec;
    S3Handle *s3;

    ticks_per_sec = sysconf(_SC_CLK_TCK);

    s3_init();
    s3 = s3_open(access_key, secret_key, NULL, NULL);

    buf.buffer = malloc(SIZE);
    buf.buffer_len = SIZE;
    buf.buffer_pos = 0;
    if (!buf) {return 1;}
    bzero(buf, SIZE);

    s3_upload(s3, BUKKIT, KEY, s3_buffer_read_func, s3_buffer_size_func, NULL, &buf, NULL, NULL);

    s3_use_ssl(s3, FALSE);

    s3_use_ssl(s3, TRUE);

    s3_free(s3);
}
