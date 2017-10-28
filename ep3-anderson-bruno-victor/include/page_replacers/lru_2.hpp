#ifndef __PG_REP_LRU_2_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

namespace PageReplacers
{
    class Lru2 : public PageReplacer
    {
        public:
            Lru2(EP &ep);
            
            bool write(int pos, byte val);

        private:
    };
}

#endif
