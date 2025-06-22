#ifndef INFO_FIELD_H
#define INFO_FIELD_H

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/label.h>

#include <optional>
#include <string>


class InfoField: public Gtk::Box
{
public:
    InfoField(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

    void set_num(std::optional<std::size_t> num = {});
    void set_num_path(std::optional<std::size_t> num = {});
    void set_source(std::optional<std::size_t> src = {});
    void set_target(std::optional<std::size_t> tgt = {});
    void set_distance(std::optional<double> dist = {});
    void set_elapsed_time(std::optional<double> elapsed = {});

private:
    Gtk::Label* m_num;
    Gtk::Label* m_num_path;
    Gtk::Label* m_src;
    Gtk::Label* m_tgt;
    Gtk::Label* m_distance;
    Gtk::Label* m_elapsed;
};

#endif // INFO_FIELD_H
