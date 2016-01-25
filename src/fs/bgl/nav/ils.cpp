/*
 * Ils.cpp
 *
 *  Created on: 25.04.2015
 *      Author: alex
 */

#include "fs/bgl/nav/ils.h"
#include "fs/bgl/nav/localizer.h"
#include "fs/bgl/nav/dme.h"
#include "fs/bgl/nav/glideslope.h"

#include "fs/bgl/converter.h"
#include "fs/bgl/recordtypes.h"
#include "io/binarystream.h"

namespace atools {
namespace fs {
namespace bgl {

using atools::io::BinaryStream;

enum IlsFlags
{
  FLAGS_DME_ONLY = 1 << 0,
  FLAGS_BC = 1 << 1,
  FLAGS_GS = 1 << 2,
  FLAGS_DME = 1 << 3,
  FLAGS_NAV = 1 << 4
              // bit 0: if 0 then DME only, otherwise 1 for ILS
              // bit 2: backcourse (0 = false, 1 = true)
              // bit 3: glideslope present
              // bit 4: DME present
              // bit 5: NAV true
};

Ils::Ils(BinaryStream *bs)
  : NavBase(bs), localizer(nullptr), glideslope(nullptr), dme(nullptr)
{
  bs->readByte();
  int flags = bs->readByte();

  // dmeOnlyOrIls = (flags & FLAGS_DME_ONLY) == FLAGS_DME_ONLY;
  isBackcourse = (flags & FLAGS_BC) == FLAGS_BC;
  // hasGlideslope = (flags & FLAGS_GS) == FLAGS_GS;
  // hasDme = (flags & FLAGS_DME) == FLAGS_DME;
  // hasNav = (flags & FLAGS_NAV) == FLAGS_NAV;

  position = BglPosition(bs, 1000.f);
  frequency = bs->readInt() / 1000;
  range = bs->readFloat();
  magVar = bs->readFloat();

  ident = converter::intToIcao(bs->readInt());

  unsigned int regionFlags = bs->readInt();
  region = converter::intToIcao(regionFlags & 0x7ff, true);
  airportIdent = converter::intToIcao((regionFlags >> 11) & 0x1fffff, true);

  while(bs->tellg() < startOffset + size)
  {
    Record r(bs);
    rec::IlsVorRecordType t = r.getId<rec::IlsVorRecordType>();

    switch(t)
    {
      case rec::ILS_VOR_NAME:
        name = bs->readString(r.getSize() - 6);
        break;
      case rec::LOCALIZER:
        r.seekToStart();
        localizer = new Localizer(bs);
        break;
      case rec::GLIDESLOPE:
        r.seekToStart();
        glideslope = new Glideslope(bs);
        break;
      case rec::DME:
        r.seekToStart();
        dme = new Dme(bs);
        break;
      default:
        qWarning().nospace().noquote() << "Unexpected record type in ILS record 0x"
                                       << hex << t << dec << " for ident "
                                       << ident;
    }
    r.seekToEnd();
  }
}

Ils::~Ils()
{
  delete localizer;
  delete dme;
  delete glideslope;
}

QDebug operator<<(QDebug out, const Ils& record)
{
  QDebugStateSaver saver(out);

  out.nospace().noquote() << static_cast<const NavBase&>(record)
  << " Ils[isBackcourse " << record.isBackcourse;
  if(record.localizer != nullptr)
    out << ", " << *record.localizer;
  if(record.glideslope != nullptr)
    out << ", " << *record.glideslope;
  if(record.dme != nullptr)
    out << ", " << *record.dme;
  out << "]";
  return out;
}

} // namespace bgl
} // namespace fs
} // namespace atools
