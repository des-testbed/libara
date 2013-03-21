/*
 * $FU-Copyright$
 */

#ifndef __UNORDEREDMAP_WATCH_H
#define __UNORDEREDMAP_WATCH_H

#include "cwatch.h"
#include "cownedobject.h"

#include <unordered_map>

NAMESPACE_BEGIN

template<class KeyType, class MappedType, class Hasher, class Predicate, class AllocatorType>
class UnorderedMapWatcher : public cStdVectorWatcherBase {
    protected:
        std::unordered_map<KeyType, MappedType, Hasher, Predicate, AllocatorType>& m;
        mutable typename std::unordered_map<KeyType, MappedType, Hasher, Predicate, AllocatorType>::iterator it;
        mutable int itPos;
        std::string classname;

   public:
        UnorderedMapWatcher(const char *name, std::unordered_map<KeyType, MappedType, Hasher, Predicate, AllocatorType>& var) : cStdVectorWatcherBase(name), m(var) {
            itPos=-1;
            classname = std::string("std::unordered_map<")+opp_typename(typeid(KeyType))+","+opp_typename(typeid(MappedType))+","+opp_typename(typeid(Hasher))+","+opp_typename(typeid(Predicate))+","+opp_typename(typeid(AllocatorType))+ ">";
        }

        const char *getClassName() const { return classname.c_str(); }

        virtual const char *getElemTypeName() const { return "struct pair<*,*>"; }

        virtual int size() const { return m.size(); }

        virtual std::string at(int i) const {
            if (i==0) {
                it=m.begin(); itPos=0;
            } else if (i==itPos+1 && it!=m.end()) {
                ++it; ++itPos;
            } else {
                it=m.begin();
                for (int k=0; k<i && it!=m.end(); k++) ++it;
                itPos=i;
            }

            if (it==m.end()) {
                return std::string("out of bounds");
            }

            return atIt();
        }

        virtual std::string atIt() const {
            std::stringstream out;
            out << it->first << " ==> " << it->second;
            return out.str();
        }
};

template <class KeyType, class MappedType, class Hasher, class Predicate, class AllocatorType>
void createStdUnorderedMapWatcher(const char *varname, std::unordered_map<KeyType, MappedType, Hasher, Predicate, AllocatorType>& m) {
     new UnorderedMapWatcher<KeyType, MappedType, Hasher, Predicate, AllocatorType>(varname, m);
}

#define WATCH_UNORDEREDMAP(m)         createStdUnorderedMapWatcher(#m,(m))

NAMESPACE_END

#endif
