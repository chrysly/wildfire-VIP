//////////////////////////////////////////////////////////////////////////
// Max Grid
// Copyright (c) (2018-), Bo Zhu
// This file is part of SimpleX, whose distribution is governed by the LICENSE file.
//////////////////////////////////////////////////////////////////////////
#ifndef __MacGrid_h__
#define __MacGrid_h__
#include "Grid.h"

template<int d> class Face
{Typedef_VectorDi(d);
public:
	int axis;VectorDi face;
	Face(const int _axis=0,const VectorDi& _face=VectorDi::Zero()):axis(_axis),face(_face){}
};

template<int d> class alignas(256) MacGrid
{Typedef_VectorDii(d);
public:
	Grid<d> grid;
	VectorDi face_counts[d];
	Grid<d> face_grids[d];				//// the grid nodes of a face grid are collocated with the face centers

	VectorDi edge_counts[d];
	Grid<d> edge_grids[d];

	////Initializer
	MacGrid();
	MacGrid(const VectorDi& _cell_counts,const real _dx=(real)0,const VectorD& _domain_min=VectorD::Zero());
	MacGrid(const Grid<d>& _grid);
	MacGrid& operator=(const MacGrid& copy);
	void Initialize(const VectorDi& _cell_counts,const real _dx=(real)0,const VectorD& _domain_min=VectorD::Zero());
	void Initialize(const Grid<d>& _grid);
protected:
	void Initialize_Helper();
	void Initialize_Face_Counts();
	void Initialize_Face_Grids();
	void Initialize_Edge_Counts();
	void Initialize_Edge_Grids();
public:
	////Face operations
	bool Valid_Face(const int axis,const VectorDi& face) const;
	bool Is_Boundary_Face(const int axis,const VectorDi& face) const;
	bool Is_Boundary_Face(const int axis,const VectorDi& face,const int ring_num) const;
	bool Is_Axial_Boundary_Face(const int axis,const VectorDi& face) const;
	VectorD Face_Center(const int axis,const VectorDi& face) const;
	int Face_Index(const int axis,const VectorDi& face) const;
	VectorDi Face_Coord(const int axis,const int face) const;
	int Number_Of_Faces(const int axis) const;
	int Number_Of_Faces() const;

	////face-cell relation
	static inline VectorDi Face_Left_Cell(const int axis,const VectorDi& face){return face-VectorDi::Unit(axis);}
	static inline VectorDi Face_Right_Cell(const int axis,const VectorDi& face){return face;}
	static inline VectorDi Cell_Left_Face(const int axis,const VectorDi& cell){return cell;}
	static inline VectorDi Cell_Right_Face(const int axis,const VectorDi& cell){return cell+VectorDi::Unit(axis);}
	static bool Is_Face_Incident_To_Cell(const int axis,const VectorDi& face,const VectorDi& cell);
	static VectorDi Face_Incident_Cell(const int axis,const VectorDi& face,const int i);
	static VectorDi Cell_Incident_Face(const int axis,const VectorDi& cell,const int i);
	static void Cell_Incident_Face(const VectorDi& cell,const int i,int& axis,VectorDi& face);	////the order of the nb faces is consistent with the order of the nb cells in Nb_C() in Grid.h

	////face-node relation
	static constexpr int Number_Of_Node_Incident_Faces(){return d==2?4:12;}
	static Face<d> Node_Incident_Face(const VectorDi& node,const int i);
	static constexpr int Number_Of_Node_Incident_Faces_Per_Axis(){return d==2?2:4;}
	static VectorDi Node_Incident_Face_Per_Axis(const VectorDi& node,const int i,const int axis);
	static constexpr int Number_Of_Face_Incident_Nodes(){return Pow(2,d-1);}
	static VectorDi Face_Incident_Node(const int axis,const VectorDi& face,const int i);
	static void Face_Incident_Nodes(const int axis,const VectorDi& face,ArrayF2P<VectorDi,d-1>& face_nodes);
	static constexpr int Number_Of_Face_Incident_Cells(){return 2;}
	static constexpr int Number_Of_Cell_Incident_Faces(){return d==2?4:6;}

	////boundary
	
	////Edge operations
	bool Valid_Edge(const int axis,const VectorDi& edge) const;
	bool Is_Boundary_Edge(const int axis,const VectorDi& edge) const;
	// bool Is_Boundary_Edge(const int axis,const VectorDi& edge,const int ring_num) const;
	bool Is_Axial_Boundary_Edge(const int axis,const VectorDi& edge) const;
	VectorD Edge_Center(const int axis,const VectorDi& edge) const;
	int Edge_Index(const int axis,const VectorDi& edge) const;
	VectorDi Edge_Coord(const int axis,const int edge) const;
	int Number_Of_Edges(const int axis) const;
	int Number_Of_Edges() const;
	
	////edge-cell relation
	static constexpr int Number_Of_Edge_Incident_Cells(){return d==2?2:4;}
	static VectorDi Edge_Incident_Cell(const int axis,const VectorDi& edge,const int i);
	static constexpr int Number_Of_Edge_Incident_Nodes(){return 2;}
	static VectorDi Edge_Incident_Node(const int axis,const VectorDi& edge,const int i);
	static constexpr int Number_Of_Cell_Incident_Edges(){return d==2?2:4;}
	static VectorDi Cell_Incident_Edge(const int axis,const VectorDi& cell,const int i);

	////parallel iterators
	template<class FintVi>		//FintVi: void(const int axis, const VectorDi &face)
	void Exec_Faces(FintVi f) const
	{
		for (int axis = 0;axis < d;axis++) {
			int face_num = face_grids[axis].node_counts.prod();
			#pragma omp parallel for
			for (int i = 0;i < face_num;i++) {
				VectorDi face = face_grids[axis].Node_Coord(i);
				f(axis, face);}}
	}
};

////Macros for using GridIterator to traverse a MacGrid
#define iterate_face(axis,iter,mac_grid) \
for(int axis=0;axis<d;axis++)for(GridIterator<d> iter(mac_grid.face_grids[axis],GridIterator<d>::IteratorType::Node);iter.Valid();iter.Next())
#define iterate_face_in_one_dim(axis,iter,mac_grid) \
for(GridIterator<d> iter(mac_grid.face_grids[axis],GridIterator<d>::IteratorType::Node);iter.Valid();iter.Next())

#define iterate_edge(axis,iter,mac_grid) \
for(int axis=0;axis<d;axis++)for(GridIterator<d> iter(mac_grid.edge_grids[axis],GridIterator<d>::IteratorType::Node);iter.Valid();iter.Next())
#define iterate_edge_in_one_dim(axis,iter,mac_grid) \
for(GridIterator<d> iter(mac_grid.edge_grids[axis],GridIterator<d>::IteratorType::Node);iter.Valid();iter.Next())

////CODESAMPLE: openmp version of iterate_face
//for(int axis=0;axis<d;axis++){int face_num=mac_grid.Number_Of_Faces(axis);
//	#pragma omp parallel for
//	for(int i=0;i<face_num;i++){VectorDi face=mac_grid.Face_Coord(axis,i);

#endif