#ifndef USERSPACE_PERSONALITY_H
#define USERSPACE_PERSONALITY_H

struct userspace_ops {
    int (*init)(void);
    int (*exec)(const char *path, char *const argv[]);
    void (*exit)(int code);
};

#endif /* USERSPACE_PERSONALITY_H */
