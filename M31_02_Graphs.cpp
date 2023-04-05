#include <iostream>
#include <map>

// Interface class
class Interface {

public:

    static const int sizeOfSplitter = 40;

    static void outSplitter (char sign = '-') {

        std::string splitter;
        splitter.assign(sizeOfSplitter, sign);
        std::cout << splitter << std::endl;

    }

    static void outHeader(const char* headerText) {

        outSplitter('=');
        std::cout << headerText << std::endl;
        outSplitter('=');

    }

    // Stream block
    static void refreshInStream() {
        std::cin.clear();
        std::cin.seekg(0);
    }

};

// Classes graphs
class IGraph {

    virtual void addVertex(int addNum) = 0;

    // Copy - copy mode, without checks
    virtual void addingEdge(int from, int to, bool copy) = 0;

protected:

    struct DirCount {
        int to = 0;
        int from = 0;
    };

    // Describe graph
    // 1) int - Current vertices (every);
    // 2) int - Next vertices (by form graph);
    // 3) DirCount - direction count (int to, from)
    std::map<int, std::map<int, DirCount>> data;

    void fillByObj(const IGraph* srcGraph) {

        data.clear();

        for(auto &currPair : srcGraph->data)
            for(auto &nextPair : currPair.second) {
                if (currPair.first == nextPair.first) continue;
                int wayCount = nextPair.second.from;
                while (wayCount-- > 0) addingEdge(currPair.first, nextPair.first, true);
            }

    }

public:

    //IGraph() = default;
    //IGraph(const IGraph& srcGraph) {}

    // Read start and finish vertices and add edge
    virtual void AddEdge(int from, int to) = 0;

    // Return count of vertices
    virtual int VerticesCount() const = 0;

    // Get target vertices
    virtual void GetNextVertices(int vertex, std::map<int, int>& vertices) const = 0;

    // Get initial vertices
    virtual void GetPrevVertices(int vertex, std::map<int, int>& vertices) const = 0;

};

// Table of vertices connections
class MatrixGraph : public IGraph {

    void addVertex(int addNum) override {

        data[addNum][addNum]; // Me

        // Bypass every vertex
        for (auto &dataPair : data) {
            // Added current vertex
            data[addNum][dataPair.first];
            // Added next vertices
            dataPair.second[addNum];
        }

    }

    void addingEdge(int from, int to, bool copy) override {

        bool isFrom = data.find(from) != data.end();
        bool isTo = data.find(to) != data.end();

        if (!copy && from == to)
            std::cout << "Edge " << from << " -> " << to << " is not exist!\n";

        else if (!copy && !data.empty() && !isFrom && !isTo)
            std::cout << "Edge " << from << " -> " << to << " is not connected!\n";

        else {

            // Add vertices if needed
            if (!isFrom) addVertex(from);
            if (!isTo) addVertex(to);

            // Add edge
            data[from][to].from++;
            data[to][from].to++;

            // Report
            std::cout << "Add edge from " << from << " to " << to << std::endl;

        }

    }

    void getDirections(int vertex, std::map<int, int>& vertices, bool dirTo) const {

        // Initialization
        vertices.clear();
        std::map<int, std::map<int, DirCount>>::const_iterator currVertex_It
                = data.find(vertex);

        // Collecting vertices data
        if (currVertex_It != data.end())
            for (auto &nextPair : currVertex_It->second) {
                // Exclude itself
                if (nextPair.first == currVertex_It->first) continue;
                // Get count of ways
                int dirCnt = (dirTo ? nextPair.second.to : nextPair.second.from);
                if (dirCnt > 0) vertices[nextPair.first] = dirCnt;
            }

    }

public:

    MatrixGraph() = default;
    MatrixGraph(const IGraph* srcGraph) { fillByObj(srcGraph); }
    MatrixGraph& operator=(const IGraph* srcGraph) {
        if (srcGraph == this) return *this;
        fillByObj(srcGraph);
        return *this;
    }

    void AddEdge(int from, int to) override { addingEdge(from, to, false); }

    int VerticesCount() const override { return (int) data.size(); }

    void GetNextVertices(int vertex, std::map<int, int>& vertices) const override {
        getDirections(vertex, vertices, false);
    };

    void GetPrevVertices(int vertex, std::map<int, int>& vertices) const override {
        getDirections(vertex, vertices, true);
    };

};

// List of vertices with list of connections
class ListGraph : public IGraph {

    void addVertex(int addNum) override { data[addNum]; }

    void addingEdge(int from, int to, bool copy) override {

        bool isFrom = data.find(from) != data.end();
        bool isTo = data.find(to) != data.end();

        if (from == to)
            std::cout << "Edge " << from << " -> " << to << " is not exist!\n";

        else if (!copy && !data.empty() && !isFrom && !isTo)
            std::cout << "Edge " << from << " -> " << to << " is not connected!\n";

        else {

            // Add vertices if needed
            if (!isFrom) addVertex(from);
            if (!isTo) addVertex(to);

            // Add edge (insert in map not overwrite data)
            data[from][to].from++;
            data[to][from].to++;

            // Report
            std::cout << "Add edge from " << from << " to " << to << std::endl;

        }

    }

    void getDirections(int vertex, std::map<int, int>& vertices, bool dirTo) const {

        // Initialization
        vertices.clear();
        std::map<int, std::map<int, DirCount>>::const_iterator currVertex_It
                = data.find(vertex);

        // Collecting vertices data
        if (currVertex_It != data.end())
            for (auto &nextPair : currVertex_It->second) {
                int dirCnt = (dirTo ? nextPair.second.to : nextPair.second.from);
                if (dirCnt > 0) vertices[nextPair.first] = dirCnt;
            }

    }

public:

    ListGraph() = default;
    ListGraph(const IGraph* srcGraph) { fillByObj(srcGraph); }
    ListGraph& operator=(const IGraph* srcGraph) {
        if (srcGraph == this) return *this;
        fillByObj(srcGraph);
        return *this;
    }

    void AddEdge(int from, int to) override { addingEdge(from, to, false); }

    int VerticesCount() const override { return (int) data.size(); }

    void GetNextVertices(int vertex, std::map<int, int>& vertices) const override {
        getDirections(vertex, vertices, false);
    };

    void GetPrevVertices(int vertex, std::map<int, int>& vertices) const override {
        getDirections(vertex, vertices, true);
    };

};

// Duty block
void outDir(std::map<int, int>& dir) {
    if (dir.empty()) std::cout << " none\n";
    else {
        std::cout << '\n';
        for (auto &currDir : dir)
            std::cout << "- Vertex: " << currDir.first << ", ways: " << currDir.second << std::endl;
    }
}

void outAllDir(int vertex, std::map<int, int>& dirFrom, std::map<int, int>& dirTo) {

    std::cout << "For vertex: " << vertex << std::endl;
    std::cout << "Direction from:";
    outDir(dirFrom);
    std::cout << "Direction to:";
    outDir(dirTo);
    Interface::outSplitter();

}

void outDirByGraph(MatrixGraph& graph, int vertex) {

    std::map<int, int> directions_to, directions_from;
    graph.GetNextVertices(vertex, directions_to);
    graph.GetPrevVertices(vertex, directions_from);

    outAllDir(vertex, directions_from, directions_to);

}

void outDirByGraph(ListGraph& graph, int vertex) {

    std::map<int, int> directions_to, directions_from;
    graph.GetNextVertices(vertex, directions_to);
    graph.GetPrevVertices(vertex, directions_from);

    outAllDir(vertex, directions_from, directions_to);

}

//
int main() {

    Interface::outHeader("Graphs");

    // Initialization
    std::map<int, int> directions;
    MatrixGraph graph;
    //ListGraph graph;

    // Check matrix
    std::cout << "Check matrix graph\n";
    Interface::outSplitter();
    std::cout << "Empty - count vertices: " << graph.VerticesCount() << std::endl;
    outDirByGraph(graph, 0);
    outDirByGraph(graph, 5);
    outDirByGraph(graph, -1);
    graph.AddEdge(3, 5);
    Interface::outSplitter();
    outDirByGraph(graph, 3);
    outDirByGraph(graph, 5);
    graph.AddEdge(3, 5);
    graph.AddEdge(5, 3);
    Interface::outSplitter();
    outDirByGraph(graph, 3);
    outDirByGraph(graph, 5);
    std::cout << "Wrong edge...\n";
    graph.AddEdge(2, 7);
    graph.AddEdge(7, 7);
    Interface::outSplitter();
    std::cout << "Count vertices: " << graph.VerticesCount() << std::endl;
    Interface::outSplitter();
    graph.AddEdge(2, 5);
    graph.AddEdge(1, 5);
    graph.AddEdge(3, 1);
    graph.AddEdge(2, 3);
    Interface::outSplitter();
    outDirByGraph(graph, 3);
    outDirByGraph(graph, 5);
    outDirByGraph(graph, 1);
    outDirByGraph(graph, 2);
    std::cout << "Count vertices: " << graph.VerticesCount() << std::endl;

/*
    // Optimal check
    ListGraph newGraph_1(&graph);
    MatrixGraph newGraph_2(&newGraph_1);
    newGraph_2 = &graph;
    //
    MatrixGraph newGraph_3(&graph);
    ListGraph newGraph_4(&newGraph_3);
    newGraph_3 = &graph;
    //
    graph.AddEdge(5, 12);
    newGraph_1 = &graph;
    newGraph_2 = &graph;
    graph.AddEdge(12,13);
*/

    // Copy
    {
        Interface::outSplitter('=');
        std::cout << "Copy to List graph...\n";
        Interface::outSplitter();
        //
        ListGraph newGraph(&graph);
        //ListGraph newGraph;
        //
        //MatrixGraph* testGraph = new MatrixGraph;
        //newGraph = *testGraph;
        //delete testGraph;
        //
        newGraph.AddEdge(10, 11);
        newGraph = &graph;
        //
        Interface::outSplitter();
        outDirByGraph(newGraph, 3);
        outDirByGraph(newGraph, 5);
        outDirByGraph(newGraph, 1);
        outDirByGraph(newGraph, 2);
        std::cout << "Count vertices: " << newGraph.VerticesCount() << std::endl;
        Interface::outSplitter();
        newGraph.AddEdge(5,12); // Check asynchronous
        outDirByGraph(newGraph, 12);
        std::cout << "Count vertices: " << newGraph.VerticesCount() << std::endl;
    }

    // Copy
    {
        Interface::outSplitter('=');
        std::cout << "Copy to Matrix graph...\n";
        Interface::outSplitter();
        //
        MatrixGraph newGraph(&graph);
        //MatrixGraph newGraph;
        //
        //ListGraph* testGraph = new ListGraph;
        //newGraph = *testGraph;
        //delete testGraph;
        //
        newGraph.AddEdge(10, 11);
        newGraph = &graph;
        //
        Interface::outSplitter();
        outDirByGraph(newGraph, 3);
        outDirByGraph(newGraph, 5);
        outDirByGraph(newGraph, 1);
        outDirByGraph(newGraph, 2);
        std::cout << "Count vertices: " << newGraph.VerticesCount() << std::endl;
        Interface::outSplitter();
        newGraph.AddEdge(5,12); // Check asynchronous
        outDirByGraph(newGraph, 12);
        std::cout << "Count vertices: " << newGraph.VerticesCount() << std::endl;
    }

    return 0;

}
