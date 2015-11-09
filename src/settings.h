#ifndef SETTINGS
#define SETTINGS

#include <osg/Vec4>

/*
 * SETTINGS is a configuration variables file for Dureu3d
 * Contains settings such as
 * - default color schemes
 * - default entoty sizes
 * - etc (more to add)
 *
 * Victoria Rudakova 2015 <victoria.rudakova@yale.edu>
*/

namespace dureu{

const osg::Vec4 CANVAS_CLR_CURRENT = osg::Vec4(0.0f,0.0f,0.0f,1.0f);

const osg::Vec4 AXES_CLR_X = osg::Vec4(float(38)/255.0f,float(139)/255.0f,float(210)/255.0f,1.0f); // solarized blue
const osg::Vec4 AXES_CLR_Y = osg::Vec4(float(133)/255.0f,float(153)/255.0f,float(0)/255.0f,1.0f); // solarized green
const osg::Vec4 AXES_CLR_Z = osg::Vec4(float(42)/255.0f,float(161)/255.0f,float(152)/255.0f,1.0f); // solarized cyan


} // namespace dura

#endif // SETTINGS

