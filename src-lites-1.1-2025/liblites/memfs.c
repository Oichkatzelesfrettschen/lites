#include "../../include/dirent.h"

#include <stdlib.h>
#include <string.h>

struct memfs_node {
    const char *name;
    unsigned char type;
    const struct memfs_node *children;
    size_t child_count;
};

static const struct memfs_node sub_nodes[] = {
    {"nested.txt", DT_REG, NULL, 0},
};

static const struct memfs_node root_nodes[] = {
    {"hello.txt", DT_REG, NULL, 0},
    {"sub", DT_DIR, sub_nodes, 1},
};

static const struct memfs_node root_node = {"/", DT_DIR, root_nodes, 2};

DIR *opendir(const char *path) {
    if (!path || strcmp(path, "/") != 0) {
        return NULL;
    }
    DIR *d = malloc(sizeof(DIR));
    if (!d) {
        return NULL;
    }
    d->node = &root_node;
    d->index = 0;
    return d;
}

static struct dirent d_buf;

struct dirent *readdir(DIR *d) {
    if (!d || d->index >= d->node->child_count) {
        return NULL;
    }
    const struct memfs_node *ent = &d->node->children[d->index++];
    d_buf.d_fileno = d->index;
    d_buf.d_reclen = sizeof(struct dirent);
    d_buf.d_type = ent->type;
    d_buf.d_namlen = strlen(ent->name);
    strncpy(d_buf.d_name, ent->name, sizeof(d_buf.d_name) - 1);
    d_buf.d_name[sizeof(d_buf.d_name) - 1] = '\0';
    return &d_buf;
}

void rewinddir(DIR *d) {
    if (d) {
        d->index = 0;
    }
}

int closedir(DIR *d) {
    if (!d) {
        return -1;
    }
    free(d);
    return 0;
}
