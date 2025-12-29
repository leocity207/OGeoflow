#ifndef DCEL_FACE_H
#define DCEL_FACE_H


// UTILS
#include <utils/unowned_ptr.h>

namespace O::DCEL
{

	/**
	 * @brief A face is way to create half_edge cycle inside the DCEL gaph.
	 *        In our case face are tightly linked to Polygone of the GeoJSON
	 */
	template<class Half_Edge>
	struct Face
	{
		O::Unowned_Ptr<Half_Edge> edge = nullptr;    ///< a representative halfedge belonging to this face
	};

} // namespace DCEL

#endif // DCEL_FACE_H
