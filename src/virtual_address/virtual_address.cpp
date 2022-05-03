/**
 * This file contains implementations for methods in the VirtualAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */
#include<string>
#include<math.h>
#include "virtual_address/virtual_address.h"

using namespace std;

VirtualAddress VirtualAddress::from_string(int process_id, string address) {
    // TODO: implement me
    int page = 0;
    int offset = 0;
    for(int i = 0; i < address.size(); ++i){
        if(i < 10){
            if(address[i] == '1'){
                page += pow(2,9-i);
            }
        }else{
            if(address[i] == '1'){
                offset += pow(2,5-(i-10));
            }
        }
    }

    return VirtualAddress(process_id, page, offset);
}


string VirtualAddress::to_string() const {
    // TODO: implement me
    return bitset<10>(this->page).to_string() + bitset<6>(this->offset).to_string();
}


ostream& operator <<(ostream& out, const VirtualAddress& address) {
    // TODO: implement me
    out << "PID " << address.process_id << " @ "  << address.to_string() << " [page: " << address.page << "; offset: " << address.offset << "]"; // this is a start at least 
    return out;
}