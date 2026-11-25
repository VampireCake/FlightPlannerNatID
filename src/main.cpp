//
//  Created by Izudin Dzafic on 10 Nov 2022.
//  Copyright © 2022 IDz. All rights reserved.
//
#include "Application.h"
#include <gui/WinMain.h>

int main(int argc, const char* argv[])
{
    Application app(argc, argv);
    auto appProperties = app.getProperties();
    td::String trLang = appProperties->getValue("translation", "ba");
    trLang.toLower();
    app.init(trLang);
    return app.run();
}
