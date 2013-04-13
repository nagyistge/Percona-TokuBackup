// Test to make sure that if the destination isn't empty, then the backup fails.  For

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "backup_test_helpers.h"

static char *src;
static char *dst;

const int expect_result = EINVAL; // 
int   error_count=0;
bool  ok=true;

static void expect_error(int error_number, const char *error_string, void *error_extra) {
    if (error_count!=0)              { ok=false; fprintf(stderr, "%s:%d error function called twice\n", __FILE__, __LINE__); }
    error_count++;
    if (error_number!=expect_result) { ok=false; fprintf(stderr, "%s:%d error_number=%d expected %d\n", __FILE__, __LINE__, error_number, expect_result); }
    if (error_string==NULL)          { ok=false; fprintf(stderr, "%s:%d expect error_string nonnull\n", __FILE__, __LINE__); }
    printf("error_string (expected)=%s\n", error_string);
    if (error_extra!=NULL)           { ok=false; fprintf(stderr, "%s:%d expect error_extra NULL\n", __FILE__, __LINE__); }
}

int test_main(int argc __attribute__((__unused__)), const char *argv[] __attribute__((__unused__))) {
    src = get_src();
    dst = get_dst();
    setup_source();
    setup_destination();
    setup_dirs();

    {
        int r = systemf("echo hello > %s/a_file_is_there_but_shouldnt_be", dst);
        assert(r!=-1);
        assert(WIFEXITED(r));
        assert(WEXITSTATUS(r)==0);
    }

    pthread_t thread;
    start_backup_thread_with_funs(&thread,
                                  strdup(src), strdup(dst), // free's src and dst
                                  simple_poll_fun, NULL,
                                  expect_error, NULL,
                                  expect_result);
    finish_backup_thread(thread);
    free(src);
    free(dst);
    return 0;
}