#include "memory.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QJSValue>

namespace lv{

Memory::Memory(QObject *parent)
    : QObject(parent)
{
}

Memory::~Memory(){
}

void Memory::gc(){
    QJSValue gcFn = ViewContext::instance().engine()->engine()->globalObject().property("gc");
    gcFn.call();
}

void Memory::reloc(){
    gc();
}

}// namespace
