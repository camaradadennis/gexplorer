#include "infofield.h"

#include <limits>  // for numeric_limits<>::max()

#define THROW_INVALID_ID(id) \
    { throw Gtk::BuilderError(Gtk::BuilderError::INVALID_ID, \
            "no object named \"" id "\" in ui definition"); }


InfoField::InfoField(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Box(cobject)
{
    m_num = builder->get_widget<Gtk::Label>("num-vertices");
    if (!m_num)
        THROW_INVALID_ID("num-vertices");

    m_num_path = builder->get_widget<Gtk::Label>("num-path");
    if (!m_num_path)
        THROW_INVALID_ID("num-path");

    m_src = builder->get_widget<Gtk::Label>("selected-src");
    if (!m_num)
        THROW_INVALID_ID("selected-src");

    m_tgt = builder->get_widget<Gtk::Label>("selected-tgt");
    if (!m_num)
        THROW_INVALID_ID("selected-tgt");

    m_distance = builder->get_widget<Gtk::Label>("path-distance");
    if (!m_num)
        THROW_INVALID_ID("path-distance");

    m_elapsed = builder->get_widget<Gtk::Label>("path-elapsed");
    if (!m_elapsed)
        THROW_INVALID_ID("path-elapsed");

    set_num();
    set_num_path();
    set_source();
    set_target();
    set_distance();
    set_elapsed_time();
}


void InfoField::set_num(std::optional<std::size_t> num)
{
    if (num)
        m_num->set_label(std::to_string(*num));
    else
        m_num->set_label("-");
}


void InfoField::set_num_path(std::optional<std::size_t> num)
{
    if (num)
        m_num_path->set_label(std::to_string(*num));
    else
        m_num_path->set_label("-");
}


void InfoField::set_source(std::optional<std::size_t> src)
{
    if (src)
        m_src->set_label(std::to_string(*src));
    else
        m_src->set_label("-");
}


void InfoField::set_target(std::optional<std::size_t> tgt)
{
    if (tgt)
        m_tgt->set_label(std::to_string(*tgt));
    else
        m_tgt->set_label("-");
}


void InfoField::set_distance(std::optional<double> dist)
{
    if (dist)
    {
        if (*dist != std::numeric_limits<double>::max())
            m_distance->set_label(std::to_string(*dist));
        else
            m_distance->set_label("no path between vertices");
    }
    else
        m_distance->set_label("-");
}


void InfoField::set_elapsed_time(std::optional<double> elapsed)
{
    if (elapsed)
        m_elapsed->set_label(std::to_string(*elapsed));
    else
        m_elapsed->set_label("-");
}
