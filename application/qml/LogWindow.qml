/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Window {
    id : root
    width: 400
    height: 200
    title: "Livekeys Log"
    color : "#081017"

    property alias contentWidth : container.width
    property alias contentHeight : container.height

    property Item content : null

    signal itemAdded()

    Item{
        id: container
        anchors.fill: parent
        children: root.content ? [root.content] : []
    }

}
