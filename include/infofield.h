/** @file infofield.h
 *
 * Implementa a classe responsável por exibir as estatísticas de utilização
 * do programa.
 */

#ifndef INFO_FIELD_H
#define INFO_FIELD_H

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/label.h>

#include <optional>
#include <string>

/** Carrega em si os vários campos de exibição de estatísticas.
 *
 * Os campos podem ser acrescentados ou removidos facilmente.
 * A instância de `InfoField` é atualizada pela janela principal do programa,
 * utilizando seus vários métodos set_*.
 *
 * Total os métodos set_* aceitam um valor opcional como argumento, de modo
 * que é possível setar cada um dos campos para um valor vazio.
 *
 * Internamente, a instância de `InfoField` mantém referências para uma série
 * de labels que são utilizados para exibir cada uma das informações.
 */
class InfoField: public Gtk::Box
{
public:
    /** Construtor.
     * @param cobject O objeto base, neste caso um Gtk::Box.
     * @param builder O objeto builder utilizado para instanciar esta classe.
     */
    InfoField(BaseObjectType*, const Glib::RefPtr<Gtk::Builder>&);

    /** Exibe o número total de vértices no grafo.
     * @param num O número total de vértices no grafo exibido.
     */
    void set_num(std::optional<std::size_t> num = {});

    /** Exibe o número de vértices no caminho selecionado.
     * @param num Número de vértices no caminho selecionado.
     */
    void set_num_path(std::optional<std::size_t> num = {});

    /** Exibe o ID do vértice de origem.
     * @param src O ID do vértice de origem.
     */
    void set_source(std::optional<std::size_t> src = {});

    /** Exibe o ID do vértice de destino.
     * @param tgt O ID do vértice de destino.
     */
    void set_target(std::optional<std::size_t> tgt = {});

    /** Exibe a distância entre a origem e o destino.
     * @param dist A distância entre a origem e o destino.
     */
    void set_distance(std::optional<double> dist = {});

    /** Exibe o tempo de execução do algoritomo de busca.
     * @param elapsed O tempo, em segundos, de execução do algoritmo.
     */
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
