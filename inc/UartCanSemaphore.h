

#ifndef __UARTCAN_SEMAPHORE_H__
#define __UARTCAN_SEMAPHORE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Implement semaphore with mutex and conditional variable.
 * Reason being, POSIX semaphore on Android are not used or
 * well tested.
 */

typedef struct {
    int val;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} uartcan_semaphore_t;

static inline void uartcan_semaphore_init(uartcan_semaphore_t *s, int n)
{
    pthread_mutex_init(&(s->mutex), NULL);
    pthread_cond_init(&(s->cond), NULL);
    s->val = n;
}

static inline void uartcan_semaphore_post(uartcan_semaphore_t *s)
{
    pthread_mutex_lock(&(s->mutex));
    s->val++;
    pthread_cond_signal(&(s->cond));
    pthread_mutex_unlock(&(s->mutex));
}

static inline int uartcan_semaphore_wait(uartcan_semaphore_t *s)
{
    int rc = 0;
    pthread_mutex_lock(&(s->mutex));
    while (s->val == 0)
        rc = pthread_cond_wait(&(s->cond), &(s->mutex));
    s->val--;
    pthread_mutex_unlock(&(s->mutex));
    return rc;
}

static inline void uartcan_semaphore_destroy(uartcan_semaphore_t *s)
{
    pthread_mutex_destroy(&(s->mutex));
    pthread_cond_destroy(&(s->cond));
    s->val = 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __QCAMERA_SEMAPHORE_H__ */
