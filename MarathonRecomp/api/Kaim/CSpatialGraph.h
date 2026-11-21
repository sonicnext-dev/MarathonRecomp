#pragma once

#include <Marathon.inl>
#include <Kaim/CConstraint.h>
#include <Kaim/CEdge.h>
#include <Kaim/CHeuristic.h>
#include <Kaim/CObject.h>
#include <Kaim/CPathObject.h>
#include <Kaim/CVertex.h>

namespace Kaim
{
    class CSpatialGraph : public CObject
    {
    public:
        be<uint32_t> m_optionalDataMask;
        be<float> m_coverageDist;
        be<uint32_t> m_lastVertexId;
        be<uint32_t> m_vertexSize;
        be<uint32_t> m_vertexCount;
        xpointer<CVertex> m_vertices;
        xpointer<xpointer<CEdge>> m_vertexAstarPredecesors;
        xpointer<be<uint32_t>> m_vertexFirstInEdges;
        xpointer<be<uint32_t>> m_vertexFirstOutEdges;
        be<uint32_t> m_lastEdgeId;
        be<uint32_t> m_edgeSize;
        be<uint32_t> m_edgeCount;
        xpointer<CEdge> m_edges;
        xpointer<be<uint32_t>> m_startVertices;
        xpointer<be<uint32_t>> m_endVertices;
        xpointer<be<uint32_t>> m_nextInEdge;
        xpointer<be<uint32_t>> m_nextOutEdge;
        xpointer<be<float>> m_edgesLength;
        xpointer<be<float>> m_edgesCost;
        xpointer<xpointer<CPathObject>> m_edgesPathObjects;
        xpointer<CConstraint> m_constraint;
        xpointer<CHeuristic> m_heuristic;
        bool m_CustomAllocation;
    };

    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_optionalDataMask, 0x04);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_coverageDist, 0x08);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_lastVertexId, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_vertexSize, 0x10);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_vertexCount, 0x14);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_vertices, 0x18);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_vertexAstarPredecesors, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_vertexFirstInEdges, 0x20);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_vertexFirstOutEdges, 0x24);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_lastEdgeId, 0x28);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_edgeSize, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_edgeCount, 0x30);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_edges, 0x34);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_startVertices, 0x38);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_endVertices, 0x3C);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_nextInEdge, 0x40);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_nextOutEdge, 0x44);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_edgesLength, 0x48);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_edgesCost, 0x4C);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_edgesPathObjects, 0x50);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_constraint, 0x54);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_heuristic, 0x58);
    MARATHON_ASSERT_OFFSETOF(CSpatialGraph, m_CustomAllocation, 0x5C);
    MARATHON_ASSERT_SIZEOF(CSpatialGraph, 0x60);
}
