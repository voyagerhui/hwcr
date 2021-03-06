#pragma once

#include <queue>
#include <vector>

#include <assert.h>

using std::vector;
using std::priority_queue;

template <class Graph> class Dijkstra {
public:
    static vector<typename Graph::NodeIndex> unused_;

    Dijkstra(const Graph &graph, typename Graph::Dist limit = Graph::kInf,
             vector<typename Graph::NodeIndex> &last = unused_,
             vector<typename Graph::NodeIndex> &reached = unused_);
    ~Dijkstra() = default;

    Dijkstra(const Dijkstra &) = delete;
    Dijkstra &operator=(const Dijkstra &) = delete;

    vector<typename Graph::Dist> operator()(typename Graph::NodeIndex source);

private:
    const Graph &graph_;
    typename Graph::Dist limit_;
    vector<typename Graph::NodeIndex> &last_;
    const bool monLast_;
    vector<typename Graph::NodeIndex> &reached_;
    const bool monReached_;

}; // class Dijkstra

/*************************************define*************************************/

template <class Graph>
vector<typename Graph::NodeIndex> Dijkstra<Graph>::unused_;

template <class Graph>
Dijkstra<Graph>::Dijkstra(const Graph &graph, typename Graph::Dist limit,
                          vector<typename Graph::NodeIndex> &last,
                          vector<typename Graph::NodeIndex> &reached)
    : graph_(graph)
    , limit_(limit)
    , last_(last)
    , monLast_(last_.begin() != unused_.begin())
    , reached_(reached)
    , monReached_(&reached_ != &unused_)
{
}

template <class Graph>
vector<typename Graph::Dist>
Dijkstra<Graph>::operator()(typename Graph::NodeIndex source)
{
    assert(source < graph_.getOrder());
    assert(!monLast_ || last_.size() == graph_.getOrder());
    assert(!monReached_ || reached_.empty());

    vector<typename Graph::Dist> dists(graph_.getOrder(), Graph::kInf);

    const typename Graph::AdjList &adjList(graph_.getAdjList());
    const typename Graph::AdjMatrix &adjMatrix(graph_.getAdjMatrix());

    if (monReached_) {
        reached_.reserve(graph_.getOrder());
    }
    vector<bool> isVisited(adjList.size(), false);
    using PDI = std::pair<typename Graph::Dist, typename Graph::NodeIndex>;
    priority_queue<PDI, vector<PDI>, std::greater<PDI>> minHeap;

    if (monLast_) {
        last_[source] = source;
    }
    dists[source] = 0;
    minHeap.push(std::make_pair(0, source));

    while (!minHeap.empty()) {

        PDI top = minHeap.top();
        minHeap.pop();
        typename Graph::NodeIndex from = top.second;
        if (isVisited[from]) {
            continue;
        }
        isVisited[from] = true;
        if (monReached_) {
            reached_.push_back(from);
        }

        for (typename Graph::NodeIndex to : adjList[from]) {
            typename Graph::Dist newDist = adjMatrix[from][to] + dists[from];
            if (newDist <= limit_ && dists[to] > newDist) {
                dists[to] = newDist;
                if (monLast_) {
                    last_[to] = from;
                }
                if (isVisited[to] == false) {
                    minHeap.push(std::make_pair(dists[to], to));
                }
            }
        }
    }
    return dists;
}
