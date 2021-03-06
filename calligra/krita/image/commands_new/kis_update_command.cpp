/*
 *  Copyright (c) 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_update_command.h"

#include "kis_image_interfaces.h"
#include "kis_node.h"


KisUpdateCommand::KisUpdateCommand(KisNodeSP node, QRect dirtyRect,
                                   KisUpdatesFacade *updatesFacade,
                                   bool needsFullRefresh)
    : KUndo2Command("UPDATE_COMMAND"),
      m_node(node),
      m_dirtyRect(dirtyRect),
      m_updatesFacade(updatesFacade),
      m_needsFullRefresh(needsFullRefresh)
{
}

KisUpdateCommand::~KisUpdateCommand()
{
}

void KisUpdateCommand::undo()
{
    KUndo2Command::undo();
    update();
}

void KisUpdateCommand::redo()
{
    KUndo2Command::redo();
    update();
}

void KisUpdateCommand::update()
{
    if(m_needsFullRefresh) {
        m_updatesFacade->refreshGraphAsync(m_node, m_dirtyRect);
    }
    else {
        m_node->setDirty(m_dirtyRect);
    }
}
