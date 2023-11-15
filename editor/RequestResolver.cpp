//
// Created by marvin on 15.11.2023.
//

#include "RequestResolver.h"
#include "Scene.h"
#include <iostream>

namespace moonshine {
    void RequestResolver::resolve(boost::json::object jObj) {
        
        boost::json::string action = jObj["action"].get_string();
        if (std::equal(action.begin(), action.end(), "updateObject")) {
            int64_t uniqueId = jObj["objectId"].as_int64();
            Scene &scene = Scene::getCurrentScene();
            {
                scene.getLock();
                //TODO: Change once uniqueId is implemented
                scene.get_at(uniqueId)->getTransform()->deserialize(jObj);
            }
        } else if (std::equal(action.begin(), action.end(), "updateScene")){
            for(auto obj : jObj["sceneObjects"].as_array()){
                resolve(obj.as_object());
            }
        }
    }
} // moonshine