/*
 * RunwayEnd.cpp
 *
 *  Created on: 23.04.2015
 *      Author: alex
 */

#include "fs/bgl/ap/rw/runwayend.h"
#include "fs/bgl/converter.h"

namespace atools {
namespace fs {
namespace bgl {

using atools::io::BinaryStream;

QString RunwayEnd::patternToStr(rw::Pattern pattern)
{
  switch(pattern)
  {
    case rw::LEFT:
      return "LEFT";

    case rw::RIGHT:
      return "RIGHT";
  }
  qWarning().nospace().noquote() << "Unknown runway pattern type " << pattern;
  return "";
}

QDebug operator<<(QDebug out, const RunwayEnd& record)
{
  QDebugStateSaver saver(out);

  out.nospace().noquote() << " RunwayEnd[Name " << record.getName()
  << ", left " << record.leftVasi
  << ", right " << record.rightVasi
  << ", " << record.approachLights
  << "]";
  return out;
}

RunwayEnd::~RunwayEnd()
{
}

QString RunwayEnd::getName() const
{
  return converter::runwayToStr(number, designator);
}

} // namespace bgl
} // namespace fs
} // namespace atools
