#include "stdafx.h"
#include "FbxOutData.h"

namespace reality
{
	void IndexWeight::insert(int index_, float weight_)
	{
		for (int i = 0; i < 8; i++)
		{
			if (weight_ > weight[i])
			{
				for (int j = 7; j > i; --j)
				{
					index[j] = index[j - 1];
					weight[j] = weight[j - 1];
				}
				index[i] = index_;
				weight[i] = weight_;
				break;
			}
		}
	}
}