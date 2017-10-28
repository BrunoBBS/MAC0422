#include "page_replacers/lru_2.hpp"

PageReplacers::Lru2::Lru2(EP &ep) :
    PageReplacer(ep, "Least Recently Used v2")
{

}

bool PageReplacers::Lru2::write(int pos, byte val)
{
    return true;
}
