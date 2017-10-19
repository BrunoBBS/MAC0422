#include "page_replacer.hpp"

PageReplacer::PageReplacer (EP &ep, int man_id) :
    man_id(man_id)
{
    ep.add_page_replacer(man_id, this);
}
