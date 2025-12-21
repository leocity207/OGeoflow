#ifndef DCEL_FACE_H
#define DCEL_FACE_H


// UTILS
#include <utils/unowned_ptr.h>

namespace O::DCEL
{
	struct Half_Edge;

	/**
	 * @brief A face is way to create half_edge cycle inside the DCEL gaph.
	 *        In our case face are tightly linked to Polygone of the GeoJSON
	 */
	struct Face
	{
		O::Unowned_Ptr<Half_Edge> edge = nullptr;    ///< a representative halfedge belonging to this face
		size_t associated_feature_index = ~0ul;             ///< indices of GeoJSON features that reference this face
		O::Unowned_Ptr<Face> outer_face = nullptr;
	};

} // namespace DCEL

#endif // DCEL_FACE_H
