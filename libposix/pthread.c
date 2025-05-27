/* See LICENSE_MAP for license and origin information */
#include "pthread.h"
#include <mach/cthreads.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg)
{
    (void)attr;
    cthread_t t = cthread_fork((cthread_fn_t)start_routine, arg);
    if (!t)
        return -1;
    *thread = (pthread_t)t;
    return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
    void *r = cthread_join((cthread_t)thread);
    if (retval)
        *retval = r;
    return 0;
}

int pthread_detach(pthread_t thread)
{
    cthread_detach((cthread_t)thread);
    return 0;
}

pthread_t pthread_self(void)
{
    return (pthread_t)cthread_self();
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    (void)attr;
    spin_lock_init(mutex);
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    (void)mutex;
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    spin_lock(mutex);
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    spin_unlock(mutex);
    return 0;
}
