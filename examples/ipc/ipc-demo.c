/**
 * Minimal demo exercising capability backed POSIX IPC wrappers.
 */
#include "posix_ipc.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    cap_mq_t *mq = cap_mq_open("/ipc_demo_mq", O_CREAT | O_RDWR, 0600, 4);
    if (!mq) {
        perror("cap_mq_open");
        return 1;
    }
    cap_sem_t *sem = cap_sem_open("/ipc_demo_sem", O_CREAT | O_RDWR, 0600, 0);
    if (!sem) {
        perror("cap_sem_open");
        cap_mq_close(mq);
        return 1;
    }
    cap_shm_t *shm = cap_shm_open("/ipc_demo_shm", O_CREAT | O_RDWR, 0600, 4096);
    if (!shm) {
        perror("cap_shm_open");
        cap_sem_close(sem);
        cap_mq_close(mq);
        return 1;
    }
    void *addr = cap_shm_map(shm, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, 0);
    if (addr == MAP_FAILED) {
        perror("cap_shm_map");
        cap_shm_close(shm);
        cap_sem_close(sem);
        cap_mq_close(mq);
        return 1;
    }

    const char *text = "hello";
    if (cap_mq_send(mq, text, strlen(text) + 1, 0) < 0) {
        perror("cap_mq_send");
    }
    char buf[32];
    if (cap_mq_receive(mq, buf, sizeof(buf), NULL) >= 0)
        printf("mq message: %s\n", buf);

    strcpy((char *)addr, "shared memory");
    cap_sem_post(sem);
    cap_sem_wait(sem);
    printf("shm contains: %s\n", (char *)addr);

    cap_shm_unmap(addr, 4096);
    cap_shm_close(shm);
    cap_sem_close(sem);
    cap_mq_close(mq);
    return 0;
}
