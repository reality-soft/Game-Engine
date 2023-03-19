#include "stdafx.h"
#include "GuideLine.h"

void reality::GuideLine::Init(GuideType guide_type)
{
	guide_type_ = guide_type;
}

void reality::GuideLine::AddNode(XMVECTOR node_pos)
{
	UINT index = line_nodes.size();
	if (line_nodes.empty() == false)
	{
		line_vectors.push_back(node_pos - line_nodes.find(index - 1)->second);
	}
	line_nodes.insert(make_pair(index, node_pos));
}

void reality::GuideLine::DeleteNode(UINT index)
{
	line_nodes.erase(line_nodes.find(index));
}