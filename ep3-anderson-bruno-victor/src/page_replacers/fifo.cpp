#include "page_replacers/fifo.hpp"

PageReplacers::Fifo::Fifo(EP &ep) :
    PageReplacer(ep, "FIFO")
{

}

bool PageReplacers::Fifo::write(int pos, byte val)
{
    return true;
}
