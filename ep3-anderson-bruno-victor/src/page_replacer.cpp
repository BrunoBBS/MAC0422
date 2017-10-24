#include "page_replacer.hpp"

PageReplacer::PageReplacer (EP &ep,
        std::string name) :
    ep(ep),
    memory(ep.mem_handler()),
    name(name)
{
}

std::string PageReplacer::get_name ()
{
    return name;
}
