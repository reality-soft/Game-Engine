#include "stdafx.h"
#include "GuideLine.h"
#include "FileTransfer.h"

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

void reality::GuideLine::Import(string mapdat_file, GuideType type)
{
	FileTransfer in_mapdata(mapdata_dir + mapdat_file, READ);

	vector<GuideLine> new_guide_lines_;

	UINT num_guide_lines = 0;
	in_mapdata.ReadBinary<UINT>(num_guide_lines);
	for (UINT i = 0; i < num_guide_lines; ++i)
	{
		GuideLine new_guide_line;
		new_guide_line.guide_type_ = type;

		UINT num_nodes = 0;
		in_mapdata.ReadBinary<UINT>(num_nodes);

		for (UINT j = 0; j < num_nodes; ++j)
		{
			UINT node_number;
			XMVECTOR node_pos;
			in_mapdata.ReadBinary<UINT>(node_number);
			in_mapdata.ReadBinary<XMVECTOR>(node_pos);

			new_guide_line.AddNode(node_pos);
		}

		new_guide_lines_.push_back(new_guide_line);
	}

	if (new_guide_lines_.empty() == false)
		*this = new_guide_lines_.front();
}
