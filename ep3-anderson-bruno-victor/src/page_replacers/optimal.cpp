#include "page_replacers/optimal.hpp"

PageReplacers::Optimal::Optimal(EP &ep) :
    PageReplacer(ep, "Optimal")
{

}

void PageReplacers::Optimal::init()
{
    // Setup basic values
    page_size = ep.get_page_size();
    phys_page_cnt = ep.phys_size() / page_size;
    virt_page_cnt = ep.virt_size() / page_size;

    // List all events in time order
    event_list.empty();
    instant_references.empty();
    for (auto &time_map : ep.get_events())
    {
        // Add reference from time to list index
        instant_references.insert({time_map.first, event_list.size()});
        // Add events to list
        for (auto &event : time_map.second)
            event_list.push_back(event);
    }

    // All pages are free
    next_free_page = phys_page_cnt - 1;

    // Remove all references from the page tables
    page_table.empty();
    page_table.resize(virt_page_cnt);
    std::fill(page_table.begin(), page_table.end(), -1);
    
    r_page_table.empty();
    r_page_table.resize(phys_page_cnt);
    std::fill(r_page_table.begin(), r_page_table.end(), -1);

    // Wipe clean memory values
    ep.mem_handler()->wipe(0, PHYS, ep.phys_size());
    ep.mem_handler()->wipe(0, VIRT, ep.virt_size());
}

bool PageReplacers::Optimal::write(int pos, byte val)
{
    return ep.mem_handler()->access(translate_address(pos), PHYS, val);
}

bool PageReplacers::Optimal::loaded(int page)
{
    if (page < 0 || ((uint) page) >= page_table.size())
        throw std::invalid_argument("[OPTIMAL] Page " + std::to_string(page)
                + " is not valid!\n");

    return page_table[page] >= 0;
}

void PageReplacers::Optimal::load_page(int page)
{
    if (page < 0 || ((uint) page) >= page_table.size())
        throw std::invalid_argument("[OPTIMAL] Page " + std::to_string(page)
                + " is not valid! Failed to load!\n");

    // If there are any free pages in the physical memory
    if (next_free_page >= 0)
    {
        // Just assign page:

        // Forward reference
        page_table[page] = next_free_page;
        // Back reference
        r_page_table[next_free_page] = page;

        // And copy contents from virtual to physical memory
        ep.mem_handler()->copy(
                page * page_size,
                (next_free_page--) * page_size,
                VIRT, PHYS, page_size);
        return;
    }

    // If not, find best page to replace

    // Number of candidates for replacement
    int candidate_cnt = phys_page_cnt;
    // Status of the pages - true if it may be replaced
    std::vector<bool> pages_status(phys_page_cnt, true);

    // Verify next actions
    int index = instant_references.at(ep.get_run_instant());
    index += ep.get_run_event();

    // Start from next event
    for (index++; ((uint) index) < event_list.size() &&
            candidate_cnt > 1; index++)
    {
        // Get the event
        Event &event = event_list[index];

        // If not a memory access, skip event
        if (event.type != ACCESS)
            continue;

        // Else, get associated process
        Process &process = ep.get_process(event.uid);

        // If process has not been started yet, ignore event
        if (process.get_offset() == -1)
            continue;

        // Calculate page accessed by the event
        int virt_addr = event.pos + process.get_offset();
        int page = virt_addr / page_size;

        // If it's not loaded in the physical memory,
        // can't be replaced anyways
        if (!loaded(page))
            continue;

        // Remove as replacement candidate (is accessed too soon)
        candidate_cnt--;
        pages_status[page_table[page]] = false;
    }

    // When no more accesses will be done or only one candidate is left,
    // replace any of the remaining candidates
    int to_replace = -1;
    for (int candidate = 0; ((uint) candidate) < pages_status.size();
            candidate++)
    {
        if (pages_status[candidate])
        {
            to_replace = candidate;
            break;
        }
    }

    // Then, save changes made to page when in physical memory
    ep.mem_handler()->copy(
            to_replace * page_size,
            r_page_table[to_replace] * page_size,
            PHYS, VIRT, page_size);

    // Load memory content from virtual to physical memory
    ep.mem_handler()->copy(
            page * page_size,
            to_replace * page_size,
            VIRT, PHYS, page_size);

    // Finally, update page tables
    page_table[r_page_table[to_replace]] = -1;
    page_table[page] = to_replace;
    r_page_table[to_replace] = page;
    
    page_fault_cnt++;
}

int PageReplacers::Optimal::translate_address(int virt)
{
    // Get page being accessed
    int page = virt / page_size;

    // If page is not loaded, load it
    if (!loaded(page))
        load_page(page);

    return page_table[page] * page_size;
}
