#ifndef __PG_REP_FIFO_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

namespace PageReplacers
{
    class Fifo : public PageReplacer
    {
        public:
            Fifo(EP &ep);
            
            bool write(int pos, byte val);

        private:
    };
}

#endif
