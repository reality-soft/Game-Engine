#include "stdafx.h"
#include "Mesh.h"

using namespace reality;

Skeleton::Skeleton(const Skeleton& other)
{
	bind_pose_matrices = other.bind_pose_matrices;
}

SkeletalMesh::SkeletalMesh(const SkeletalMesh& other)
{
	meshes.resize(other.meshes.size());
	meshes = other.meshes;

	skeleton = other.skeleton;
}

StaticMesh::StaticMesh(const StaticMesh& other)
{
	meshes.resize(other.meshes.size());
	meshes = other.meshes;
}