#include "omnetpp/RoutingTableWatcher.h" 

NAMESPACE_BEGIN

RoutingTableWatcher::RoutingTableWatcher(const char *name, std::unordered_map<std::shared_ptr<ARA::Address>, std::deque<ARA::RoutingTableEntry*>*, ARA::AddressHash, ARA::AddressPredicate>& var) : cStdVectorWatcherBase(name), m(var) {
    itPos = -1;
    classname = std::string(name);
}

const char* RoutingTableWatcher::getClassName() const { 
    return classname.c_str(); 
}

const char* RoutingTableWatcher::getElemTypeName() const { 
    return "struct pair<*,*>"; 
}

int RoutingTableWatcher::size() const { 
    return m.size(); 
}

std::string RoutingTableWatcher::at(int i) const {
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

std::string RoutingTableWatcher::atIt() const {
    std::stringstream out;
    std::shared_ptr<ARA::Address> first = it->first;
    std::deque<ARA::RoutingTableEntry*>* second = it->second;     
    out << "[destination] " << first.get()->toString() << " ";
    std::deque<ARA::RoutingTableEntry*>::iterator i;     
    for(i = second->begin(); i != second->end(); i++){
        out << *(*i);
    } 
    return out.str();
}

NAMESPACE_END
