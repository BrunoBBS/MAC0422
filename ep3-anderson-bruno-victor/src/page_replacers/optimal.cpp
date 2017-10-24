#include "page_replacers/optimal.hpp"

PageReplacers::Optimal::Optimal(EP &ep) :
    PageReplacer(ep, "Optimal")
{

}

bool PageReplacers::Optimal::write(int pos, byte val)
{
    return true;
}
