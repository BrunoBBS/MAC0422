#ifndef __PAGE_REPLACER_HPP__
#define __PAGE_REPLACER_HPP__

#include "ep_base.hpp"

class EP;

class PageReplacer
{
    public:
        PageReplacer(EP &ep, int man_id);

        virtual void access(int pos) = 0;

    private:
        int man_id;
};

#endif
