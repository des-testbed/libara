#include "omnetpp/RoutingTableWatcher.h" 

#include "RoutingTableEntry.h" 
#include "EnergyAwareRoutingTableEntry.h" 

NAMESPACE_BEGIN

template<class T>
RoutingTableWatcher<T>::RoutingTableWatcher(const char *name, std::unordered_map<std::shared_ptr<ARA::Address>, std::deque<T*>*, ARA::AddressHash, ARA::AddressPredicate>& var) : cStdVectorWatcherBase(name), m(var) {
    itPos = -1;
    // set iterator at least to a useful position
    it = m.begin();
    classname = std::string(name);
}

template<class T>
const char* RoutingTableWatcher<T>::getClassName() const { 
    return classname.c_str(); 
}

template<class T>
const char* RoutingTableWatcher<T>::getElemTypeName() const { 
    return "struct pair<*,*>"; 
}

template<class T>
int RoutingTableWatcher<T>::size() const { 
    return m.size(); 
}

template<class T>
std::string RoutingTableWatcher<T>::at(int i) const {
     if (i==0) {
         it = m.begin(); 
         itPos = 0;
     } else if (i == itPos+1 && it != m.end()) {
         ++it; 
         ++itPos;
     } else {
         it = m.begin();

         for (int k=0; k<i && it!=m.end(); k++){ 
             ++it;
         }

         itPos = i;
     }
 
     if (it == m.end()) {
         return std::string("out of bounds");
     }

     return atIt();
}

template<class T>
std::string RoutingTableWatcher<T>::atIt() const {
    std::stringstream out;
    std::shared_ptr<ARA::Address> first = it->first;
    std::deque<T*>* second = it->second;     
    out << "[destination] " << first.get()->toString() << " ";
    typename std::deque<T*>::iterator i;     

    for(i = second->begin(); i != second->end(); i++){
        out << *(*i);
    } 
    return out.str();
}

template class RoutingTableWatcher<ARA::RoutingTableEntry>;
template class RoutingTableWatcher<ARA::EnergyAwareRoutingTableEntry>;

NAMESPACE_END

