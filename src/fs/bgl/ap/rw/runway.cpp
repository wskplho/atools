/*
 * Runway.cpp
 *
 *  Created on: 20.04.2015
 *      Author: alex
 */

#include "runway.h"

#include "fs/bgl/recordtypes.h"
#include "fs/bgl/converter.h"
#include "io/binarystream.h"

namespace atools {
namespace fs {
namespace bgl {

using atools::io::BinaryStream;

QString Runway::runwayMarkingsToStr(rw::RunwayMarkings flags)
{
  QString retval;
  if((flags & rw::EDGES) == rw::EDGES)
    retval += "EDGES,";
  if((flags & rw::THRESHOLD) == rw::THRESHOLD)
    retval += "THRESHOLD,";
  if((flags & rw::FIXED_DISTANCE) == rw::FIXED_DISTANCE)
    retval += "FIXED_DISTANCE,";
  if((flags & rw::TOUCHDOWN) == rw::TOUCHDOWN)
    retval += "TOUCHDOWN,";
  if((flags & rw::DASHES) == rw::DASHES)
    retval += "DASHES,";
  if((flags & rw::IDENT) == rw::IDENT)
    retval += "IDENT,";
  if((flags & rw::PRECISION) == rw::PRECISION)
    retval += "PRECISION,";
  if((flags & rw::EDGE_PAVEMENT) == rw::EDGE_PAVEMENT)
    retval += "EDGE_PAVEMENT,";
  if((flags & rw::SINGLE_END) == rw::SINGLE_END)
    retval += "SINGLE_END,";
  if((flags & rw::PRIMARY_CLOSED) == rw::PRIMARY_CLOSED)
    retval += "PRIMARY_CLOSED,";
  if((flags & rw::SECONDARY_CLOSED) == rw::SECONDARY_CLOSED)
    retval += "SECONDARY_CLOSED,";
  if((flags & rw::PRIMARY_STOL) == rw::PRIMARY_STOL)
    retval += "PRIMARY_STOL,";
  if((flags & rw::SECONDARY_STOL) == rw::SECONDARY_STOL)
    retval += "SECONDARY_STOL,";
  if((flags & rw::ALTERNATE_THRESHOLD) == rw::ALTERNATE_THRESHOLD)
    retval += "ALTERNATE_THRESHOLD,";
  if((flags & rw::ALTERNATE_FIXEDDISTANCE) == rw::ALTERNATE_FIXEDDISTANCE)
    retval += "ALTERNATE_FIXEDDISTANCE,";
  if((flags & rw::ALTERNATE_TOUCHDOWN) == rw::ALTERNATE_TOUCHDOWN)
    retval += "ALTERNATE_TOUCHDOWN,";
  if((flags & rw::ALTERNATE_PRECISION) == rw::ALTERNATE_PRECISION)
    retval += "ALTERNATE_PRECISION,";
  if((flags & rw::LEADING_ZERO_IDENT) == rw::LEADING_ZERO_IDENT)
    retval += "LEADING_ZERO_IDENT,";
  if((flags & rw::NO_THRESHOLD_END_ARROWS) == rw::NO_THRESHOLD_END_ARROWS)
    retval += "NO_THRESHOLD_END_ARROWS,";
  return retval;
}

QString Runway::lightToStr(rw::Light type)
{
  switch(type)
  {
    case rw::NO_LIGHT:
      return "NONE";

    case rw::LOW:
      return "LOW";

    case rw::MEDIUM:
      return "MEDIUM";

    case rw::HIGH:
      return "HIGH";
  }
  qWarning().nospace().noquote() << "Unknown runway lights type " << type;
  return "";
}

QString Runway::surfaceToStr(rw::Surface surface)
{
  switch(surface)
  {
    case rw::CONCRETE:
      return "CONCRETE";

    case rw::GRASS:
      return "GRASS";

    case rw::WATER:
      return "WATER";

    case rw::ASPHALT:
      return "ASPHALT";

    case rw::CLAY:
      return "CLAY";

    case rw::SNOW:
      return "SNOW";

    case rw::ICE:
      return "ICE";

    case rw::DIRT:
      return "DIRT";

    case rw::CORAL:
      return "CORAL";

    case rw::GRAVEL:
      return "GRAVEL";

    case rw::OIL_TREATED:
      return "OIL_TREATED";

    case rw::STEEL_MATS:
      return "STEEL_MATS";

    case rw::BITUMINOUS:
      return "BITUMINOUS";

    case rw::BRICK:
      return "BRICK";

    case rw::MACADAM:
      return "MACADAM";

    case rw::PLANKS:
      return "PLANKS";

    case rw::SAND:
      return "SAND";

    case rw::SHALE:
      return "SHALE";

    case rw::TARMAC:
      return "TARMAC";

    case rw::UNKNOWN:
      return "UNKNOWN";
  }
  qWarning().nospace().noquote() << "Unknown runway surface type " << surface;
  return "";
}

Runway::Runway(BinaryStream *bs, const QString& airportIdent)
  : Record(bs)
{
  surface = static_cast<rw::Surface>(bs->readShort());
  primary.number = bs->readByte();
  primary.designator = bs->readByte();
  secondary.number = bs->readByte();
  secondary.designator = bs->readByte();
  bs->skip(8);
  // primary.ilsIdent = converter::intToIcao(bs->readInt(), true);
  // secondary.ilsIdent = converter::intToIcao(bs->readInt(), true);

  position = BglPosition(bs, 1000.f);

  length = bs->readFloat();
  width = bs->readFloat();
  heading = bs->readFloat();
  patternAltitude = bs->readFloat();

  markingFlags = bs->readUShort();
  if((markingFlags & rw::PRIMARY_CLOSED) == rw::PRIMARY_CLOSED)
    primary.closedMarkings = true;
  if((markingFlags & rw::SECONDARY_CLOSED) == rw::SECONDARY_CLOSED)
    secondary.closedMarkings = true;
  if((markingFlags & rw::PRIMARY_STOL) == rw::PRIMARY_STOL)
    primary.stolMarkings = true;
  if((markingFlags & rw::SECONDARY_STOL) == rw::SECONDARY_STOL)
    secondary.stolMarkings = true;

  lightFlags = bs->readUByte();
  edgeLight = static_cast<rw::Light>(lightFlags & rw::EDGE_MASK);
  centerLight = static_cast<rw::Light>((lightFlags & rw::CENTER_MASK) >> 2);
  centerRed = (lightFlags & rw::CENTER_RED) == rw::CENTER_RED;

  patternFlags = bs->readUByte();
  if((patternFlags & rw::PRIMARY_TAKEOFF) == 0)
    primary.takeoff = true;
  if((patternFlags & rw::PRIMARY_LANDING) == 0)
    primary.landing = true;
  if((patternFlags & rw::PRIMARY_PATTERN_RIGHT) == rw::PRIMARY_PATTERN_RIGHT)
    primary.pattern = rw::RIGHT;
  else
    primary.pattern = rw::LEFT;
  if((patternFlags & rw::SECONDARY_TAKEOFF) == 0)
    secondary.takeoff = true;
  if((patternFlags & rw::SECONDARY_LANDING) == 0)
    secondary.landing = true;
  if((patternFlags & rw::SECONDARY_PATTERN_RIGHT) == rw::SECONDARY_PATTERN_RIGHT)
    secondary.pattern = rw::RIGHT;
  else
    secondary.pattern = rw::LEFT;

  while(bs->tellg() < startOffset + size)
  {
    Record r(bs);
    rec::RunwayRecordType t = r.getId<rec::RunwayRecordType>();

    switch(t)
    {
      case rec::OFFSET_THRESHOLD_PRIM:
        primary.offsetThreshold = readRunwayExtLength();
        break;
      case rec::OFFSET_THRESHOLD_SEC:
        secondary.offsetThreshold = readRunwayExtLength();
        break;
      case rec::BLAST_PAD_PRIM:
        primary.blastPad = readRunwayExtLength();
        break;
      case rec::BLAST_PAD_SEC:
        secondary.blastPad = readRunwayExtLength();
        break;
      case rec::OVERRUN_PRIM:
        primary.overrun = readRunwayExtLength();
        break;
      case rec::OVERRUN_SEC:
        secondary.overrun = readRunwayExtLength();
        break;
      case rec::VASI_PRIM_LEFT:
        r.seekToStart();
        primary.leftVasi = RunwayVasi(bs);
        break;
      case rec::VASI_PRIM_RIGHT:
        r.seekToStart();
        primary.rightVasi = RunwayVasi(bs);
        break;
      case rec::VASI_SEC_LEFT:
        r.seekToStart();
        secondary.leftVasi = RunwayVasi(bs);
        break;
      case rec::VASI_SEC_RIGHT:
        r.seekToStart();
        secondary.rightVasi = RunwayVasi(bs);
        break;
      case rec::APP_LIGHTS_PRIM:
        r.seekToStart();
        primary.approachLights = RunwayAppLights(bs);
        break;
      case rec::APP_LIGHTS_SEC:
        r.seekToStart();
        secondary.approachLights = RunwayAppLights(bs);
        break;
      default:
        qWarning().nospace().noquote() << "Unexpected record type in Runway record 0x" << hex << t << dec
                                       << " for ident " << airportIdent
                                       << " runway " << primary.getName() << "/" << secondary.getName();
    }
    r.seekToEnd();
  }
}

int Runway::readRunwayExtLength()
{
  bs->readShort();
  int l = static_cast<int>(bs->readFloat());
  bs->readFloat();
  return l;
}

QDebug operator<<(QDebug out, const Runway& record)
{
  QDebugStateSaver saver(out);

  out.nospace().noquote() << static_cast<const Record&>(record)
  << " Runway[length " << record.length
  << ", width " << record.width
  << ", hdg " << record.heading
  << ", surface " << Runway::surfaceToStr(record.surface) << endl
  << ", primary " << record.primary << endl
  << ", secondary " << record.secondary << endl
  << "]";
  return out;
}

Runway::~Runway()
{
}

} // namespace bgl
} // namespace fs
} // namespace atools
