#ifndef SEARCH_FIELD_H
#define SEARCH_FIELD_H

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/searchentry2.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/stringlist.h>

#include <optional>
#include <string>
#include <vector>


class SearchField: public Gtk::Box
{
public:
    using DataType = std::size_t;

    SearchField(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

    void set_data(const std::vector<DataType>&);
    void set_placeholder_text(const std::string&);

    std::optional<DataType> get_selected() const;

private:
    Gtk::SearchEntry2* m_entry;
    Glib::RefPtr<Gtk::SingleSelection> m_selector;
    Glib::RefPtr<Gtk::StringList> m_list;
};

#endif // SEARCH_FIELD_H
