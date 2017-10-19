#include "page_replacer.hpp"

PageReplacer::PageReplacer (EP &ep, std::string name) :
    name(name)
{
    ep.add_page_replacer(man_id, this);
}

std::string get_name ()
{
    return name;
}
