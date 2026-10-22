#include <stdio.h>

#define SPRITE_SIZE 5


const char *zero[SPRITE_SIZE] = {
    "01110",
    "10001",
    "10001",
    "10001",
    "01110"
};

const char *one[SPRITE_SIZE] = {
    "00100",
    "01100",
    "00100",
    "00100",
    "01110"
};

const char *two[SPRITE_SIZE] = {
    "11110",
    "00001",
    "01110",
    "10000",
    "11111"
};

const char *three[SPRITE_SIZE] = {
    "11110",
    "00001",
    "01110",
    "00001",
    "11110"
};

const char *four[SPRITE_SIZE] = {
    "10010",
    "10010",
    "11111",
    "00010",
    "00010"
};

const char *five[SPRITE_SIZE] = {
    "11111",
    "10000",
    "11110",
    "00001",
    "11110"
};

const char *six[SPRITE_SIZE] = {
    "01110",
    "10000",
    "11110",
    "10001",
    "01110"
};

const char *seven[SPRITE_SIZE] = {
    "11111",
    "00001",
    "00010",
    "00100",
    "01000"
};

const char *eight[SPRITE_SIZE] = {
    "01110",
    "10001",
    "01110",
    "10001",
    "01110"
};

const char *nine[SPRITE_SIZE] = {
    "01110",
    "10001",
    "01111",
    "00001",
    "01110"
};

// capital letters // 
const char *a[SPRITE_SIZE] = {
    "01110",
    "10001",
    "11111",
    "10001",
    "10001"
};

const char *b[SPRITE_SIZE] = {
    "11110",
    "10001",
    "11110",
    "10001",
    "11110"
};

const char *c[SPRITE_SIZE] = {
    "01110",
    "10001",
    "10000",
    "10001",
    "01110"
};

const char *d[SPRITE_SIZE] = {
    "11100",
    "10010",
    "10001",
    "10010",
    "11100"
};

const char *e[SPRITE_SIZE] = {
    "11111",
    "10000",
    "11110",
    "10000",
    "11111"
};

const char *f[SPRITE_SIZE] = {
    "11111",
    "10000",
    "11110",
    "10000",
    "10000"
};

const char *g[SPRITE_SIZE] = {
    "01110",
    "10000",
    "10011",
    "10001",
    "01111"
};

const char *h[SPRITE_SIZE] = {
    "10001",
    "10001",
    "11111",
    "10001",
    "10001"
};

const char *i[SPRITE_SIZE] = {
    "11111",
    "00100",
    "00100",
    "00100",
    "11111"
};

const char *j[SPRITE_SIZE] = {
    "00111",
    "00010",
    "00010",
    "10010",
    "01100"
};

const char *k[SPRITE_SIZE] = {
    "10001",
    "10010",
    "11100",
    "10010",
    "10001"
};

const char *l[SPRITE_SIZE] = {
    "10000",
    "10000",
    "10000",
    "10000",
    "11111"
};

const char *m[SPRITE_SIZE] = {
    "10001",
    "11011",
    "10101",
    "10001",
    "10001"
};

const char *n[SPRITE_SIZE] = {
    "10001",
    "11001",
    "10101",
    "10011",
    "10001"
};

const char *o[SPRITE_SIZE] = {
    "01110",
    "10001",
    "10001",
    "10001",
    "01110"
};

const char *p[SPRITE_SIZE] = {
    "11110",
    "10001",
    "11110",
    "10000",
    "10000"
};

const char *q[SPRITE_SIZE] = {
    "01110",
    "10001",
    "10101",
    "01110",
    "00001"
};

const char *r[SPRITE_SIZE] = {
    "11110",
    "10001",
    "11110",
    "10010",
    "10001"
};

const char *s[SPRITE_SIZE] = {
    "01111",
    "10000",
    "01110",
    "00001",
    "11110"
};

const char *t[SPRITE_SIZE] = {
    "11111",
    "00100",
    "00100",
    "00100",
    "00100"
};

const char *u[SPRITE_SIZE] = {
    "10001",
    "10001",
    "10001",
    "10001",
    "01110"
};

const char *v[SPRITE_SIZE] = {
    "10001",
    "10001",
    "10001",
    "01010",
    "00100"
};

const char *w[SPRITE_SIZE] = {
    "10001",
    "10001",
    "10101",
    "11011",
    "10001"
};

const char *x[SPRITE_SIZE] = {
    "10001",
    "01010",
    "00100",
    "01010",
    "10001"
};

const char *y[SPRITE_SIZE] = {
    "10001",
    "01010",
    "00100",
    "00100",
    "00100"
};

const char *z[SPRITE_SIZE] = {
    "11111",
    "00010",
    "00100",
    "01000",
    "11111"
};


typedef struct {
    char character;
    const char **sprite;
} Character;

Character sprites[] = {
    {'A', a}, {'B', b}, {'C', c},
    {'D', d}, {'E', e}, {'F', f},
    {'G', g}, {'H', h}, {'I', i},
    {'J', j}, {'K', k}, {'L', l},
    {'M', m}, {'N', n}, {'O', o},
    {'P', p}, {'Q', q}, {'R', r},
    {'S', s}, {'T', t}, {'U', u},
    {'V', v}, {'W', w}, {'X', x},
    {'Y', y}, {'Z', z},

    {'0', zero}, {'1', one}, {'2', two},
    {'3', three}, {'4', four}, {'5', five},
    {'6', six}, {'7', seven}, {'8', eight},
    {'9', nine}
};
