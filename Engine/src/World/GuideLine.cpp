#include "stdafx.h"
#include "GuideLine.h"

void reality::GuideLine::Init(GuideType guide_type)
{
	guide_type_ = guide_type;
}

void reality::GuideLine::AddNode(XMVECTOR node_pos)
{
	UINT index = line_nodes.size();
	line_nodes.insert(make_pair(index, node_pos));
}

void reality::GuideLine::DeleteNode(UINT index)
{
	line_nodes.erase(line_nodes.find(index));
}

void reality::GuideLine::UpdateLines()
{
	if (line_nodes.size() < 2)
		return;

	line_vectors.clear();
	for (UINT i = 1; i < line_nodes.size(); ++i)
	{
		XMVECTOR vector = line_nodes.at(i) - line_nodes.at(i - 1);
		line_vectors.push_back(vector);
	}
}
