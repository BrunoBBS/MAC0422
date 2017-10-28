#include "page_replacers/lru_4.hpp"

PageReplacers::Lru4::Lru4(EP &ep) :
    PageReplacer(ep, "Least Recently Used v4")
{

}

bool PageReplacers::Lru4::write(int pos, byte val)
{
    return true;
}
