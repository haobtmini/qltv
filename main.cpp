#include "logic.h"
#include "ui.h"

int main() {
    
    LibraryService service;
    service.loadFromFile();
    LibraryTUI app(service);
    app.run();

    return 0;
}
