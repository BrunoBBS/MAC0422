#ifndef __PAGE_REPLACER_HPP__
#define __PAGE_REPLACER_HPP__

#include "ep_base.hpp"

class EP;

class PageReplacer
{
    public:
        PageReplacer(EP &ep, int man_id);

    private:
        int man_id;
};

#endif
