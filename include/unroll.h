#ifndef LITES_UNROLL_H
#define LITES_UNROLL_H

/* Unroll a loop in chunks of 4 iterations. */
#define UNROLL4_LOOP(I, END, CODE)                \
    for (; (I) + 4 <= (END); (I) += 4) {           \
        CODE; CODE; CODE; CODE;                   \
    }                                             \
    for (; (I) < (END); ++(I)) {                   \
        CODE;                                     \
    }

#endif /* LITES_UNROLL_H */
