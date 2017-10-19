#ifndef __PAGE_REPLACER_HPP__
#define __PAGE_REPLACER_HPP__

#include "ep_base.hpp"

class EP;

class PageReplacer
{
    public:
        PageReplacer(EP &ep, std::string name);

        std::string get_name();

        virtual void access(int pos) = 0;

    private:
        std::string name;
};

#endif
