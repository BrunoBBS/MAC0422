#ifndef __PG_REP_LRU_4_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

namespace PageReplacers
{
    class Lru4 : public PageReplacer
    {
        public:
            Lru4(EP &ep);
            
            bool write(int pos, byte val);

        private:
    };
}

#endif
