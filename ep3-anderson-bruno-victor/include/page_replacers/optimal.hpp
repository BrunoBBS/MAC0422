#ifndef __PG_REP_OPTIMAL_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

namespace PageReplacers
{
    class Optimal : public PageReplacer
    {
        public:
            Optimal(EP &ep);
            
            bool write(int pos, byte val);

        private:
    };
}

#endif
