#include "../../include/dirent.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    DIR *d = opendir("/");
    if (!d) {
        perror("opendir");
        return 1;
    }
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        printf("%s\n", ent->d_name);
    }
    closedir(d);
    return 0;
}
