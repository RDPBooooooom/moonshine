//
// Created by marvin on 15.11.2023.
//

#ifndef MOONSHINE_REQUESTRESOLVER_H
#define MOONSHINE_REQUESTRESOLVER_H

#include <boost/json/object.hpp>

namespace moonshine {

    class RequestResolver {

        
    public:
        
        void resolve(boost::json::object jObj);
    };

} // moonshine

#endif //MOONSHINE_REQUESTRESOLVER_H
