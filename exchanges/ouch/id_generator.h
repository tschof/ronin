#ifndef __ID_GENERATOR_H__
#define __ID_GENERATOR_H__

struct dstring;

struct id_generator {
    int (*generate_id) (struct id_generator *, struct dstring *);
    void (*free) (void);
};

#endif
