/**
 * This file contains implementations for methods in the Process class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "process/process.h"

using namespace std;


Process* Process::read_from_input(std::istream& in) {
    // TODO
    size_t num_bytes = 0;
    std::vector<Page*> pages;
    string input;
    while( !in.eof()){
        Page* curr_page = Page::read_from_input(in);
        pages.push_back(curr_page);
        num_bytes += in.gcount();
    }
    //size_t num_bytes = in.gcount();


    Process* process = new Process(num_bytes, pages);
    return process;
}


size_t Process::size() const
{
    // TODO
    return num_bytes;
}


bool Process::is_valid_page(size_t index) const
{
    // TODO
    if( index < pages.size() ){
        return true;
    }
    return false;
}


size_t Process::get_rss() const//don't currently know how to calculate this or what it is :(
{
    return page_table.get_present_page_count();
}


double Process::get_fault_percent() const
{
    if(memory_accesses == 0){
        return 0;
    }else{
        return 100*(static_cast<double>(page_faults)/static_cast<double>(memory_accesses));
    }
}
