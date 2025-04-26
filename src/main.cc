#include "main_window.h"

#include <gtkmm/application.h>
#include <gtkmm/builder.h>

namespace {

Glib::RefPtr<Gtk::Application> app;

void on_app_activate()
{
    auto refBuilder = Gtk::Builder::create_from_resource(
        "/io/github/camaradadennis/gexplorer/main_window.ui");

    auto window = Gtk::Builder::get_widget_derived<MainWindow>(
        refBuilder, "main_window");

    app->add_window(*window);
    window->set_visible(true);
}

} // anonymous namespace

int main(int argc, char* argv[])
{
    app = Gtk::Application::create("io.github.camaradadennis.gexplorer");
    app->signal_activate().connect([] () { on_app_activate(); });
    return app->run();
}
