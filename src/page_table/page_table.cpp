/**
 * This file contains implementations for methods in the PageTable class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "page_table/page_table.h"

using namespace std;


size_t PageTable::get_present_page_count() const {
    // TODO: implement me
    int count = 0;
    for(Row curr_row: rows){
        if(curr_row.present)count++;
    }
    return count;
}


size_t PageTable::get_oldest_page() const {
    // TODO: implement me
    int counter = 0;
    int ret_counter = 0;
    int global_min = -1;
    for(Row curr_row: rows){
        if(curr_row.loaded_at == -1){
            counter++;
            continue;
        }
        if(curr_row.present){
            if(global_min == -1){
                global_min = curr_row.loaded_at;
                ret_counter = counter;
            }
            if(global_min > curr_row.loaded_at){
                global_min = curr_row.loaded_at;
                ret_counter = counter;
            }
        }
        counter++;
    }
    return ret_counter;
}


size_t PageTable::get_least_recently_used_page() const {
    int counter = 0;
    int ret_counter = 0;
    int global_min = -1;
    for(Row curr_row: rows){
        if(curr_row.last_accessed_at == -1){
            counter++;
            continue;
        }
        if(curr_row.present){
            if(global_min == -1){
                global_min = curr_row.last_accessed_at;
                ret_counter = counter;
            }
            if(global_min > curr_row.last_accessed_at){
                global_min = curr_row.last_accessed_at;
                ret_counter = counter;
            }
        }
        counter++;
    }
    return ret_counter;
}
