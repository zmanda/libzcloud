#include <stdio.h>
#include "zcloud/zcloud.h"

int main()
{
    GError *error = NULL;
    if (!zcloud_init(&error)) {
        fprintf(stderr, "Could not initialize libzcloud: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    if (!zcloud_load_plugin("disk", &error)) {
        fprintf(stderr, "Could not load plugin 'disk': %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    fprintf(stderr, "SUCCESS\n");
}
