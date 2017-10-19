#include "page_replacer.hpp"

PageReplacer::PageReplacer (EP &ep, std::string name) :
    name(name)
{
}

std::string PageReplacer::get_name ()
{
    return name;
}
