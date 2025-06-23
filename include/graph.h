/** @file graph.h
 *
 * Interface pública da classe `Graph`.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <boost/graph/adjacency_list.hpp>

#include <memory>       // for unique_ptr
#include <optional>
#include <string>
#include <utility>      // for pair
#include <vector>


template<typename Iter, typename T>
class GraphIterator;


/** Classe encapsulando a estrutura de dados de um grafo e operações sobre ela.
 *
 * A classe `Graph` é utilizada para encapsular a estrutura de dados e as
 * operações sobre um grafo. Internamente, ela usa uma classe de
 * lista de adjacências, fornecida pela biblioteca Boost Graph Library (BGL).
 *
 * Atualmente, não existe nenhuma forma de instanciar `Graph` com uma estrurura
 * pronta. Todas as novas instâncias, a não ser que forem cópias de outras
 * instâncias de `Graph`, são inicializadas com um grafo vazio. Os métodos
 * `Graph::add_edge()` e `Graph::add_vertex` são utilizados para compor o grafo.
 *
 * A melhor forma de criar um grafo novo é pelo método estático `Graph::create()`,
 * que retorna um ponteiro único para o novo objeto.
 */
class Graph
{
public:

    /** Coordenadas de um vértice.
     *
     * As coordenadas de cada vértice são armazenadas como um par de pontos
     * (x, y).
     */
    struct VertexCoords { double x; double y; };

    /** Propriedades dos vértices no grafo.
     *
     * O ID deve ser diferenciado do identificador de tipo `Graph::VertexID`.
     * ID é um atributo herdado do openstreetmaps. Este valor *não* é utilizado
     * para acessar diretamente os vértices no grafo. `Graph::VertexID` é, na
     * verdade, o índice do vértice na lista de adjacencias utilizada
     * internamente.
     */
    struct VertexProperties
    {
        std::size_t id;         /**< O ID numérico do vértice. */
        VertexCoords coord;     /**< As coordenadas do vértice no plano. */
    };

    /** Propriedades das arestas no grafo. */
    struct EdgeProperties
    {
        std::string name;       /**< O nome da aresta. Não precisa ser único. */
        double weight;          /**< O peso da aresta. Corresponde à sua distância em metros. */
        bool oneway;            /**< Verdadeiro se a aresta só tiver um sentido. */
    };

    /* Os tipos abaixo são tipos concretos dos templates fornecidos pela BGL. */
    using AdjList = boost::adjacency_list<
        boost::listS, boost::vecS, boost::directedS,
        VertexProperties, EdgeProperties>;

    using VertexT = boost::graph_traits<AdjList>::vertex_descriptor;
    using EdgeT = boost::graph_traits<AdjList>::edge_descriptor;
    using VertexIter = boost::graph_traits<AdjList>::vertex_iterator;
    using EdgeIter = boost::graph_traits<AdjList>::edge_iterator;

    /** Cria uma nova instância de `Graph`.
     *
     * A forma recomendada de instanciar um novo grafo é por meio deste método
     * estático. Embora seja possível utilizar o construtor default e conseguir
     * um ponteiro com `new Graph()`, a utilização de um unique_ptr automatiza
     * o manejo de memória. E uma instância de `Graph` não precisa ser
     * compartilhada por mais de uma estrutura do programa.
     *
     * @return Um ponteiro único para `Graph`.
     */
    static std::unique_ptr<Graph> create();

    /** Adiciona um novo vértice ao grafo.
     *
     * O vértice adicionado não vai estar ligado a nenhum outro, enquanto
     * `Graph::add_edge()` não for utilizado. As propriedades do vértice,
     * passadas por referência, são copiadas para o grafo. É possível adicionar
     * dois vértices com as mesmas propriedades. Serão tratados como dois
     * vértices distintos.
     *
     * @warning Adicionar ou remover vértices do grafo invalida quaisquer
     *          iterators em uso.
     * @param vertex Referência a uma estrutura do tipo `Graph::VertexProperties`.
     * @return O identificador único que referencia o vértice adicionado.
     */
    VertexT add_vertex(const VertexProperties& vertex);

    /** Remove um vértice do grafo.
     *
     * Quaisquer arestas partindo ou chegando no vértice removido também
     * serão removidas.
     *
     * @warning Adicionar ou remover vértices do grafo invalida quaisquer
     *          iterators em uso.
     * @param vertex O identificador único que referencia o vértice a ser removido.
     */
    void remove_vertex(const VertexT& vertex);

    /** Adiciona uma aresta ao grafo.
     *
     * A aresta ligará, direcionalmente, `src` a `tgt`. As propriedades da
     * aresta serão copiadas para o grafo e o caller não precisa se preocupar
     * em manter a estrutura viva no programa.
     *
     * Essa operação pode falhar, caso algum dos vértices de origem ou destino
     * não existirem no grafo. Neste caso, retornará std::nullopt ({}).
     *
     * @warning Adicionar ou remover arestas do grafo invalida quaisquer
     *          iterators em uso.
     * @param src O identificador único do vértice de origem.
     * @param tgt O identificador único do vértice de destino.
     * @param edge Referência para uma estrutura do tipo `Graph::EdgeProperties`.
     * @return O identificador único da aresta adicionada ou {}.
     */
    std::optional<EdgeT> add_edge(const VertexT& src, const VertexT& tgt,
                                  const EdgeProperties& edge);

    /** Retorna o número de vértices no grafo.
     * @return O número de vértices no grafo.
     */
    std::size_t num_vertices() const;

    /** Acessa a estrutura de propriedades de um vérice.
     *
     * O vértice é identificado por seu identificador único. A estrutura
     * de propriedades é retornada por referência constante e não pode ser
     * utilizada para alterar propriedades do vértice armazenado no grafo.
     *
     * @param vertex O identificar único do vértice no grafo.
     * @return Uma referência para a estrutura de propriedades do vértice.
     */
    const VertexProperties& get_vertex_properties(const VertexT&) const;

    /** Acessa a estrutura de propriedades de uma aresta.
     *
     * A aresta é identificada por seu identificador único. A estrutura
     * de propriedades é retornada por referência constante e não pode ser
     * utilizada para alterar propriedades da aresta armazenada no grafo.
     *
     * @param edge O identificar único da aresta no grafo.
     * @return Uma referência para a estrutura de propriedades da aresta.
     */
    const EdgeProperties& get_edge_properties(const EdgeT& edge) const;

    /** Acessa as coordenadas de um vértice.
     *
     * As coordenadas são retornadas como referência constante. Esta função não
     * pode ser utilizada para alterar as coordenadas de um vértice no grafo.
     * É um atalho para `get_vertex_properties(vertex).coords`.
     *
     * @param vertex O identificar único do vértice no grafo.
     * @return Uma referência para as coordenadas do vértice.
     */
    const VertexCoords& get_vertex_coords(const VertexT& vertex) const;

    /** Acessa o ID do vértice.
     *
     * O ID do vértice é um valor numérico de 64 bits. É único entre todos
     * os nós de um mapa.
     *
     * @param vertex O identificar único do vértice no grafo.
     * @return O ID do vértice.
     */
    std::size_t get_vertex_id(const VertexT& vertex) const;

    /** Constrói um vetor com os IDs de vértices no grafo.
     *
     * É basicamente um atalho para criar um vetor manualmente a partir de um
     * iterador para todos os vértices.
     *
     * @return Um vetor com todos os IDs de vértices.
     */
    std::vector<std::size_t> get_vertex_id_list() const;

    /** Encontra o menor caminho entre `src` e `tgt`.
     *
     * O algoritmo utiliza a implementação de dijkstra_shortest_paths()
     * fornecida pela BGL. O vetor `path` é um argumento de entrada e saída. O
     * vetor deve ser passado vazio. `plot_path()` irá adicionar `src`, `tgt` e
     * os vértices entre eles ao vetor. A órdem dos vértices é invertida, ou
     * seja de `tgt` até `src`.
     *
     * No caso de não existir um caminho entre os dois vértices, `path` não é
     * alterado e o valor retornado por `plot_path()` é igual ao máximo valor
     * possível de ser armazenado por uma variável de tipo double.
     *
     * @param src O identificador único do vértice de origem.
     * @param tgt O identificador único do vértice de destino.
     * @param path Vetor onde serão coletados os vértices entre a origem e o
     *        destino.
     * @return A distância total percorrida entre a origem e o destino.
     */
    double plot_path(const VertexT& src, const VertexT& tgt, std::vector<VertexT>& path) const;

    /** Retorna o vértice de origem da aresta.
     *
     * @param edge O identificador único da aresta.
     * @return O identificador único do vértice onde a aresta tem início.
     */
    VertexT get_edge_src(const EdgeT& edge) const;

    /** Retorna o vértice de destino da aresta.
     *
     * @param edge O identificador único da aresta.
     * @return O identificador único do vértice onde a aresta termina.
     */
    VertexT get_edge_tgt(const EdgeT& edge) const;

    /** Retorna o peso da aresta.
     *
     * É um atalho para `Graph::get_edge_properties(edge).weight.
     *
     * O peso da aresta corresponde à distância entre os dois vértices ligados
     * por ela. Se a coordenadas dos vértices são projetadas em um plano
     * cartesiano cuja unidade de medida seja em metros, a distância entre
     * os vértices será em metros.
     *
     * @param edge O identificador único da aresta.
     * @return O peso da aresta.
     */
    double get_edge_weight(const EdgeT& edge) const;

    /** Se é direcionada ou bi-direcionada (via de mão dupla).
     * @return True se a aresta só tiver um sentido. False se for de dois sentidos.
     */
    bool is_edge_oneway(const EdgeT& edge) const;

    /** Retorna um par de iteradores para os vértices do grafo.
     *
     * O primeiro iterador apontará para o primeiro descritor, enquanto o segundo
     * apontará para o descritor após o último.
     *
     * @return Um par de iterators para os vértices do grafo.
     */
    std::pair<VertexIter, VertexIter> iter_vertices() const;

    /** Retorna um par de iteradores para as arestas do grafo.
     *
     * O primeiro iterador apontará para o primeiro descritor de aresta, enquanto
     * o segundo apontará para o descritor após o último.
     *
     * @return Um par de iterators para as arestas do grafo.
     */
    std::pair<EdgeIter, EdgeIter> iter_edges() const;

    /** Retorna o descritor do vértice que se encontra nas coordenadas indicadas.
     *
     * Como os vértices são representados no plano com uma área, ao invés de um
     * ponto único, encontrar o vértice nas coordenadas indicadas envolve uma
     * margem de tolerância. Assim, este método retorna um vértice, se as
     * coordenadas do mesmo estiverem dentro do círculo com centro em `x` e `y`
     * e raio `margin`. Pode retornar nulo caso não haja nenhum vértice nesta
     * área.
     *
     * Se mais de um vértice estiver na área indicada, o primeiro encontrado
     * será retornado. Este método tem complexidade O(n).
     *
     * @param x A posição no eixo X do centro da área buscada.
     * @param y A posição no eixo Y do centro da área buscada.
     * @param margin O raio do círculo com centro em `x` e `y` onde pode existir
     *        um vértice.
     * @return O descritor do vértice na área ou nulo caso não exista nenhum.
     */
    std::optional<VertexT> find_vertex_with_coords(double x, double y, double margin) const;

    /** Retorna o vértice com ID `id`.
     *
     * Este método retorna um par de iterators. Caso exista com vértice com
     * ID = `id`, o primeiro iterator apontara para o descritor deste vértice.
     * Caso não exista, o primeiro iterator será igual ao último, que apontada
     * para o elemento após o fim do vetor de vértices no grafo.
     *
     * @param id O ID do vértice buscado.
     * @return Um par de iterators para os vértices. O primeiro apontará para
     *         o vértice com ID = `id`, se houver, ou para o fim se não houver.
     */
    std::pair<VertexIter, VertexIter> find_vertex_id(std::size_t id) const;

    /** Retorna a aresta com nome = `name`.
     *
     * Este método retorna um par de iterators. Caso exista uma aresta com
     * nome = `name`, o primeiro iterator apontará para o descritor desta
     * aresta. Caso não exista, o primeiro iterator será igual ao segundo, que
     * aponta para o elemento após o último da lista de arestas no grafo.
     *
     * @param name O nome da aresta buscada.
     * @return Um par de iterators para as arestas.
     */
    std::pair<EdgeIter, EdgeIter> find_edge_name(const std::string& name) const;

private:
    AdjList m_adj_list; /**< Lista de adjacências do grafo. */
};


#endif // GRAPH_H
