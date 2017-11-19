#include "ep_base.hpp"
#include "memory.hpp"
#include "typedef.hpp"
#include "util.hpp"

#include "space_managers/worst_fit.hpp"

#include "page_replacers/lru_4.hpp"

#include <stdio.h>

#include <iostream>
#include <string>

#include <vector>

// Create options
void initialize(EP &ep)
{
    // Add space managers
    ep.add_space_manager(2, new SpaceManagers::WorstFit(ep));

    // Add page replacers
    ep.add_page_replacer(2, new PageReplacers::Lru4(ep));
}

// Main function
int main()
{
    // Base EP class
    EP ep;

    // Initialize
    initialize(ep);

    // Select managers
    ep.select_free_space_manager("2");
    ep.select_page_replace_manager("2");
    ep.load_file("tst/5");
    globals::e = true;
    ep.run("100");
}
