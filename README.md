# Documentação do GExplorer

## Integrantes do Projeto
- **Caio Gabriel Alves De Oliveira**
- **Dennis Lucas Gonçalves**
- **Murilo Rodrigues Santos**
- **Vitor Vitorette Serafim De Pina**

## 1. Propósito Principal do Sistema

O **GExplorer** é uma aplicação de visualização e análise de grafos geográficos que processa dados do OpenStreetMap (OSM). O sistema oferece:

- **Visualização Interativa**: Renderização gráfica de redes rodoviárias
- **Análise de Caminhos**: Cálculo de rotas entre pontos origem/destino
- **Interface Intuitiva**: Navegação com zoom, pan e seleção de vértices
- **Exportação**: Salvamento de visualizações em diferentes formatos

## 2. Tecnologias e Frameworks

- **C++17**: Linguagem principal com recursos modernos
- **GTKmm 4**: Framework para interface gráfica
- **Cairo**: Engine de renderização 2D
- **sigc++**: Sistema de sinais e eventos
- **Meson**: Sistema de build

## 3. Arquitetura e Módulos

### Estrutura Principal
```
gexplorer/
├── include/          # Cabeçalhos (.h)
├── src/             # Implementações (.cpp)
├── ui/              # Interfaces gráficas
└── meson.build      # Configuração de build
```

### Módulos Principais

- **GraphDrawingArea**: Componente de visualização e interação
- **Graph**: Estrutura de dados e algoritmos do grafo
- **OSMParser**: Processamento de arquivos OSM XML
- **MainWindow**: Interface principal da aplicação


### GraphDrawingArea
- `set_graph()`: Define o grafo a ser visualizado
- `on_draw()`: Renderiza o grafo usando Cairo
- `set_src_vertex_id/set_tgt_vertex_id()`: Define pontos origem/destino
- Sistema de eventos para zoom, pan e seleção

### Graph
- Representação de vértices com coordenadas geográficas
- Algoritmos de caminho mínimo (Dijkstra)
- Gerenciamento de arestas e conectividade

### OSMParser
- Parsing de XML do OpenStreetMap
- Extração de nodes, ways e tags
- Conversão para estrutura Graph

## 5. Requisitos e Configuração

### Dependências (Ubuntu/Debian)
```bash
sudo apt install build-essential meson pkg-config
sudo apt install libgtkmm-4.0-dev libcairomm-1.16-dev
```

### Dependências (Windows)
#### Usando MSYS2
```bash
# Instalar MSYS2 primeiro: https://www.msys2.org/
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-meson mingw-w64-x86_64-pkg-config
pacman -S mingw-w64-x86_64-gtkmm-4.0 mingw-w64-x86_64-cairomm
```

#### Usando vcpkg (Visual Studio)
```bash
# Instalar vcpkg primeiro
vcpkg install gtkmm:x64-windows
vcpkg install cairomm:x64-windows
```

### Compilação
#### Linux
```bash
cd gexplorer
meson setup build
cd build
meson compile
```

#### Windows (MSYS2)
```bash
cd gexplorer
meson setup build --buildtype=release
cd build
meson compile
```

### Execução
```bash
./gexplorer arquivo.osm
```


## 6. Como Usar a Aplicação

### Carregando um Arquivo OSM
1. Execute o programa: `./gexplorer`
2. Use o menu **File > Open** ou execute diretamente: `./gexplorer arquivo.osm`
3. O grafo será carregado e visualizado automaticamente

### Navegação na Interface
- **Zoom**: Use a roda do mouse para ampliar/reduzir
- **Pan**: Clique e arraste para mover a visualização
- **Reset**: Duplo clique para centralizar a visualização

### Selecionando Pontos
- **Ponto de Origem**: Clique em um vértice (fica destacado em verde)
- **Ponto de Destino**: Clique em outro vértice (fica destacado em vermelho)
- **Cálculo de Rota**: O caminho é calculado automaticamente e exibido

### Informações Exibidas
- **Número de vértices**: Total de pontos no grafo
- **Distância do caminho**: Comprimento da rota calculada
- **Número de pontos na rota**: Quantidade de vértices no caminho

### Exportação
- Use **File > Export** para salvar a visualização atual
- Formatos suportados: PNG, SVG
- Especifique largura e altura desejadas
