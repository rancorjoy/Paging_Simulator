// Paging_Simulator.cpp : This simulation takes user input and produces paging tables that represent the consents of RAM

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

// Struct to hold user input values for the simulation
struct user_input {
	int phys_mem;		// physical memory size in MB
	int virt_mem;			// virtual memory size in MB
	int page_size;			// page size in KB
	int num_pages;		// number of pages in virtual memory
	int num_frames;		// number of frames in physical memory
};

user_input get_user_input() {					// function to get user input for the simulation

	std::cout << "SET SIMULATION SETTINGS:\n\n";

	bool valid_input = false;						// boolean to check if user input is valid
	user_input input;									// create a struct to hold the user input values

	while (valid_input == false) {
		std::cout << "Enter physical memory size (MB): ";											// get user input for simulation
		std::cin >> input.phys_mem;

		std::cout << "\nEnter virtual memory size (MB): ";
		std::cin >> input.virt_mem;

		std::cout << "\nEnter page size (KB): ";
		std::cin >> input.page_size;

		if (!(input.phys_mem > 0 && input.virt_mem > 0 && input.page_size > 0)) {	// check if all values are positive integers
			std::cout << "Invalid input. Please enter positive integers for all values.\n\n";
		}
		else if ((input.virt_mem * 1024) % input.page_size != 0) {									// check if VM divides evenly by page size
			std::cout << "Invalid input. The given page size does not divide evenly into the Virtual Memory size\n\n";
		}
		else if ((input.phys_mem * 1024) % input.page_size != 0) {								// check if PM divides evenly by page size
			std::cout << "Invalid input. The given page size does not divide evenly into the Physical Memory size\n\n";
		}
		else {
			valid_input = true;																					// if all input is valid, set boolean to true to exit loop

			input.num_pages = (input.virt_mem * 1024) / input.page_size;		// convert virtual memory to KB and divide by page size
			std::cout << "\nNumber of Pages:" << input.num_pages << "\n";

			input.num_frames = (input.phys_mem * 1024) / input.page_size;	// convert physical memory to KB and divide by page size
			std::cout << "Number of Frames:" << input.num_frames << "\n";

			return input;																							// return the user input as a struct
		}
	}
}

int set_page(const std::vector<int> page_map, int page_index) {

	std::vector<int> current_map = page_map;		// create a copy of the page map to modify
	while (true) {														// attempt to get a valid page <-> frame relationship until sucsessful
		int current_frame = 0;									// container variable

		std::cout << "Enter a frame index to map this page " << page_index << " to: ";
		std::cin >> current_frame;

		// Check if the frame is already assigned to another page
		std::vector<int>::iterator it = std::find(current_map.begin(), current_map.end(), current_frame);

		// Check if the iterator is not equal to vec.end()
		if (it != current_map.end()) {
			std::cout << "This frame is already assigned\n";
		}
		else {
			return current_frame;		// if the frame is not already assigned, return the frame index to assign to the page
		}
	}

}

int write_page(int page_index) {
	while (true)
	{
		int data = 0; // container variable
		std::cout << "Enter data to write to page " << page_index << ": ";
		std::cin >> data;
		if (data > -1)
		{
			return data;		// return the data to write to the page
		}
		else
		{
			std::cout << "Invalid input. Please enter a non-negative integer for the data.\n";
		}
	}
}

std::vector<int> set_page_map(std::vector<int> page_map, int num_frames){

	std::vector<int> current_map = page_map;		// create a copy of the page map to modify

	std::cout << "\n\nSET PAGE MAPS:\n";
	std::cout << "Enter '-1' to randomly assign all remaining pages or enter '-2' to continue with current assignments\n\n";
	
	bool map_done= false;						// boolean to check if user input is valid
	while (map_done== false) {
		int current_page = 0;						// container variable

		std::cout << "Enter a page index to map: ";
		std::cin >> current_page;

		if (current_page == -1)					// user entered -1 -> randomly assign all remaining pages to frames
		{
			std::vector<int> used_frames;	// build list of currently used frames
			for (int f : current_map) {
				if (f != -1) used_frames.push_back(f);
			}

			std::vector<int> available_frames; // build list of currently available frames
			for (int i = 0; i < num_frames; i++) { 
				if (std::find(used_frames.begin(), used_frames.end(), i) == used_frames.end()) {
					available_frames.push_back(i);  // frame not yet assigned
				}
			}

			// Shuffle available frames to randomize assignment
			std::random_device rd;		// obtain a random number from hardware
			std::mt19937 rng(rd());		// seed the generator

			std::vector<int> unmapped_pages; // build list of currently unmapped pages
			for (int i = 0; i < current_map.size(); i++) {
				if (current_map[i] == -1) unmapped_pages.push_back(i);
			}

			// Shuffle both pages and frames to randomize assignment (much cheaper than guess and check)
			std::shuffle(available_frames.begin(), available_frames.end(), rng);
			std::shuffle(unmapped_pages.begin(), unmapped_pages.end(), rng);

			// Randomly decide how many pages to map based on number of available frames and unmapped pages
			int max_mappable = std::min((int)unmapped_pages.size(), (int)available_frames.size()); 
			std::uniform_int_distribution<int> dist(1, max_mappable);
			int pages_to_map = dist(rng);

			// Assign pre-shuffled lists to eachother to create random page <-> frame relationships
			for (int i = 0; i < pages_to_map; i++) {
				current_map[unmapped_pages[i]] = available_frames[i];
			}
			map_done = true;
			return current_map; // return the modified page map with the new page <-> frame relationships
		}
		else if (current_page == -2) // user entered -2 -> save and continue current assignments
		{
			map_done = true;
			return current_map;
		}
		else if (current_page < 0) // user entered an invalid page index
		{
			std::cout << "Invalid input, enter a valid page index (0 to n-1): \n";
		}
		else // user entered a valid page index, attempt to set the page <-> frame relationship
		{
			current_map[current_page] = set_page(current_map, current_page);		// call function to set the page map for the current page index
			std::cout << "\n";
		}
	}
}

std::vector<int> set_frame_map(std::vector<int> page_map, std::vector<int> frame_map, int num_frames) {
	std::vector<int> current_map = frame_map;		// create a copy of the frame map to modify
	for (int i = 0; i < page_map.size(); i++) {				// for each page, find frame...
		if (page_map[i] != -1) {
			current_map[page_map[i]] = i;						// assign page to frame
		}
	}
	return current_map;
}

std::vector<int> set_page_data(std::vector<int> page_map, std::vector<int> page_data, int num_pages) {

	std::vector<int> current_map = page_data;		// create a copy of the page map to modify

	std::cout << "\n\nSET PAGE DATA:\n";
	std::cout << "Enter '-1' to randomly populate all remaining pages or enter '-2' to continue with current data\n\n";

	bool map_done = false;						// boolean to check if user input is valid
	while (map_done == false) {
		int current_page = 0;						// container variable

		std::cout << "Enter a page index to populate with data:";
		std::cin >> current_page;

		if (current_page == -1)					// user entered -1 -> randomly populate all remaining pages with data
		{
			std::random_device rd;											// obtain a random number from hardware
			std::mt19937 rng(rd());											// seed the generator
			std::uniform_int_distribution<int> dist(0, 999);	// define the range of random data to write to pages

			for (int i = 0; i < current_map.size(); i++) {			// assign random data to all empty pages that have a frame
				if (current_map[i] == -1 && page_map[i] != -1) {
					current_map[i] = dist(rng);
				}
			}
			map_done = true;
			return current_map; // return randomly populated page data
		}
		else if (current_page == -2)
		{
			map_done = true;
			return current_map;
		}
		else if (current_page < 0)		// user entered an invalid page index
		{
			std::cout << "Invalid input, enter a valid page index (0 to n-1): \n";
		}
		else if (page_map[current_page] == -1) // user entered a page index that is not currently mapped to a frame
		{
			std::cout << "This page is not currently mapped to a frame, so data cannot be written to it\n";
		}
		else // user entered a valid page index, attempt to write data to the page
		{
			current_map[current_page] = write_page(current_page);		// call function to set the page map for the current page index
			std::cout << "\n";
		}
	}
}

std::vector<bool> set_page_valid(std::vector<int> page_map, std::vector<bool> page_valid) {
	for (int i = 0; i < page_map.size(); i++) {
		if (page_map[i] != -1) {
			page_valid[i] = true;  // if the page is mapped to a frame, it is valid
		}
	}
	return page_valid;
}

void print_page_table(std::vector<int> page_map, std::vector<int> page_data, std::vector<bool> page_valid) {
	std::cout << "\nPAGE TABLE:\n";
	std::cout << "Page Index\tFrame Index\tData\tValid\n";
	for (int i = 0; i < page_map.size(); i++) {
		std::cout << i << "\t\t" << page_map[i] << "\t\t" << page_data[i] << "\t" << (page_valid[i] ? "Yes" : "No") << "\n";
	}
}

void print_frame_table(std::vector<int> frame_map) {
	std::cout << "\nFRAME TABLE:\n";
	std::cout << "Frame Index\tPage Index\n";
	for (int i = 0; i < frame_map.size(); i++) {
		std::cout << i << "\t\t" << frame_map[i] << "\n";
	}
}


int main()
{
	user_input input = get_user_input();		// call function to get user input for the simulation

	const int num_pages = input.num_pages;		// create a constant for the number of pages in virtual memory
	const int num_frames = input.num_frames;	// create a constant for the number of frames in physical memory

	std::vector<int> page_map(input.num_pages, -1);		// stores the frame for each page, starts at -1
	std::vector<int> frame_map(input.num_frames, -1);	// stores the page for each frame, starts at -1

	std::vector<int> page_data(input.num_pages, -1);				// stores contents of each page, starts at -1 (no data)
	std::vector<bool> page_valid(input.num_pages, false);		// stores the validity of each page

	page_map = set_page_map(page_map, num_frames);							// call function to set the page map for the simulation
	frame_map = set_frame_map(page_map, frame_map, num_frames);	// call function to set the frame map for the simulation
	page_data = set_page_data(page_map, page_data, num_pages);			// call function to set the page data for the simulation
	page_valid = set_page_valid(page_map, page_valid);								// call function to set the page valid bits for the simulation

	print_page_table(page_map, page_data, page_valid);			// call function to print the page table for the simulation
	print_frame_table(frame_map);												// call function to print the frame table for the simulation
}

