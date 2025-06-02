#include "main_window.h"

#include <gtkmm/application.h>
#include <gtkmm/builder.h>


namespace {
    Glib::RefPtr<Gtk::Application> app;

    void on_app_activate()
    {
        auto builder = Gtk::Builder::create_from_resource(
            "/io/github/camaradadennis/gexplorer/main_window.ui");

        auto window = Gtk::Builder::get_widget_derived<MainWindow>(
            builder, "main-window");

        if (!window)
            throw Gtk::BuilderError(Gtk::BuilderError::INVALID_ID,
                "no object named \"main-window\" in ui definition");

        app->add_window(*window);
        window->set_visible(true);
    }
}


int main(int argc, char* argv[])
{
    // See: https://gnome.pages.gitlab.gnome.org/gtkmm/classGtk_1_1Application.html#ab8afbd8e83c870d14e6a6545e56e26b3
    Gtk::Application::wrap_in_search_entry2();

    app = Gtk::Application::create("io.github.camaradadennis.gexplorer");
    app->signal_activate().connect([] () { on_app_activate(); });
    return app->run();
}
