#ifndef DCEL_EXCEPTION_H
#define DCEL_EXCEPTION_H

namespace O::DCEL
{
    struct Exception
    {
        enum Type
        {
            VERTICES_OVERFLOW,
            HALF_EDGES_OVERFLOW,
            FACES_OVERFLOW,
        };
        Type type;
    };
}

#endif //DCEL_EXCEPTION_H