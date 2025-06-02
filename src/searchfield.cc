#include "searchfield.h"

#include <gtkmm/stringobject.h>

#define THROW_INVALID_ID(id) \
    { throw Gtk::BuilderError(Gtk::BuilderError::INVALID_ID, \
            "no object named \"" id "\" in ui definition"); }


SearchField::SearchField(BaseObjectType* cobject,
                         const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Box(cobject)
{
    auto field_builder = Gtk::Builder::create_from_resource(
        "/io/github/camaradadennis/gexplorer/search_field.ui");

    auto container = field_builder->get_widget<Gtk::Box>("container");
    if (!container)
        THROW_INVALID_ID("container");

    m_entry = field_builder->get_widget<Gtk::SearchEntry2>("entry");
    if (!m_entry)
        THROW_INVALID_ID("entry");

    m_selector = std::dynamic_pointer_cast<Gtk::SingleSelection>(
        field_builder->get_object("selector"));
    if (!m_selector)
        THROW_INVALID_ID("selector");

    m_list = std::dynamic_pointer_cast<Gtk::StringList>(
        field_builder->get_object("list"));
    if (!m_list)
        THROW_INVALID_ID("list");

    append(*container);
}


void SearchField::set_data(const std::vector<SearchField::DataType>& data_vec)
{
    while (m_list->get_n_items() > 0)
        m_list->remove(0);

    for (const auto& item: data_vec)
        m_list->append(std::to_string(item));
}


void SearchField::set_placeholder_text(const std::string& text)
{
    m_entry->set_placeholder_text(text);
}


std::optional<SearchField::DataType> SearchField::get_selected() const
{
    auto selected = std::dynamic_pointer_cast<Gtk::StringObject>(
        m_selector->get_selected_item());

    if (selected)
        return std::stoull(selected->get_string());
    else
        return std::nullopt;
}
