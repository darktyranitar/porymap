#include "editcommands.h"
#include "mappixmapitem.h"
#include "draggablepixmapitem.h"
#include "bordermetatilespixmapitem.h"
#include "editor.h"

#include <QDebug>



void renderMapBlocks(Map *map, bool ignoreCache = false) {
    map->mapItem->draw(ignoreCache);
    map->collisionItem->draw(ignoreCache);
}
PaintMetatile::PaintMetatile(Map *map,
    Blockdata *oldMetatiles, Blockdata *newMetatiles,
    unsigned actionId, QUndoCommand *parent) : QUndoCommand(parent) {
    setText("Paint Metatiles");

    this->map = map;
    this->oldMetatiles = oldMetatiles;
    this->newMetatiles = newMetatiles;

    this->actionId = actionId;
}

PaintMetatile::~PaintMetatile() {
    if (newMetatiles) delete newMetatiles;
    if (oldMetatiles) delete oldMetatiles;
}

void PaintMetatile::redo() {
    QUndoCommand::redo();

    if (!map) return;

    if (map->layout->blockdata) {
        map->layout->blockdata->copyFrom(newMetatiles);
    }

    renderMapBlocks(map);
}

void PaintMetatile::undo() {
    if (!map) return;

    if (map->layout->blockdata) {
        map->layout->blockdata->copyFrom(oldMetatiles);
    }

    renderMapBlocks(map);

    QUndoCommand::undo();
}

bool PaintMetatile::mergeWith(const QUndoCommand *command) {
    const PaintMetatile *other = static_cast<const PaintMetatile *>(command);

    if (this->map != other->map)
        return false;

    if (actionId != other->actionId)
        return false;

    this->newMetatiles->copyFrom(other->newMetatiles);

    return true;
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

PaintBorder::PaintBorder(Map *map,
    Blockdata *oldBorder, Blockdata *newBorder,
    unsigned actionId, QUndoCommand *parent) : QUndoCommand(parent) {
    setText("Paint Border");

    this->map = map;
    this->oldBorder = oldBorder;
    this->newBorder = newBorder;

    this->actionId = actionId;
}

PaintBorder::~PaintBorder() {
    if (newBorder) delete newBorder;
    if (oldBorder) delete oldBorder;
}

void PaintBorder::redo() {
    QUndoCommand::redo();

    if (!map) return;

    if (map->layout->border) {
        map->layout->border->copyFrom(newBorder);
    }

    map->borderItem->draw();
}

void PaintBorder::undo() {
    if (!map) return;

    if (map->layout->border) {
        map->layout->border->copyFrom(oldBorder);
    }

    map->borderItem->draw();

    QUndoCommand::undo();
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

BucketFillMetatile::BucketFillMetatile(Map *map,
    Blockdata *oldMetatiles, Blockdata *newMetatiles,
    unsigned actionId, QUndoCommand *parent)
        : PaintMetatile(map, oldMetatiles, newMetatiles, actionId, parent) {
    setText("Bucket Fill Metatiles");
}

BucketFillMetatile::~BucketFillMetatile() {
    PaintMetatile::~PaintMetatile();
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

MagicFillMetatile::MagicFillMetatile(Map *map,
    Blockdata *oldMetatiles, Blockdata *newMetatiles,
    unsigned actionId, QUndoCommand *parent)
        : PaintMetatile(map, oldMetatiles, newMetatiles, actionId, parent) {
    setText("Magic Fill Metatiles");
}

MagicFillMetatile::~MagicFillMetatile() {
    PaintMetatile::~PaintMetatile();
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

ShiftMetatiles::ShiftMetatiles(Map *map,
    Blockdata *oldMetatiles, Blockdata *newMetatiles,
    unsigned actionId, QUndoCommand *parent) : QUndoCommand(parent) {
    setText("Shift Metatiles");

    this->map = map;
    this->oldMetatiles = oldMetatiles;
    this->newMetatiles = newMetatiles;

    this->actionId = actionId;
}

ShiftMetatiles::~ShiftMetatiles() {
    if (newMetatiles) delete newMetatiles;
    if (oldMetatiles) delete oldMetatiles;
}

void ShiftMetatiles::redo() {
    QUndoCommand::redo();

    if (!map) return;

    if (map->layout->blockdata) {
        map->layout->blockdata->copyFrom(newMetatiles);
    }

    renderMapBlocks(map, true);
}

void ShiftMetatiles::undo() {
    if (!map) return;

    if (map->layout->blockdata) {
        map->layout->blockdata->copyFrom(oldMetatiles);
    }

    renderMapBlocks(map, true);

    QUndoCommand::undo();
}

bool ShiftMetatiles::mergeWith(const QUndoCommand *command) {
    const ShiftMetatiles *other = static_cast<const ShiftMetatiles *>(command);

    if (this->map != other->map)
        return false;

    if (actionId != other->actionId)
        return false;

    this->newMetatiles->copyFrom(other->newMetatiles);

    return true;
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

ResizeMap::ResizeMap(Map *map, QSize oldMapDimensions, QSize newMapDimensions,
    Blockdata *oldMetatiles, Blockdata *newMetatiles,
    QSize oldBorderDimensions, QSize newBorderDimensions,
    Blockdata *oldBorder, Blockdata *newBorder,
    QUndoCommand *parent) : QUndoCommand(parent) {
    setText("Resize Map");

    this->map = map;

    this->oldMapWidth = oldMapDimensions.width();
    this->oldMapHeight = oldMapDimensions.height();

    this->newMapWidth = newMapDimensions.width();
    this->newMapHeight = newMapDimensions.height();

    this->oldMetatiles = oldMetatiles;
    this->newMetatiles = newMetatiles;

    this->oldBorderWidth = oldBorderDimensions.width();
    this->oldBorderHeight = oldBorderDimensions.height();

    this->newBorderWidth = newBorderDimensions.width();
    this->newBorderHeight = newBorderDimensions.height();

    this->oldBorder = oldBorder;
    this->newBorder = newBorder;
}

ResizeMap::~ResizeMap() {
    if (newMetatiles) delete newMetatiles;
    if (oldMetatiles) delete oldMetatiles;
}

void ResizeMap::redo() {
    QUndoCommand::redo();

    if (!map) return;

    if (map->layout->blockdata) {
        map->layout->blockdata->copyFrom(newMetatiles);
        map->setDimensions(newMapWidth, newMapHeight, false);
    }

    if (map->layout->border) {
        map->layout->border->copyFrom(newBorder);
        map->setBorderDimensions(newBorderWidth, newBorderHeight, false);
    }

    map->mapNeedsRedrawing();
}

void ResizeMap::undo() {
    if (!map) return;

    if (map->layout->blockdata) {
        map->layout->blockdata->copyFrom(oldMetatiles);
        map->setDimensions(oldMapWidth, oldMapHeight, false);
    }

    if (map->layout->border) {
        map->layout->border->copyFrom(oldBorder);
        map->setBorderDimensions(oldBorderWidth, oldBorderHeight, false);
    }

    map->mapNeedsRedrawing();

    QUndoCommand::undo();
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

EventMove::EventMove(QList<Event *> events,
    int deltaX, int deltaY, unsigned actionId,
    QUndoCommand *parent) : QUndoCommand(parent) {
    setText("Move Event");

    this->events = events;

    this->deltaX = deltaX;
    this->deltaY = deltaY;

    this->actionId = actionId;
}

EventMove::~EventMove() {}

void EventMove::redo() {
    QUndoCommand::redo();

    for (Event *event : events) {
        event->pixmapItem->move(deltaX, deltaY);
    }
}

void EventMove::undo() {
    for (Event *event : events) {
        event->pixmapItem->move(-deltaX, -deltaY);
    }

    QUndoCommand::undo();
}

bool EventMove::mergeWith(const QUndoCommand *command) {
    const EventMove *other = static_cast<const EventMove *>(command);

    if (actionId != other->actionId)
        return false;

    // TODO: check that same events as well?

    this->deltaX += other->deltaX;
    this->deltaY += other->deltaY;

    return true;
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

EventShift::EventShift(QList<Event *> events,
    int deltaX, int deltaY, unsigned actionId,
    QUndoCommand *parent) 
  : EventMove(events, deltaX, deltaY, actionId, parent) {
    setText("Shift Events");
}

EventShift::~EventShift() {}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

EventCreate::EventCreate(Editor *editor, Map *map, Event *event,
    QUndoCommand *parent) : QUndoCommand(parent) {
    //
    setText("Create Event");

    this->editor = editor;

    this->map = map;
    this->event = event;
}

EventCreate::~EventCreate() {}

void EventCreate::redo() {
    QUndoCommand::redo();

    map->addEvent(event);
    
    editor->project->loadEventPixmaps(map->getAllEvents());
    editor->addMapEvent(event);

    map->objectsChanged();

    // select this event
    editor->selected_events->clear();
    editor->selectMapEvent(event->pixmapItem, false);
}

void EventCreate::undo() {
    map->removeEvent(event);

    if (editor->scene->items().contains(event->pixmapItem)) {
        editor->scene->removeItem(event->pixmapItem);
    }
    editor->selected_events->removeOne(event->pixmapItem);

    editor->updateSelectedEvents();

    map->objectsChanged();

    QUndoCommand::undo();
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

EventDelete::EventDelete(Editor *editor, Map *map,
    QList<Event *> selectedEvents, Event *nextSelectedEvent,
    QUndoCommand *parent) : QUndoCommand(parent) {
    if (selectedEvents.size() > 1) {
        setText("Delete Events");
    } else {
        setText("Delete Event");
    }

    this->editor = editor;
    this->map = map;

    this->selectedEvents = selectedEvents;
    this->nextSelectedEvent = nextSelectedEvent;
}

EventDelete::~EventDelete() {}

void EventDelete::redo() {
    QUndoCommand::redo();

    for (Event *event : selectedEvents) {
        map->removeEvent(event);

        if (editor->scene->items().contains(event->pixmapItem)) {
            editor->scene->removeItem(event->pixmapItem);
        }
        editor->selected_events->removeOne(event->pixmapItem);
    }

    map->objectsChanged();

    editor->selected_events->clear();
    editor->selected_events->append(nextSelectedEvent->pixmapItem);
    editor->updateSelectedEvents();
}

void EventDelete::undo() {
    for (Event *event : selectedEvents) {
        map->addEvent(event);
    
        editor->project->loadEventPixmaps(map->getAllEvents());
        editor->addMapEvent(event);
    }

    map->objectsChanged();

    // select these events
    editor->selected_events->clear();
    for (Event *event : selectedEvents) {
        editor->selected_events->append(event->pixmapItem);
    }
    editor->updateSelectedEvents();

    QUndoCommand::undo();
}

/******************************************************************************
    ************************************************************************
 ******************************************************************************/

EventDuplicate::EventDuplicate(Editor *editor, Map *map,
    QList<Event *> selectedEvents,
    QUndoCommand *parent) : QUndoCommand(parent) {
    //
    setText("Duplicate Event");

    this->editor = editor;

    this->map = map;
    this->selectedEvents = selectedEvents;
}

EventDuplicate::~EventDuplicate() {}

void EventDuplicate::redo() {
    QUndoCommand::redo();

    for (Event *event : selectedEvents) {
        map->addEvent(event);
    }

    editor->project->loadEventPixmaps(map->getAllEvents());

    for (Event *event : selectedEvents) {
        editor->addMapEvent(event);
    }

    map->objectsChanged();

    // select these events
    editor->selected_events->clear();
    for (Event *event : selectedEvents) {
        editor->selected_events->append(event->pixmapItem);
    }
    editor->updateSelectedEvents();
}

void EventDuplicate::undo() {
    for (Event *event : selectedEvents) {
        map->removeEvent(event);

        if (editor->scene->items().contains(event->pixmapItem)) {
            editor->scene->removeItem(event->pixmapItem);
        }
        editor->selected_events->removeOne(event->pixmapItem);
    }

    map->objectsChanged();

    QUndoCommand::undo();
}