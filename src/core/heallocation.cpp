#include "heallocation.h"
#include "config.h"

HealLocation::HealLocation(QString id, QString map, int i, uint16_t x, uint16_t y)
{
    this->idName = id;
    this->mapName = map;
    this->index = i;
    this->x = x;
    this->y = y;
    this->respawnMap = "";
    this->respawnNPC = 0;
}

HealLocation::HealLocation(QString id, QString map, int i, uint16_t x, uint16_t y, QString respawnMap, uint16_t respawnNPC)
{
    this->idName = id;
    this->mapName = map;
    this->index = i;
    this->x = x;
    this->y = y;
    this->respawnMap = respawnMap;
    this->respawnNPC = respawnNPC;
}

HealLocation HealLocation::fromEvent(Event *event)
{
    HealLocation hl;
    hl.idName = event->get("id_name");
    hl.mapName = event->get("loc_name");
    try {
        hl.index = event->get("index").toInt();
    }
    catch(...) {
        hl.index = 0;
    }
    hl.x     = event->getU16("x");
    hl.y     = event->getU16("y");
    if (projectConfig.getBaseGameVersion() == BaseGameVersion::pokefirered) {
        hl.respawnNPC = event->getU16("respawn_npc");
        hl.respawnMap = event->get("respawn_map");
    }
    return hl;
}

QDebug operator<<(QDebug debug, const HealLocation &hl)
{
    debug << "HealLocation_" + hl.mapName << "(" << hl.x << ',' << hl.y << ")";
    return debug;
}
