/** @file osm_parser.h
 *
 * `osm_parser` é o módulo de carregamento de dados no programa.
 * Destina-se a leitura de dados no formato nativo do OpenStreetMaps.
 * Caso uma extensão seja implementada para carregar dados em outros formatos,
 * deve-se escrever outro módulo que retorne um ponteiro para uma instância
 * de `Graph`.
 */
#ifndef OSM_PARSER_H
#define OSM_PARSER_H

#include "graph.h"

#include <string>
#include <stdexcept>

/** Namespace osm_parser
 *
 * Evita poluir o namespace root do aplicativo com um módulo acessório.
 */
namespace osm_parser
{
    /** Carrega um novo grafo a partir do arquivo.
     *
     * Esta função é responsável por carregar um novo grafo a partir do arquivo
     * em formato OSM XML. Ela pode jogar (throw) `osm_parser::ParserError` e
     * retornar nulo caso a leitura não seja bem suscedida.
     *
     * @param filename O caminho para o arquivo que se deseja abrir.
     * @return Um ponteiro para uma instância da `Graph` contendo os dados
     *         do arquivo carregado.
     */
    std::unique_ptr<Graph> parse(const std::string& filename);

    /** Erro indicando falha na leitura ou parsing do arquivo. */
    class ParserError: public std::runtime_error
    {
    public:
        explicit ParserError(const std::string& what)
            : std::runtime_error(what) {}
    };
}

#endif // OSM_PARSER_H
