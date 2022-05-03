/**
 * This file contains implementations for methods in the PhysicalAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "physical_address/physical_address.h"

using namespace std;

string PhysicalAddress::to_string() const {
    // TODO: implement me
    return bitset<10>(this->frame).to_string() + bitset<6>(this->offset).to_string();
}


ostream& operator <<(ostream& out, const PhysicalAddress& address) {
    // TODO: implement me
    out << address.to_string() << " [frame: " << address.frame << "; offset: " << address.offset << "]"; // this is a start at least 

    return out;
}
