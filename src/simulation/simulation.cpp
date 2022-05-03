    /**
 * This file contains implementations for the methods defined in the Simulation
 * class.
 *
 * You'll probably spend a lot of your time here.
 */

#include "simulation/simulation.h"
#include <stdexcept>

Simulation::Simulation(FlagOptions& flags)
{
    this->flags = flags;
    this->frames.resize(this->NUM_FRAMES);
}

void Simulation::run() {
    for(size_t i = 0; i < NUM_FRAMES; ++i) {
        free_frames.push_back(i);
    }

    for(auto address: virtual_addresses){
        if(segfault == true){
            break;
        }
        perform_memory_access(address);
        time++;
    }
    if(segfault != true){
        print_summary();   
    }
}

char Simulation::perform_memory_access(const VirtualAddress& virtual_address) {

    Process *process = processes.at(virtual_address.process_id);
    PageTable *table = &process->page_table;//maybe on the & sign

    std::cout << virtual_address << "\n";
    if(!process->is_valid_page(virtual_address.page)){
        std::cout << "SEGFAULT - INVALID PAGE";
        segfault = true;
    }else if(!process->pages.at(virtual_address.page)->is_valid_offset(virtual_address.offset)){
        handle_page_fault(process, virtual_address.page);
        PhysicalAddress phyAddress(table->rows.at(virtual_address.page).frame, virtual_address.offset);
        std::cout << "\t-> physical address " << phyAddress << "\n";
        std::cout << "SEGFAULT - INVALID OFFSET";//invalid offset
        segfault = true;
    }else{
        process->memory_accesses++;//increments memory access by 1

         if(table->rows[virtual_address.page].present){//checks to see if it is in the page table
            table->rows.at(virtual_address.page).last_accessed_at = time;
            std::cout << "\t-> IN MEMORY" << "\n";

        }else{//there is a page fault.
                
            handle_page_fault(process, virtual_address.page);
        }
            //prints out physical address and rss
        PhysicalAddress phyAddress(table->rows.at(virtual_address.page).frame, virtual_address.offset);
        std::cout << "\t-> physical address " << phyAddress << "\n";
        std::cout << "\t-> RSS: " << process->get_rss() << "\n";
    }


    
    std::cout << "\n";
    return 0;
}

void Simulation::handle_page_fault(Process* process, size_t page) {
    // TODO: implement me
    std::cout << "\t-> PAGE FAULT\n";
    page_faults++;
    process->page_faults++;
    process->page_table.rows.at(page).loaded_at = time;
    process->page_table.rows.at(page).last_accessed_at = time;

    if(free_frames.size() != 0 && process->get_rss() < flags.max_frames){
        frames.at(free_frames.front()).set_page(process, page);
        process->page_table.rows.at(page).frame = free_frames.front();
        process->page_table.rows.at(page).present = true;
        
        free_frames.pop_front();
            
    }else{//if there isn't any frames open

        size_t index;

        if(flags.strategy == ReplacementStrategy::FIFO ){//FIFO

            index = process->page_table.get_oldest_page();

        }else{//LRU

            index = process->page_table.get_least_recently_used_page();
            
        }
        process->page_table.rows.at(index).present = false;
        frames.at(process->page_table.rows.at(index).frame).set_page(process, page);//currently erroring rn
        process->page_table.rows.at(page).frame = process->page_table.rows.at(index).frame;
        process->page_table.rows.at(page).present = true;

        // frames[process->page_table.get_oldest_page()].set_page(process, page);
            // process->page_table.rows.at(page).frame = process->page_table.get_oldest_page();
            // process->page_table.rows.at(page).present = true;

    }
    
}

void Simulation::print_summary() {
    if (!this->flags.csv) {
        boost::format process_fmt(
            "Process %3d:  "
            "ACCESSES: %-6lu "
            "FAULTS: %-6lu "
            "FAULT RATE: %-8.2f "
            "RSS: %-6lu\n");

        for (auto entry : this->processes) {
            std::cout << process_fmt
                % entry.first
                % entry.second->memory_accesses
                % entry.second->page_faults
                % entry.second->get_fault_percent()
                % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "\n%-25s %12lu\n"
            "%-25s %12lu\n"
            "%-25s %12lu\n");

        std::cout << summary_fmt
            % "Total memory accesses:"
            % this->virtual_addresses.size()
            % "Total page faults:"
            % this->page_faults
            % "Free frames remaining:"
            % this->free_frames.size();
    }

    if (this->flags.csv) {
        boost::format process_fmt(
            "%d,"
            "%lu,"
            "%lu,"
            "%.2f,"
            "%lu\n");

        for (auto entry : processes) {
            std::cout << process_fmt
                % entry.first
                % entry.second->memory_accesses
                % entry.second->page_faults
                % entry.second->get_fault_percent()
                % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "%lu,,,,\n"
            "%lu,,,,\n"
            "%lu,,,,\n");

        std::cout << summary_fmt
            % this->virtual_addresses.size()
            % this->page_faults
            % this->free_frames.size();
    }
}

int Simulation::read_processes(std::istream& simulation_file) {
    int num_processes;
    simulation_file >> num_processes;

    for (int i = 0; i < num_processes; ++i) {
        int pid;
        std::string process_image_path;

        simulation_file >> pid >> process_image_path;

        std::ifstream proc_img_file(process_image_path);

        if (!proc_img_file) {
            std::cerr << "Unable to read file for PID " << pid << ": " << process_image_path << std::endl;
            return 1;
        }
        this->processes[pid] = Process::read_from_input(proc_img_file);
    }
    return 0;
}

int Simulation::read_addresses(std::istream& simulation_file) {
    int pid;
    std::string virtual_address;

    try {
        while (simulation_file >> pid >> virtual_address) {
            this->virtual_addresses.push_back(VirtualAddress::from_string(pid, virtual_address));
        }
    } catch (const std::exception& except) {
        std::cerr << "Error reading virtual addresses." << std::endl;
        std::cerr << except.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Error reading virtual addresses." << std::endl;
        return 1;
    }
    return 0;
}

int Simulation::read_simulation_file() {
    std::ifstream simulation_file(this->flags.filename);
    // this->simulation_file.open(this->flags.filename);

    if (!simulation_file) {
        std::cerr << "Unable to open file: " << this->flags.filename << std::endl;
        return -1;
    }
    int error = 0;
    error = this->read_processes(simulation_file);

    if (error) {
        std::cerr << "Error reading processes. Exit: " << error << std::endl;
        return error;
    }

    error = this->read_addresses(simulation_file);

    if (error) {
        std::cerr << "Error reading addresses." << std::endl;
        return error;
    }

    if (this->flags.file_verbose) {
        for (auto entry: this->processes) {
            std::cout << "Process " << entry.first << ": Size: " << entry.second->size() << std::endl;
        }

        for (auto entry : this->virtual_addresses) {
            std::cout << entry << std::endl;
        }
    }

    return 0;
}
