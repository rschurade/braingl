/*
 * vptr.h
 *
 *  Created on: 17.06.2012
 *      Author: Ralph Schurade
 */

#ifndef VPTR_H_
#define VPTR_H_

template<class T> class VPtr
{
public:
    static T* asPtr( QVariant v )
    {
        return (T *) v.value<void *>();
    }

    static QVariant asQVariant( T* ptr )
    {
        return qVariantFromValue( (void *) ptr );
    }
};

#endif /* VPTR_H_ */
