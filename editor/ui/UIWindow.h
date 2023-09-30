//
// Created by marvin on 30.09.2023.
//

#ifndef MOONSHINE_UIWINDOW_H
#define MOONSHINE_UIWINDOW_H

namespace moonshine {

    class UIWindow {
    public:
        virtual ~UIWindow() {}
        
        virtual void draw() = 0;
    };

} // moonshine

#endif //MOONSHINE_UIWINDOW_H
