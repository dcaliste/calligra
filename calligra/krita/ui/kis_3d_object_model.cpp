/* This file is part of the KDE project
 * Copyright (C) Lukáš Tvrdý, lukast.dev@gmail.com (c) 2009
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QHash>

#include "kis_3d_object_model.h"
#include "kis_vec.h"
#include "kis_debug.h"

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kcomponentdata.h>

Kis3DObjectModel::Kis3DObjectModel(const QString& model, const QString& material)
{
    KGlobal::mainComponent().dirs()->addResourceType("kis_brushmodels", "data", "krita/brushmodels/");
    QString path = KGlobal::mainComponent().dirs()->findResource("kis_brushmodels", model);

    m_cached = false;

    parseMaterial(material);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    QString key;
    QString edge;
    QStringList edgeList;

    while (!in.atEnd()) {

        QString line = in.readLine();
        if (line.isEmpty() || line[0] == '#') {
            continue;
        }

        QTextStream ts(&line);
        QString id;
        ts >> id;
        if (id == "v") {
            KisVector3D vector;
            for (int i = 0; i < 3; i++) {
                ts >> vector[i];
            }
            m_vertex.append(vector);
        } else if (id == "vn") {
            KisVector3D normal;
            for (int i = 0; i < 3; i++) {
                ts >> normal[i];

            }
            m_normal.append(normal);
        } else if (id == "usemtl") {
            ts >> key;
        } else if (id == "f") {
            if (key.isEmpty() || key.isNull()) {
                // TODO: will we support obj files without materials? I don't like that idea..
                qFatal("No material defined in model file!");
            }
            // triangles are supported so far and format f vertexIndex//vertexNormalIndex
            for (int i = 0; i < 3; i++) {
                ts >> edge;
                edgeList = edge.split('/');
                //m_vertexIndex.append( edgeList.value(0).toInt() - 1);
                //m_normalIndex.append( edgeList.value(2).toInt() - 1);
                m_vertexHash[ key ].append(edgeList.value(0).toInt() - 1);
                m_normalHash[ key ].append(edgeList.value(2).toInt() - 1);
            }
        }
    } // while

}

#define MODEL_SCALE 30
GLuint Kis3DObjectModel::displayList()
{
    if (m_cached) {
        return m_displayList;
    }

    // nothing to render
    if (m_vertex.size() == 0) {
        return 0;
    }

    KisVector3D vertex;
    KisVector3D normal;
    QList<QString> keyList = m_vertexHash.keys();

    m_displayList = glGenLists(1);

    glNewList(m_displayList, GL_COMPILE);
    glScalef(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);
    glBegin(GL_TRIANGLES);
    for (int k = 0; k < keyList.size(); k++) {
        m_vertexIndex = m_vertexHash.value(keyList[k]);
        m_normalIndex = m_normalHash.value(keyList[k]);

        Material m = m_material.value(keyList[k]);
        glColor3f(m.Kd[0], m.Kd[1], m.Kd[2]);

        for (int i = 0; i < m_vertexIndex.size(); i += 3) {
            for (int j = 0; j < 3; j++) {
                int vi = m_vertexIndex[i+j];
                int ni = m_normalIndex[i+j];
                vertex = m_vertex[ vi ];
                normal = m_normal[ ni ];
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(vertex.x(), vertex.y(), vertex.z());
            }
        }


    }


    glEnd();
    const GLfloat _1_MODEL_SCALE = GLfloat(1.0 / MODEL_SCALE);
    glScalef(_1_MODEL_SCALE, _1_MODEL_SCALE, _1_MODEL_SCALE);
    glEndList();
    /*        glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);

            glVertexPointer(3, GL_FLOAT, 0, (float *)m_vertex.data());
            glNormalPointer(GL_FLOAT, 0, (float *)m_normal.data());
            glDrawElements(GL_TRIANGLES, m_vertexIndex.size(), GL_UNSIGNED_INT, m_vertexIndex.data());

            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
    */


    // list is ready
    m_cached = true;
    return m_displayList;

}


void Kis3DObjectModel::parseMaterial(const QString& fileName)
{

    QString path = KGlobal::mainComponent().dirs()->findResource("kis_brushmodels", fileName);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);

    QString key;
    while (!in.atEnd()) {

        QString line = in.readLine();
        if (line.isEmpty() || line[0] == '#') {
            continue;
        }

        QTextStream ts(&line);
        QString id;
        ts >> id;
        if (id == "Ka") {
            KisVector3D vector;
            for (int i = 0; i < 3; i++) {
                ts >> vector[i];
            }
            m_material[ key ].Ka = vector;

        } else if (id == "Kd") {

            KisVector3D vector;
            for (int i = 0; i < 3; i++) {
                ts >> vector[i];
            }
            m_material[ key ].Kd = vector;

        } else if (id == "Ks") {
            KisVector3D vector;
            for (int i = 0; i < 3; i++) {
                ts >> vector[i];
            }
            m_material[ key ].Ks = vector;
        } else if (id == "d") {
            ts >> m_material[ key ].d;
        } else if (id == "Ns") {
            ts >> m_material[ key ].Ns;
        } else if (id == "Ni") {
            ts >> m_material[ key ].Ni;
        } else if (id == "newmtl") {
            ts >> key;
        }

    }
}
