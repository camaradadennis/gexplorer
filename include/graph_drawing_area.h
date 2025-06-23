/** @file graph_drawing_area.h
 *
 * Interface pública da classe `GraphDrawingArea`.
 */

#ifndef GRAPH_DRAWING_AREA_H
#define GRAPH_DRAWING_AREA_H

#include "graph.h"

#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gestureclick.h>

#include <optional>
#include <memory>    // for unique_ptr
#include <utility>   // for pair
#include <vector>


/** Classe responsável por desenhar o grafo na tela.
 *
 * Herda publicamente de Gtk::DrawingArea, uma classe que apresenta um canvas
 * vazio para que o programa desenhe livremente. `GraphDrawingArea` especializa
 * esta classe, no sentido de torná-la específica para desenhar vértices e
 * arestas de um grafo arbitrário.
 *
 * Também, captura inputs do usuário, de modo a permitir ações da zoom e pan
 * no grafo desenhado. Permite a seleção de cada vértice, bem como a seleção
 * de um vértice de origem e outro de destino para que sejam calculado o menor
 * caminho entre eles.
 *
 * Ainda possui umas "flags" setáveis, configurando opções de exibição, como
 * mostrar as setas indicando a direção das arestas ou mostrar os valores
 * números do peso das arestas. Ou, configurando opções de comportamento,
 * como entrar no modo de edição de grafos.
 */
class GraphDrawingArea final: public Gtk::DrawingArea
{
public:
    using SignalChangedSelection = sigc::signal<void()>;

    /** Construtor.
     *
     * Este construtor é utilizado ao instanciar `GraphDrawingArea` a partir
     * de uma definição de UI. Os argumentos são passados automaticamente
     * pelo instanciador de Gtk::Builder.
     *
     * @param cobject O objeto base. Corresponde a um ponteiro para o objeto
     *        Gtk::DrawingArea.
     * @param refBuilder O objeto Gtk::Builder que mantém as definições de UI.
     */
    GraphDrawingArea(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& refBuilder);

    /** Associa a instância de `GraphDrawingArea` ao objeto `Graph`.
     *
     * `Graph` é o objeto que encapsula os dados e métodos de um grafo. Sempre
     * que um grafo novo for acoplado a instância, o antigo será liberado da
     * memória.
     *
     * @param graph Ponteiro único para uma instância de `Graph`.
     */
    void set_graph(std::unique_ptr<Graph>);

    /** Se a instância possui um grafo associado.
     * @return True se houve ou False se o ponteiro para o grafo for nulo.
     */
    bool has_graph() const;

    /** Seleciona um vértice do grafo pelo ID para ser o vértice de origem.
     *
     * O vértice é selecionado pelo ID. Caso o ID não exista, o método retorna
     * `false`.
     *
     * @param id O ID do vértice para ser a origem.
     * @return `true` ou `false` dependendo se houver ou não um vértice com `id`.
     */
    bool set_src_vertex_id(std::size_t id);

    /** Seleciona um vértice do grafo pelo ID para ser o vértice de destino.
     *
     * O vértice é selecionado pelo ID. Caso o ID não exista, o método retorna
     * `false`.
     *
     * @param id O ID do vértice para ser o destino.
     * @return `true` ou `false` dependendo se houver ou não um vértice com `id`.
     */
    bool set_tgt_vertex_id(std::size_t id);

    /** Torna o grafo editável.
     *
     * Ao tornar o grafo editável, o usuário poderá incluir novos vértices e
     * arestas ao grafo.
     *
     * @param state `true` para entrar no modo editável.
     */
    void set_editable(bool state);

    /** Causa a exibição das setas de direção das arestas.
     *
     * Ao habilitar a exibição, pequenas setas serão desenhadas sobre as arestas
     * indicando sua direção.
     *
     * @param state `true` para habilitar a visualização das setas.
     */
    void set_show_arrows(bool state);

    /** Causa a exibição dos pesos das arestas.
     *
     * Ao habilitar a exibição, o valor numérico que representa o peso de cada
     * aresta será visível na área de desenho.
     *
     * @param state `true` para habilitar a visualização dos pesos.
     */
    void set_show_weights(bool state);

    /** Salva a imagem do grafo em `filename`.
     *
     * A imagem será salva em um formato .png, com as dimensões `width` e
     * `height`. A imagem salva não será a porção do grafo que estiver visível
     * na tela, uma vez que este método dimensiona todo o grafo para caber na
     * imagem com as dimensões solicitadas. Contudo, informações como a o
     * caminho marcado, setas e pesos, serão mantidas na imagem salva.
     *
     * @param filename O nome do arquivo onde a imagem será gravada.
     * @param width A largura em pixels da imagem.
     * @param height A altura em pixels da imagem.
     */
    void save_to(const std::string& filename, int width, int height);

    /** Retorna o ID do vértice de origem ou nulo.
     * @return O ID do vértice que estiver selecionado como a origem. Ou nulo,
     *         se não houver.
     */
    std::optional<std::size_t> get_src_vertex_id() const;

    /** Retorna o ID do vértice de destino ou nulo.
     * @return O ID do vértice que estiver selecionado como o destino. Ou nulo,
     *         se não houver.
     */
    std::optional<std::size_t> get_tgt_vertex_id() const;

    /** Retorna o número total de vértices no grafo ou nulo.
     *
     * Este método pode retornar nulo caso não haja nenhum grafo carregado.
     *
     * @return O número total de vértices no grafo ou nulo se não houver grafo.
     */
    std::optional<std::size_t> get_num_vertices() const;

    /** Retorna o número de vértices no caminho selecionado.
     * @return O número de vértices no caminho selecionado ou nulo se não houver
     *         caminho.
     */
    std::optional<std::size_t> get_num_on_path() const;

    /** Retorna a distância total do caminho selecionado.
     *
     * A distância total compreenderá à distância percorrida entra a origem
     * e o destino, passando por todas as arestas entre os dois. Pode ser nula,
     * caso não haja caminho selecionado.
     *
     * @return A distância entre origem e destino, caso exista uma seleção de
     *         caminho.
     */
    std::optional<double> get_path_distance() const;

    /** Retorna o tempo utilizado pelo algoritmo de cálculo de menor caminho.
     *
     * O tempo utilizado será computado a cada nova seleção de origem e destino
     * e será mantido em um membro privado da instância, podendo ser retornado
     * com este método. Pode retornar nulo, caso não haja caminho selecionado.
     *
     * @return O tempo utilizado pela busca de menor caminho. Ou nulo, caso não
     *         exista um caminho selecionado.
     */
    std::optional<double> get_elapsed_time() const;

    /** Sinal emitido sempre que a seleção de vértices mudar.
     *
     * Um cliente que intercepte este sinal, pode solicitar informações sobre
     * o novo estado de seleção do grafo por meio de seus métodos get_*.
     */
    SignalChangedSelection signal_changed_selection();

private:
    /** Método responsável por desenhar os elementos na tela.
     *
     * Este método é o núcleo da classe. É chamado a cada vez que se torna
     * necessário redesenhar os elementos da tela.
     *
     * @param cr O objeto Cairo::Context que é passado à função automaticamente.
     * @param width A largura em pixels da tela disponível para o usuário.
     * @param height A altura em pixels da tela disponível para o usuário.
     */
    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    /** Captura o início da ação de "arrasto" do usuário.
     * @param x A coordenada de onde começou o arrasto.
     * @param y A coordenada de onde começou o arrasto.
     */
    void on_drag_begin(double x, double y);

    /** Captura a continuidade da ação de "arrasto" do usuário.
     * @param offset_x O quanto a ação se distanciou do ponto de origem.
     * @param offset_y O quanto a ação se distanciou do ponto de origem.
     */
    void on_drag_update(double offset_x, double offset_y);

    /** Captura a ação de clique do mouse do usuário.
     * @param n_press Quantas vezes o botão de mouse foi pressionado.
     * @param x A coordenada onde ocorreu o clique.
     * @param y A coordenada onde ocorreu o clique.
     * @param click O objeto que representa o clique. Utilizado para inspecionar
     *              se tal clique foi com o botão principal ou secundário.
     */
    void on_click(int n_press, double x, double y,
                  Glib::RefPtr<Gtk::GestureClick>& click);

    /** Capura a ação de zoom efetuada pela rolagem da roda no mouse.
     * @param dx A distância percorrida pela roda.
     * @param dy A distância percorrida pela roda.
     * @return `true` se o evento tiver sido tratado com sucesso.
     */
    bool on_scroll(double dx, double dy);

    /** Captura ações de teclado do usuário.
     * @param keyval O valor numérico da tecla utilizada pelo usuário.
     * @param keycode O código da tecla utilizada pelo usuário.
     * @param modifier O valor da tecla modificadora (ALT, CTRL, etc), se
     *                 tiver sido utilizada.
     * @return `true` se o evento tiver sido processado com sucesso.
     */
    bool on_key_pressed(guint, guint, Gdk::ModifierType);

    /** Seleciona `vertex` como o vértice de origem.
     * @param vertex O descritor do vértice de origem.
     */
    void set_src_vertex(const Graph::VertexT& vertex);

    /** Seleciona `vertex` como o vértice de destino.
     *
     * Logo após um destino ser selecionado, se houver uma origem também
     * selecionada, a instância de `GraphDrawingArea` já irá chamar o algoritmo
     * de computação de menor caminho entre os dois pontos.
     *
     * @param vertex O descritor do vértice de destino.
     */
    void set_tgt_vertex(const Graph::VertexT& vertex);

    bool m_editable{ false };       /**< Flag de modo edição. */
    bool m_view_arrows{ false };    /**< Flag de exibição de setas. */
    bool m_view_weights{ false };   /**< Flag de exibição de pesos. */

    double m_scale_factor{ 1.0 };   /**< Armazena o fator de escala (zoom). */
    double m_offset_x{ 0.0 };       /**< Armazena o offset da visualização, com relação ao centro. */
    double m_offset_y{ 0.0 };       /**< Armazena o offset da visualização, com relação ao centro. */
    double m_drag_start_x{ 0.0 };   /**< Armazena o ponto de origem da ação de pan. */
    double m_drag_start_y{ 0.0 };   /**< Armazena o ponto de origem da ação de pan. */

    std::unique_ptr<Graph> m_graph{ nullptr };      /**< Grafo. */
    std::optional<Graph::VertexT> m_src_vertex{};   /**< Vértice de origem. */
    std::optional<Graph::VertexT> m_tgt_vertex{};   /**< Vértice de destino. */
    std::optional<double> m_path_distance;          /**< Distância. */
    std::optional<double> m_path_processing_time;   /**< Tempo de processamento do menor caminho. */
    std::vector<Graph::VertexT> m_path;             /**< Vetor com os vértices entre origem e destino. */

    SignalChangedSelection m_signal_changed_selection; /**< Sinal emitido. */
};

#endif // GRAPH_DRAWING_AREA_H
